#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/int32_multi_array.h>
#include <HardwareSerial.h>
#include <SCServo.h>

#include "ble_driver.h"

SMS_STS st;
#define S_RXD 44
#define S_TXD 43
#define LED_PIN 13

const int SERVO_COUNT = 6;
const int servoIDs[SERVO_COUNT] = {50, 77, 88, 99, 100, 110};

// micro-ROS 全局变量
rcl_publisher_t heartbeat_publisher;
rcl_publisher_t joint_state_publisher;
rcl_subscription_t joint_command_subscriber;

std_msgs__msg__Int32 heartbeat_msg;
std_msgs__msg__Int32MultiArray joint_state_msg;
std_msgs__msg__Int32MultiArray joint_command_msg;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t heartbeat_timer;
rcl_timer_t joint_state_timer;

// 位置数据 - 使用 volatile 因为在不同任务间共享
static volatile int32_t current_positions[SERVO_COUNT] = {0};
static volatile int32_t target_positions[SERVO_COUNT] = {2048, 2048, 2048, 2048, 2048, 2048};  // 初始化为中间位置
static volatile bool new_command = false;  // 标记是否有新命令

// 任务句柄
TaskHandle_t microros_task_handle;
TaskHandle_t servo_task_handle;

// 舵机控制任务 - 定期执行
void servo_control_task(void *parameter) {
    while(1) {
        if(new_command) {
            // 发送一包完整的舵机命令
            for(int i = 0; i < SERVO_COUNT; i++) {
                st.WritePosEx(servoIDs[i], target_positions[i], 1500, 50);
                vTaskDelay(pdMS_TO_TICKS(20));  // 每个舵机命令间延时20ms
            }
            new_command = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));  // 每50ms检查一次是否有新命令
    }
}

// 心跳回调 - 仅更新LED和计数
void heartbeat_timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        rcl_publish(&heartbeat_publisher, &heartbeat_msg, NULL);
        heartbeat_msg.data++;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

// 关节状态回调 - 仅读取和发布状态
void joint_state_timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        for(int i = 0; i < SERVO_COUNT; i++) {
            current_positions[i] = st.ReadPos(servoIDs[i]);
        }
        joint_state_msg.data.data = (int32_t*)current_positions;
        rcl_publish(&joint_state_publisher, &joint_state_msg, NULL);
    }
}

// 关节命令回调 - 仅更新目标位置
void joint_command_callback(const void * msgin) {
    const std_msgs__msg__Int32MultiArray * msg = (const std_msgs__msg__Int32MultiArray *)msgin;
    
    if (msg->data.size >= SERVO_COUNT) {
        for(size_t i = 0; i < SERVO_COUNT; i++) {
            target_positions[i] = msg->data.data[i];
        }
        new_command = true;  // 标记有新命令
    }
}

// micro-ROS 任务
void microros_task(void *parameter) {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    set_microros_serial_transports(Serial);
    delay(2000);
    
    allocator = rcl_get_default_allocator();
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "xiao_esp32s3_node", "", &support);
    
    // 创建发布器
    rclc_publisher_init_default(
        &heartbeat_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "xiao/heartbeat");
        
    rclc_publisher_init_default(
        &joint_state_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
        "xiao/joint_states");
        
    // 创建订阅器
    rclc_subscription_init_default(
        &joint_command_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
        "xiao/joint_commands");
    
    // 创建定时器
    rclc_timer_init_default(
        &heartbeat_timer,
        &support,
        RCL_MS_TO_NS(1000),
        heartbeat_timer_callback);
        
    rclc_timer_init_default(
        &joint_state_timer,
        &support,
        RCL_MS_TO_NS(100),
        joint_state_timer_callback);
    
    // 创建执行器
    rclc_executor_init(&executor, &support.context, 3, &allocator);
    rclc_executor_add_timer(&executor, &heartbeat_timer);
    rclc_executor_add_timer(&executor, &joint_state_timer);
    rclc_executor_add_subscription(
        &executor,
        &joint_command_subscriber,
        &joint_command_msg,
        &joint_command_callback,
        ON_NEW_DATA);
    
    // 初始化消息
    heartbeat_msg.data = 0;
    
    joint_state_msg.data.capacity = SERVO_COUNT;
    joint_state_msg.data.size = SERVO_COUNT;
    joint_state_msg.data.data = (int32_t*)current_positions;
    
    joint_command_msg.data.capacity = SERVO_COUNT;
    joint_command_msg.data.size = SERVO_COUNT;
    joint_command_msg.data.data = (int32_t*)target_positions;
    
    while(1) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
    st.pSerial = &Serial1;
    
    // 创建舵机控制任务
    xTaskCreate(
        servo_control_task,
        "Servo Control",
        2048,
        NULL,
        2,  // 优先级高于micro-ROS任务
        &servo_task_handle
    );
    
    // 创建micro-ROS任务
    xTaskCreate(
        microros_task,
        "micro-ROS Task",
        8192,
        NULL,
        1,
        &microros_task_handle
    );
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(100));
}