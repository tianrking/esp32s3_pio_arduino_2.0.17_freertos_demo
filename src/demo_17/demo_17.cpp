#include "demo_17.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

// 工具函数：获取任务状态的字符串表示
const char *getTaskStateString(eTaskState state) {
    switch (state) {
        case eRunning:   return "Running";
        case eReady:     return "Ready";
        case eBlocked:   return "Blocked";
        case eSuspended: return "Suspended";
        case eDeleted:   return "Deleted";
        default:         return "Invalid";
    }
}

// 修改后的打印任务信息函数
void printTaskInfo(TaskHandle_t taskHandle) {
    if (taskHandle == NULL) return;

    const char *taskName = pcTaskGetTaskName(taskHandle);
    eTaskState state = eTaskGetState(taskHandle);
    
    Serial.print("Task Name: ");
    Serial.println(taskName);
    Serial.print("Task State: ");
    Serial.println(getTaskStateString(state));
    Serial.print("Task Priority: ");
    Serial.println((unsigned int)uxTaskPriorityGet(taskHandle));
    Serial.print("Task Stack High Water Mark: ");
    Serial.println((unsigned int)uxTaskGetStackHighWaterMark(taskHandle));
    Serial.println();
}

// 打印系统信息
void printSystemInfo() {
    Serial.println("=== System Information ===");
    
    Serial.print("FreeRTOS Version: ");
    Serial.println(tskKERNEL_VERSION_NUMBER);
    
    Serial.print("Tick Rate Hz: ");
    Serial.println(configTICK_RATE_HZ);
    
    Serial.print("Up Time (ticks): ");
    Serial.println((unsigned int)xTaskGetTickCount());
    
    Serial.print("Up Time (ms): ");
    Serial.println((unsigned int)xTaskGetTickCount() * portTICK_PERIOD_MS);

    TaskHandle_t idleTaskHandle = xTaskGetIdleTaskHandle();
    if (idleTaskHandle != NULL) {
        Serial.print("Idle Task Stack High Water Mark: ");
        Serial.println((unsigned int)uxTaskGetStackHighWaterMark(idleTaskHandle));
    }
    Serial.println();
}

// 打印内存信息
void printMemoryInfo() {
    Serial.println("=== Memory Information ===");

    multi_heap_info_t heapInfo;
    heap_caps_get_info(&heapInfo, MALLOC_CAP_8BIT);
    
    Serial.print("Total Heap Size (Bytes): ");
    Serial.println(heapInfo.total_free_bytes + heapInfo.total_allocated_bytes);
    
    Serial.print("Free Heap Size (Bytes): ");
    Serial.println(heapInfo.total_free_bytes);
    
    Serial.print("Minimum Ever Free Heap Size (Bytes): ");
    Serial.println(heapInfo.minimum_free_bytes);
    
    Serial.print("Allocated Heap Blocks: ");
    Serial.println(heapInfo.allocated_blocks);
    
    Serial.print("Free Heap Blocks: ");
    Serial.println(heapInfo.free_blocks);
    
    Serial.print("Largest Free Block (Bytes): ");
    Serial.println(heapInfo.largest_free_block);
    Serial.println();
}

void monitorTask(void *pvParameters) {
    while (1) {
        Serial.println("=====================================");
        Serial.println("Resource Monitor");
        Serial.println("=====================================");
        Serial.println();

        // 打印系统信息
        printSystemInfo();

        // 打印内存信息
        printMemoryInfo();

        // 获取当前任务数量并打印
        UBaseType_t taskCount = uxTaskGetNumberOfTasks();
        Serial.print("Number of Tasks: ");
        Serial.println((unsigned int)taskCount);
        Serial.println();

        // 打印任务信息
        Serial.println("=== Task Information ===");
        
        // 打印空闲任务信息
        TaskHandle_t idleTask = xTaskGetIdleTaskHandle();
        if (idleTask) {
            printTaskInfo(idleTask);
        }
        
        // 打印当前任务信息
        TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
        if (currentTask) {
            printTaskInfo(currentTask);
        }

        Serial.println("=====================================");
        Serial.println();

        vTaskDelay(pdMS_TO_TICKS(5000)); // 每 5 秒打印一次
    }
}

void initialize_demo_17() {
    xTaskCreate(monitorTask, "Monitor Task", 4096, NULL, 1, NULL);
}