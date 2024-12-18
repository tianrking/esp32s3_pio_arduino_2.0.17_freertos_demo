#include <Arduino.h>
#include "config.h"

// 引入各个 demo 的头文件
#include "demo_01/demo_01.h"
#include "demo_02/demo_02.h"
#include "demo_03/demo_03.h"
#include "demo_04/demo_04.h"
#include "demo_05/demo_05.h"
#include "demo_06/demo_06.h"
#include "demo_07/demo_07.h"
#include "demo_08/demo_08.h"
#include "demo_09/demo_09.h"
#include "demo_10/demo_10.h"
#include "demo_11/demo_11.h"
#include "demo_12/demo_12.h"
#include "demo_13/demo_13.h"
#include "demo_14/demo_14.h"
#include "demo_15/demo_15.h"
#include "demo_16/demo_16.h"
#include "demo_17/demo_17.h"

void setup() {
  Serial.begin(115200);

  switch (DEMO_NUMBER) {
    case 1:
      initialize_demo_01();
      break;
    case 2:
      initialize_demo_02();
      break;
    case 3:
      initialize_demo_03();
      break;
    case 4:
      initialize_demo_04();
      break;
    case 5:
      initialize_demo_05();
      break;
    case 6:
      initialize_demo_06();
      break;
    case 7:
      initialize_demo_07();
      break;
    case 8:
      initialize_demo_08();
      break;
    case 9:
      initialize_demo_09();
      break;
    case 10:
      initialize_demo_10();
      break;
    case 11:
      initialize_demo_11();
      break;
    case 12:
      initialize_demo_12();
      break;
    case 13:
      initialize_demo_13();
      break;
    case 14:
      initialize_demo_14();
      break;
    case 15:
      initialize_demo_15();
      break;
    case 16:
      initialize_demo_16();
      break;

    case 17:
      initialize_demo_17();
      break;

    default:
      Serial.println("Invalid demo number!");
      break;
  }
}

void loop() {
  // FreeRTOS 任务调度，loop 函数为空
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}