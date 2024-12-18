#include "demo_07.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

EventGroupHandle_t xEventGroup = NULL;

// 定义事件位
#define BIT_0 (1 << 0) // 第 0 位
#define BIT_1 (1 << 1) // 第 1 位
#define BIT_2 (1 << 2) // 第 2 位

void task_set_bits(void *pvParameters) {
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    Serial.println("Setting BIT_0");
    xEventGroupSetBits(xEventGroup, BIT_0); // 设置 BIT_0

    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("Setting BIT_1");
    xEventGroupSetBits(xEventGroup, BIT_1); // 设置 BIT_1

    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Clearing BIT_0 and BIT_1");
    xEventGroupClearBits(xEventGroup, BIT_0 | BIT_1); // 清除 BIT_0 和 BIT_1

    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("Setting BIT_2");
    xEventGroupSetBits(xEventGroup, BIT_2); // 设置 BIT_2

    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Clearing BIT_2");
    xEventGroupClearBits(xEventGroup, BIT_2); // 清除 BIT_2
  }
}

void task_wait_bit_0(void *pvParameters) {
  while (1) {
    EventBits_t bits = xEventGroupWaitBits(xEventGroup, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY); // pdTRUE 清除BIT_0
    Serial.printf("task_wait_bit_0 - Bits set: 0x%X, BIT_0 detected and cleared.\n", bits);
  }
}

void task_wait_bit_0_and_1(void *pvParameters) {
  while (1) {
    // 等待 BIT_0 和 BIT_1 都被设置
    EventBits_t bits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_1, pdTRUE, pdTRUE, portMAX_DELAY); // pdTRUE 清除等待的位
    Serial.printf("task_wait_bit_0_and_1 - Bits set: 0x%X, BIT_0 and BIT_1 detected and cleared.\n", bits);
  }
}

void task_wait_bit_2(void *pvParameters) {
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(xEventGroup, BIT_2, pdTRUE, pdFALSE, portMAX_DELAY); // pdTRUE 清除 BIT_2
        Serial.printf("task_wait_bit_2 - Bits set: 0x%X, BIT_2 detected and cleared.\n", bits);
    }
}

void initialize_demo_07() {
  xEventGroup = xEventGroupCreate(); // 创建事件组

  xTaskCreate(task_set_bits, "Set Bits", 2048, NULL, 1, NULL);
  xTaskCreate(task_wait_bit_0, "Wait BIT_0", 2048, NULL, 2, NULL);
  xTaskCreate(task_wait_bit_0_and_1, "Wait BIT_0 and BIT_1", 2048, NULL, 2, NULL);
  xTaskCreate(task_wait_bit_2, "Wait BIT_2", 2048, NULL, 2, NULL);
}