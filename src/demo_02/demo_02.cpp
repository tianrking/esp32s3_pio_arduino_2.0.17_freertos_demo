#include "demo_02.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_low_priority(void *pvParameters) {
  while (1) {
    Serial.println("Task with LOW priority is running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task_high_priority(void *pvParameters) {
  while (1) {
    Serial.println("Task with HIGH priority is running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void initialize_demo_02() {
  // 创建低优先级任务
  xTaskCreate(task_low_priority, "Low Priority Task", 2048, NULL, 1, NULL);

  // 创建高优先级任务
  xTaskCreate(task_high_priority, "High Priority Task", 2048, NULL, 2, NULL);
}