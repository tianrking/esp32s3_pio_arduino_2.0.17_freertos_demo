#include "demo_15.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// 使用 static 关键字将这两个变量的作用域限制在当前文件
static TimerHandle_t xAutoReloadTimer = NULL;
static TimerHandle_t xOneShotTimer = NULL;

// 使用 static 关键字将这两个函数的作用域限制在当前文件
static void vAutoReloadTimerCallback(TimerHandle_t pxTimer) {
    Serial.println("Auto-reload timer callback executed (demo_15)");
}

static void vOneShotTimerCallback(TimerHandle_t pxTimer) {
    Serial.println("One-shot timer callback executed (demo_15)");
}

void initialize_demo_15() {
    // 创建自动重载定时器，周期为 2000ms
    xAutoReloadTimer = xTimerCreate("AutoReloadTimer_15", pdMS_TO_TICKS(2000), pdTRUE, (void *)0, vAutoReloadTimerCallback);

    // 创建单次定时器，周期为 5000ms
    xOneShotTimer = xTimerCreate("OneShotTimer_15", pdMS_TO_TICKS(5000), pdFALSE, (void *)0, vOneShotTimerCallback);

    if (xAutoReloadTimer != NULL && xOneShotTimer != NULL) {
        // 启动定时器
        xTimerStart(xAutoReloadTimer, 0);
        xTimerStart(xOneShotTimer, 0);
    }

    // 打印 Timer Task/Daemon Task 的优先级
    Serial.printf("Timer task priority: %d\n", configTIMER_TASK_PRIORITY);
}