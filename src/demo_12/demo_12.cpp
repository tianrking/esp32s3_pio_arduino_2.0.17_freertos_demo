#include "demo_12.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// 静态分配任务控制块 (TCB) 和堆栈
StaticTask_t xTaskBuffer;
StackType_t xTaskStack[1024];

// 静态分配定时器控制块
StaticTimer_t xTimerBuffer;

void vTaskFunction(void *pvParameters) {
    while (1) {
        Serial.println("Task running (statically allocated)");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTimerCallback(TimerHandle_t pxTimer) {
    Serial.println("Timer expired (statically allocated)");
}

void initialize_demo_12() {
    // 静态创建任务
    xTaskCreateStatic(
        vTaskFunction,
        "Static Task",
        1024,
        NULL,
        1,
        xTaskStack,
        &xTaskBuffer);

    // 静态创建定时器
    xTimerCreateStatic(
        "Static Timer",
        pdMS_TO_TICKS(5000),
        pdFALSE,
        (void *)0,
        vTimerCallback,
        &xTimerBuffer);
    xTimerStart(&xTimerBuffer,0);
}