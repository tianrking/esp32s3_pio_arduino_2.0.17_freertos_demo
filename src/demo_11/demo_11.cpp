#include "demo_11.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"

StreamBufferHandle_t xStreamBuffer = NULL;

void task_send_stream(void *pvParameters) {
    const char *data = "Stream data...";
    size_t dataLength = strlen(data);
    size_t sentBytes = 0;
    size_t offset = 0;

    while (1) {
        sentBytes = xStreamBufferSend(xStreamBuffer, (void *)(data + offset), dataLength - offset, portMAX_DELAY);
        offset = (offset + sentBytes) % dataLength; // 循环发送
        Serial.printf("Sent %d bytes to stream buffer\n", sentBytes);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_receive_stream(void *pvParameters) {
    char buffer[20];
    while (1) {
        size_t receivedBytes = xStreamBufferReceive(xStreamBuffer, (void *)buffer, sizeof(buffer) - 1, portMAX_DELAY);
        buffer[receivedBytes] = '\0'; // 确保字符串结束符
        Serial.printf("Received %d bytes: %s\n", receivedBytes, buffer);
    }
}

void initialize_demo_11() {
    // 创建流缓冲区，大小为 100 字节，触发级别为 1 字节
    xStreamBuffer = xStreamBufferCreate(100, 1);

    xTaskCreate(task_send_stream, "Send Stream", 2048, NULL, 1, NULL);
    xTaskCreate(task_receive_stream, "Receive Stream", 2048, NULL, 2, NULL);
}