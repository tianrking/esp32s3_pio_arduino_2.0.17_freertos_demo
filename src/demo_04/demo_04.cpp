#include "demo_04.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xMutex = NULL;

void task_access_resource(void *pvParameters) {
  const char *taskName = pcTaskGetName(NULL);
  while (1) {
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      Serial.printf("Task %s: accessing resource...\n", taskName);
      // 模拟访问共享资源
      vTaskDelay(500 / portTICK_PERIOD_MS);
      xSemaphoreGive(xMutex); // 释放互斥量
      Serial.printf("Task %s: releasing resource\n", taskName);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void initialize_demo_04() {
  xMutex = xSemaphoreCreateMutex(); // 创建互斥量

  xTaskCreate(task_access_resource, "Task A", 2048, NULL, 1, NULL);
  xTaskCreate(task_access_resource, "Task B", 2048, NULL, 1, NULL);
}