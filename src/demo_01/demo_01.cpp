#include "demo_01.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task1(void *pvParameters) {
  while (1) {
    Serial.println("Task 1 is running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void *pvParameters) {
  while (1) {
    Serial.println("Task 2 is running");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void initialize_demo_01() {
  xTaskCreate(task1, "Task 1", 2048, NULL, 1, NULL);
  xTaskCreate(task2, "Task 2", 2048, NULL, 2, NULL);
}