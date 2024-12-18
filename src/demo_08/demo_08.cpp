#include "demo_08.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t xTaskToNotify = NULL;

void task_send_notification(void *pvParameters) {
  while (1) {
    Serial.println("Sending notification");
    xTaskNotifyGive(xTaskToNotify); // 发送通知
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void task_receive_notification(void *pvParameters) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 接收通知，清除通知值
    Serial.println("Notification received");
  }
}

void initialize_demo_08() {
  xTaskCreate(task_receive_notification, "Receive Notification", 2048, NULL, 2, &xTaskToNotify);
  xTaskCreate(task_send_notification, "Send Notification", 2048, NULL, 1, NULL);
}