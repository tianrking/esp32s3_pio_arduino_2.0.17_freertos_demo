#include "demo_13.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

void task_long_delay(void *pvParameters) {
    while (1) {
        Serial.println("Task going to sleep for 30 seconds");
        vTaskDelay(pdMS_TO_TICKS(30000)); // 30 秒
        Serial.println("Task woke up");
    }
}

void initialize_demo_13() {
    // 启用深度睡眠模式下的 Tickless Idle
    //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); //错误方法 ESP_PD_DOMAIN_RTC_PERIPH 在深度睡眠时掉电
    esp_sleep_enable_timer_wakeup(30 * 1000000); // 设置定时器唤醒，这里设置为 30 秒，与任务延时相匹配

    xTaskCreate(task_long_delay, "Long Delay Task", 2048, NULL, 1, NULL);
}