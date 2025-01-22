// // microros_driver.h
// #ifndef MICROROS_DRIVER_H
// #define MICROROS_DRIVER_H

// #include <Arduino.h>
// #include <micro_ros_platformio.h>
// #include <rcl/rcl.h>
// #include <rclc/rclc.h>
// #include <rclc/executor.h>
// #include <std_msgs/msg/int32_multi_array.h>
// #include <std_msgs/msg/int32.h>

// #define JOINT_COUNT 6

// class MicroROSDriver {
// public:
//     MicroROSDriver();
//     void init();
//     void spin();
//     static void task(void *parameter);

// private:
//     // micro-ROS variables
//     rcl_subscription_t joint_command_subscriber;
//     rcl_publisher_t heartbeat_publisher;
//     std_msgs__msg__Int32MultiArray joint_command_msg;
//     std_msgs__msg__Int32 heartbeat_msg;
    
//     rclc_executor_t executor;
//     rclc_support_t support;
//     rcl_allocator_t allocator;
//     rcl_node_t node;
//     rcl_timer_t timer;

//     static void error_loop();
//     static void joint_command_callback(const void * msgin);
//     static void timer_callback(rcl_timer_t * timer, int64_t last_call_time);
    
//     // Current joint positions
//     int32_t joint_positions[JOINT_COUNT];
// };

// #endif // MICROROS_DRIVER.H