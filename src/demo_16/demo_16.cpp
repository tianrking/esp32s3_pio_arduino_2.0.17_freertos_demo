#include "demo_16.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

// 使用 static 关键字将这两个变量的作用域限制在当前文件
static EventGroupHandle_t xEventGroup = NULL;
static QueueHandle_t xQueue = NULL;

#define BIT_EVENT_1 (1 << 0)
#define BIT_EVENT_2 (1 << 1)

typedef struct {
    int data;
    char source[20];
} Item;

void task_producer(void *pvParameters) {
    int count = 0;
    while (1) {
        count++;
        Item item;
        item.data = count;
        sprintf(item.source, "Producer");

        // 发送数据到队列
        xQueueSend(xQueue, &item, portMAX_DELAY);
        Serial.printf("Producer sent data: %d\n", count);

        // 每隔 3 次发送数据后，设置一个事件位
        if (count % 3 == 0) {
            xEventGroupSetBits(xEventGroup, BIT_EVENT_1);
            Serial.println("Producer set event: BIT_EVENT_1");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_consumer_event(void *pvParameters) {
    while (1) {
        // 等待事件位
        EventBits_t bits = xEventGroupWaitBits(xEventGroup, BIT_EVENT_1, pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & BIT_EVENT_1) {
            Serial.println("Consumer Event received event: BIT_EVENT_1");
            // 可以在这里执行一些与事件相关的操作
        }
    }
}

void task_consumer_queue(void *pvParameters) {
    while (1) {
        Item item;
        // 从队列接收数据
        if (xQueueReceive(xQueue, &item, portMAX_DELAY) == pdTRUE) {
            Serial.printf("Consumer Queue received data: %d from %s\n", item.data, item.source);

            // 如果数据大于 5，则设置另一个事件位
            if (item.data > 5) {
                xEventGroupSetBits(xEventGroup, BIT_EVENT_2);
                Serial.println("Consumer Queue set event: BIT_EVENT_2");
            }
        }
    }
}

void initialize_demo_16() {
    // 创建事件组
    xEventGroup = xEventGroupCreate();
    // 创建队列
    xQueue = xQueueCreate(10, sizeof(Item));

    xTaskCreate(task_producer, "Producer", 2048, NULL, 1, NULL);
    xTaskCreate(task_consumer_event, "Consumer Event", 2048, NULL, 2, NULL);
    xTaskCreate(task_consumer_queue, "Consumer Queue", 2048, NULL, 2, NULL);
}