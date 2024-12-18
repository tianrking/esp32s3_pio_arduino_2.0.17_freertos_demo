#include "demo_05.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

QueueHandle_t xQueue = NULL;

void task_send_to_queue(void *pvParameters) {
  int count = 0;
  while (1) {
    count++;
    Serial.printf("Sending data to queue: %d\n", count);
    xQueueSend(xQueue, &count, portMAX_DELAY); // 发送数据到队列，永久等待
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task_receive_from_queue(void *pvParameters) {
  int received_data;
  while (1) {
    if (xQueueReceive(xQueue, &received_data, portMAX_DELAY) == pdTRUE) { // 从队列接收数据，永久等待
      Serial.printf("Received data from queue: %d\n", received_data);
    }
  }
}

void initialize_demo_05() {
  xQueue = xQueueCreate(5, sizeof(int)); // 创建一个队列，长度为 5，每个元素大小为 sizeof(int)

  xTaskCreate(task_send_to_queue, "Send to Queue", 2048, NULL, 1, NULL);
  xTaskCreate(task_receive_from_queue, "Receive from Queue", 2048, NULL, 2, NULL);
}