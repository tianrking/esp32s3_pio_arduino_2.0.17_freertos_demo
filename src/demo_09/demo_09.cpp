#include "demo_09.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define WATCHDOG_TIMEOUT_S 3 // 看门狗超时时间 (秒)

void task_with_watchdog(void *pvParameters) {
    while (1) {
        // 喂狗
        esp_task_wdt_reset();
        Serial.println("Task is running and watchdog has been fed");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_without_watchdog(void *pvParameters) {
    while (1) {
        Serial.println("Task is running without feeding watchdog");
        // 此任务将导致看门狗超时
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

void initialize_demo_09() {
    // 初始化任务看门狗
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);

    // 创建带有看门狗的任务
    xTaskCreate(task_with_watchdog, "Task with Watchdog", 2048, NULL, 2, NULL);
    esp_task_wdt_add(NULL); // 将当前任务添加到看门狗监视列表 (setup() 也在一个任务中)

    // 创建不带看门狗的任务 (将触发看门狗复位)
    //xTaskCreate(task_without_watchdog, "Task without Watchdog", 2048, NULL, 1, NULL);
}