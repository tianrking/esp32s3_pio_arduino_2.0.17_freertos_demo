#include "demo_03.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore = NULL;

void task_give_semaphore(void *pvParameters) {
  while (1) {
    Serial.println("Task GIVE semaphore");
    xSemaphoreGive(xSemaphore); // 释放信号量
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void task_take_semaphore(void *pvParameters) {
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) { // 获取信号量，永久等待
      Serial.println("Task TAKE semaphore: performing action...");
      // 执行需要信号量保护的操作
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void initialize_demo_03() {
  xSemaphore = xSemaphoreCreateBinary(); // 创建二进制信号量

  xTaskCreate(task_give_semaphore, "Give Semaphore", 2048, NULL, 1, NULL);
  xTaskCreate(task_take_semaphore, "Take Semaphore", 2048, NULL, 2, NULL);
}