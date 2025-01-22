// // microros_driver.cpp
// #include "microros_driver.h"

// #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
// #define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// // 静态成员用于在回调中访问实例
// static MicroROSDriver* g_instance = nullptr;

// MicroROSDriver::MicroROSDriver() {
//     // Initialize joint positions
//     for(int i = 0; i < JOINT_COUNT; i++) {
//         joint_positions[i] = 0;
//     }
//     // 存储实例指针供回调使用
//     g_instance = this;
// }

// void MicroROSDriver::error_loop() {
//     while(1) {
//         delay(100);
//     }
// }

// void MicroROSDriver::timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
//     RCLC_UNUSED(last_call_time);
//     if (timer != NULL && g_instance != nullptr) {
//         // 发布心跳消息
//         RCSOFTCHECK(rcl_publish(&g_instance->heartbeat_publisher, &g_instance->heartbeat_msg, NULL));
//         g_instance->heartbeat_msg.data++;
//     }
// }

// void MicroROSDriver::joint_command_callback(const void * msgin) {
//     const std_msgs__msg__Int32MultiArray * msg = (const std_msgs__msg__Int32MultiArray *)msgin;
    
//     if (g_instance != nullptr) {
//         // 处理接收到的关节命令
//         for(size_t i = 0; i < msg->data.size && i < JOINT_COUNT; i++) {
//             g_instance->joint_positions[i] = msg->data.data[i];
//             // 这里添加舵机控制代码
//             // st.WritePosEx(servoIDs[i], msg->data.data[i], speed, acc);
//         }
//     }
// }

// void MicroROSDriver::init() {
//     set_microros_serial_transports(Serial);
//     delay(2000);

//     allocator = rcl_get_default_allocator();

//     // 创建初始化选项
//     RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

//     // 创建节点
//     RCCHECK(rclc_node_init_default(&node, "arm_controller_node", "", &support));

//     // 创建关节命令订阅器
//     RCCHECK(rclc_subscription_init_default(
//         &joint_command_subscriber,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
//         "joint_commands"));

//     // 创建心跳发布器
//     RCCHECK(rclc_publisher_init_default(
//         &heartbeat_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "xiao"));

//     // 创建心跳定时器
//     const unsigned int timer_timeout = 1000;
//     RCCHECK(rclc_timer_init_default(
//         &timer,
//         &support,
//         RCL_MS_TO_NS(timer_timeout),
//         timer_callback));

//     // 创建执行器
//     RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
//     RCCHECK(rclc_executor_add_subscription(
//         &executor,
//         &joint_command_subscriber,
//         &joint_command_msg,
//         &joint_command_callback,
//         ON_NEW_DATA));
//     RCCHECK(rclc_executor_add_timer(&executor, &timer));

//     // 初始化心跳消息
//     heartbeat_msg.data = 0;
// }

// void MicroROSDriver::spin() {
//     RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
//     delay(50);
// }

// void MicroROSDriver::task(void *parameter) {
//     MicroROSDriver* driver = new MicroROSDriver();
//     driver->init();
    
//     while(1) {
//         driver->spin();
//         vTaskDelay(pdMS_TO_TICKS(50));
//     }
// }