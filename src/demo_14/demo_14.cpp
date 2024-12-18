#include "demo_14.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_0 // 使用 BOOT 按钮

volatile bool buttonPressed = false;

// 中断服务程序 (ISR)
void IRAM_ATTR button_isr_handler(void* arg) {
    // 简单的去抖动
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = xTaskGetTickCountFromISR();
    if (interrupt_time - last_interrupt_time > pdMS_TO_TICKS(200)) {
        buttonPressed = true;
    }
    last_interrupt_time = interrupt_time;
}

void task_process_button_press(void *pvParameters) {
    while (1) {
        if (buttonPressed) {
            buttonPressed = false;
            Serial.println("Button pressed!");
            // 执行其他操作...
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void initialize_demo_14() {
    // 配置 GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // 下降沿触发
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // 安装 GPIO 中断服务
    gpio_install_isr_service(0);

    // 挂钩 ISR 到 GPIO
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, (void*) BUTTON_PIN);

    xTaskCreate(task_process_button_press, "Process Button Press", 2048, NULL, 2, NULL);
}