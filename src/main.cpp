#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <HardwareSerial.h>

// 创建硬件串口对象用于舵机控制
HardwareSerial ServoSerial(1); // UART1, 使用D6(GPIO43, TX)和D7(GPIO44, RX)

// micro-ROS 全局变量
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

// FreeRTOS 任务句柄
TaskHandle_t microros_task_handle;
TaskHandle_t servo_task_handle;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Error handle loop
void error_loop() {
  while(1) {
    delay(100);
  }
}

// Timer callback function
void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
    Serial.println("Published: " + String(msg.data));
  }
}

// 舵机控制任务
void servo_control_task(void *parameter) {
  // 初始化舵机串口，使用GPIO43(TX)和GPIO44(RX)
  ServoSerial.begin(115200, SERIAL_8N1, 44, 43); // RX=44(D7), TX=43(D6)

  // 定期发送测试数据到舵机
  while(1) {
    // 示例：发送舵机控制命令
    // 这里替换成你的舵机实际控制协议
    ServoSerial.write(0xFF);
    ServoSerial.write(0x01);
    ServoSerial.write(90); // 舵机角度
    ServoSerial.write(0xFE);
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// micro-ROS 任务函数
void microros_task(void *parameter) {
  // USB CDC已经是默认的Serial，用于micro-ROS通信
  Serial.begin(115200);
  while (!Serial) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  
  set_microros_serial_transports(Serial);
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "xiao_esp32s3_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "xiao_counter"));

  // create timer
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  msg.data = 0;

  // micro-ROS 主循环
  while(1) {
    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup() {
  // 创建 micro-ROS 任务
  xTaskCreate(
    microros_task,          /* 任务函数 */
    "micro-ROS Task",       /* 任务名称 */
    8192,                   /* 堆栈大小 */
    NULL,                   /* 任务参数 */
    1,                      /* 任务优先级 */
    &microros_task_handle   /* 任务句柄 */
  );

  // 创建舵机控制任务
  xTaskCreate(
    servo_control_task,     /* 任务函数 */
    "Servo Control",        /* 任务名称 */
    4096,                   /* 堆栈大小 */
    NULL,                   /* 任务参数 */
    1,                      /* 任务优先级 */
    &servo_task_handle      /* 任务句柄 */
  );
}

void loop() {
  // FreeRTOS 调度器会处理任务调度
  vTaskDelay(pdMS_TO_TICKS(100));
}