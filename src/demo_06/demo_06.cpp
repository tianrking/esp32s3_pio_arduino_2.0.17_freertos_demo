#include "demo_06.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// 定时器句柄
TimerHandle_t xOneShotTimer = NULL;
TimerHandle_t xAutoReloadTimer = NULL;
TimerHandle_t xManualTimer = NULL;

// 定时器回调函数
void vOneShotTimerCallback(TimerHandle_t pxTimer) {
  Serial.println("One-shot timer callback executed!");
}

void vAutoReloadTimerCallback(TimerHandle_t pxTimer) {
  Serial.println("Auto-reload timer callback executed!");
}

void vManualTimerCallback(TimerHandle_t pxTimer) {
  const char *pcTimerName;
  pcTimerName = pcTimerGetName(pxTimer);
  Serial.printf("Manual timer callback executed! Timer name: %s\n", pcTimerName);
}

void task_control_timer(void *pvParameters) {
    // 5秒后启动手动定时器
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Starting Manual Timer");
    xTimerStart(xManualTimer, 0);
    
    // 15秒后停止手动定时器
    vTaskDelay(pdMS_TO_TICKS(10000));
    Serial.println("Stopping Manual Timer");
    xTimerStop(xManualTimer,0);
    
    // 20秒后启动手动定时器 并且修改周期为1秒
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Starting Manual Timer and change period");
    xTimerChangePeriod(xManualTimer,pdMS_TO_TICKS(1000),0);
    xTimerStart(xManualTimer, 0);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}

void initialize_demo_06() {
  // 创建单次定时器，周期为 3000ms
  xOneShotTimer = xTimerCreate("OneShotTimer", pdMS_TO_TICKS(3000), pdFALSE, (void *)0, vOneShotTimerCallback);

  // 创建多次定时器，周期为 2000ms
  xAutoReloadTimer = xTimerCreate("AutoReloadTimer", pdMS_TO_TICKS(2000), pdTRUE, (void *)0, vAutoReloadTimerCallback);

  // 创建手动定时器，不自动启动，周期设置为 10000ms 仅仅是为了演示启动 停止  修改周期的API使用
  xManualTimer = xTimerCreate("ManualTimer", pdMS_TO_TICKS(10000), pdTRUE, (void *)0, vManualTimerCallback);

  // 启动单次定时器和自动重载定时器
  if (xOneShotTimer != NULL && xAutoReloadTimer != NULL) {
    xTimerStart(xOneShotTimer, 0);
    xTimerStart(xAutoReloadTimer, 0);
    Serial.println("One-shot and auto-reload timers started");
  }

  // 创建控制任务
  xTaskCreate(task_control_timer, "Control Timer", 2048, NULL, 1, NULL);
}