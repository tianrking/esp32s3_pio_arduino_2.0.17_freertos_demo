#include "demo_10.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"

MessageBufferHandle_t xMessageBuffer = NULL;

void task_send_message(void *pvParameters) {
    const char *message = "Hello from sender!";
    while (1) {
        size_t sentBytes = xMessageBufferSend(xMessageBuffer, (void *)message, strlen(message) + 1, portMAX_DELAY);
        Serial.printf("Sent %d bytes to message buffer\n", sentBytes);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void task_receive_message(void *pvParameters) {
    char buffer[50];
    while (1) {
        size_t receivedBytes = xMessageBufferReceive(xMessageBuffer, (void *)buffer, sizeof(buffer), portMAX_DELAY);
        Serial.printf("Received %d bytes: %s\n", receivedBytes, buffer);
    }
}

void initialize_demo_10() {
    // 创建消息缓冲区，大小为 100 字节
    xMessageBuffer = xMessageBufferCreate(100);

    xTaskCreate(task_send_message, "Send Message", 2048, NULL, 1, NULL);
    xTaskCreate(task_receive_message, "Receive Message", 2048, NULL, 2, NULL);
}