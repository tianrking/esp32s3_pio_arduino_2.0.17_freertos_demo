#include <Arduino.h>
#include "config.h"

// 定义函数指针类型
typedef void (*InitFunctionPtr)();

// 定义 DemoEntry 结构体
typedef struct {
    int demoNumber;
    InitFunctionPtr initFunction;
} DemoEntry;

// 定义 demoTable 结构体数组
DemoEntry demoTable[] = {
    {1, initialize_demo_01},
    {2, initialize_demo_02},
    {3, initialize_demo_03},
    {4, initialize_demo_04},
    {5, initialize_demo_05},
    {6, initialize_demo_06},
    {7, initialize_demo_07},
    {8, initialize_demo_08},
    {9, initialize_demo_09},
    {10, initialize_demo_10},
    {11, initialize_demo_11},
    {12, initialize_demo_12},
    {13, initialize_demo_13},
    {14, initialize_demo_14},
    {15, initialize_demo_15},
    {16, initialize_demo_16},
    {17, initialize_demo_17}
    // ... 添加后续的 demo
};

void setup() {
    Serial.begin(115200);
    bool demoFound = false;
    // 遍历 demoTable 数组
    for (int i = 0; i < sizeof(demoTable) / sizeof(demoTable[0]); i++) {
        if (demoTable[i].demoNumber == DEMO_NUMBER) {
            // 调用对应的初始化函数
            demoTable[i].initFunction();
            demoFound = true;
            break;
        }
    }
    if (!demoFound) {
        Serial.println("Invalid demo number!");
    }
}

void loop() {
    // FreeRTOS 任务调度，loop 函数为空
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}