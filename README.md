**目录和文件解释:**

*   **`include/`**: 存放项目头文件。
    *   **`config.h`**: 项目配置文件，目前主要用于通过 `#define DEMO_NUMBER` 选择要运行的 demo。
*   **`src/`**: 存放项目源代码。
    *   **`main.cpp`**: 主程序入口，根据 `config.h` 中的 `DEMO_NUMBER` 宏定义，初始化并运行对应的 demo。
    *   **`demo_xx/`**: 每个 `demo_xx` 文件夹包含一个特定的 FreeRTOS 示例程序。
        *   **`demo_xx.cpp`**:  demo 的主要 C++ 代码实现。
        *   **`demo_xx.h`**: demo 的头文件，通常包含函数声明和全局变量的 `extern` 声明。
    *   **`utils/`**: 存放通用的实用工具函数。
        *   **`utils.cpp`**: 实用工具函数的 C++ 代码实现。
        *   **`utils.h`**: 实用工具函数的头文件。
*   **`lib/`**: 存放项目依赖的外部库。
*   **`platformio.ini`**: PlatformIO 项目的配置文件，用于配置项目使用的开发板、框架、编译选项等。
*   **`sdkconfig.defaults`**:  用于配置 ESP-IDF 的特定选项，例如启用 FreeRTOS 的某些特性。
*   **`README.md`**: 项目说明文档。

**每个 Demo 的详细解释:**

**`demo_01`：任务创建与删除**

*   **知识点:**  `xTaskCreate()`, `vTaskDelete()`
*   **功能:** 演示如何创建和删除 FreeRTOS 任务。创建两个任务 `task1` 和 `task2`，它们分别以不同的频率打印信息。
*   **代码:**
    *   `xTaskCreate()` 用于创建任务，指定任务函数、任务名称、堆栈大小、参数、优先级和任务句柄。
    *   `vTaskDelete()` 用于删除任务，传入任务句柄。

**`demo_02`：任务优先级**

*   **知识点:**  `xTaskCreate()`, 任务优先级
*   **功能:** 演示 FreeRTOS 中任务优先级的概念。创建两个任务，一个优先级较高，一个优先级较低，观察它们的执行顺序。
*   **代码:**
    *   `xTaskCreate()` 创建任务时，通过 `uxPriority` 参数设置任务优先级，数字越大优先级越高。

**`demo_03`：信号量 (Semaphore)**

*   **知识点:**  `xSemaphoreCreateBinary()`, `xSemaphoreGive()`, `xSemaphoreTake()`
*   **功能:** 演示如何使用二进制信号量进行任务同步。一个任务释放信号量，另一个任务获取信号量。
*   **代码:**
    *   `xSemaphoreCreateBinary()` 创建一个二进制信号量。
    *   `xSemaphoreGive()` 释放信号量。
    *   `xSemaphoreTake()` 获取信号量，如果信号量不可用，则阻塞等待。

**`demo_04`：互斥量 (Mutex)**

*   **知识点:**  `xSemaphoreCreateMutex()`, `xSemaphoreTake()`, `xSemaphoreGive()`
*   **功能:** 演示如何使用互斥量保护共享资源。两个任务尝试访问同一个共享资源 (模拟)，使用互斥量确保同一时间只有一个任务可以访问。
*   **代码:**
    *   `xSemaphoreCreateMutex()` 创建一个互斥量。
    *   `xSemaphoreTake()` 获取互斥量，如果互斥量已被占用，则阻塞等待。
    *   `xSemaphoreGive()` 释放互斥量。

**`demo_05`：队列 (Queue)**

*   **知识点:**  `xQueueCreate()`, `xQueueSend()`, `xQueueReceive()`
*   **功能:** 演示如何使用队列进行任务间通信。一个任务向队列发送数据，另一个任务从队列接收数据。
*   **代码:**
    *   `xQueueCreate()` 创建一个队列，指定队列长度和每个元素的大小。
    *   `xQueueSend()` 向队列发送数据，如果队列已满，则阻塞等待。
    *   `xQueueReceive()` 从队列接收数据，如果队列为空，则阻塞等待。

**`demo_06`：定时器 (Timer)**

*   **知识点:**  `xTimerCreate()`, `xTimerStart()`, `xTimerStop()`, `xTimerChangePeriod()`, 软件定时器回调函数
*   **功能:** 演示如何创建和使用软件定时器，包括单次定时器、周期性定时器和手动控制定时器。
*   **代码:**
    *   `xTimerCreate()` 创建一个软件定时器，指定定时器名称、周期、是否自动重载、定时器 ID 和回调函数。
    *   `xTimerStart()` 启动定时器。
    *   `xTimerStop()` 停止定时器。
    *   `xTimerChangePeriod()` 改变定时器周期。
    *   定时器到期时，会调用注册的回调函数。

**`demo_07`：事件组 (Event Group)**

*   **知识点:**  `xEventGroupCreate()`, `xEventGroupSetBits()`, `xEventGroupClearBits()`, `xEventGroupWaitBits()`
*   **功能:** 演示如何使用事件组进行任务同步。一个任务设置事件位，其他任务等待事件位，并可以设置超时和清除事件位。
*   **代码:**
    *   `xEventGroupCreate()` 创建一个事件组。
    *   `xEventGroupSetBits()` 设置事件组中的指定位。
    *   `xEventGroupClearBits()` 清除事件组中的指定位。
    *   `xEventGroupWaitBits()` 等待指定的事件位被设置，可以指定超时时间。

**`demo_08`：任务通知 (Task Notification)**

*   **知识点:**  `xTaskNotifyGive()`, `ulTaskNotifyTake()`, `xTaskNotify()`
*   **功能:** 演示如何使用任务通知进行任务间通信和同步。一个任务发送通知，另一个任务接收通知。
*   **代码:**
    *   `xTaskNotifyGive()` 向指定任务发送一个通知，使任务的通知值加一。
    *   `ulTaskNotifyTake()` 获取任务的通知值，可以选择获取后清零或不清零。
    *   `xTaskNotify()` 可以用来发送带有特定值的通知，以及执行更复杂的操作，例如设置任务通知值、替换任务通知值等.

**`demo_09`：任务看门狗 (Task Watchdog)**

*   **知识点:**  `esp_task_wdt_init()`, `esp_task_wdt_add()`, `esp_task_wdt_reset()`
*   **功能:** 演示如何使用 ESP32 的任务看门狗 (Task Watchdog) 来监控任务是否正常运行。
*   **代码:**
    *   `esp_task_wdt_init()` 初始化任务看门狗。
    *   `esp_task_wdt_add()` 将任务添加到看门狗的监控列表。
    *   `esp_task_wdt_reset()` 重置看门狗计数器 (喂狗)。

**`demo_10`：消息缓冲区 (Message Buffers)**

*   **知识点:**  `xMessageBufferCreate()`, `xMessageBufferSend()`, `xMessageBufferReceive()`
*   **功能:** 演示如何使用消息缓冲区进行任务间通信，消息缓冲区适用于传递可变长度的消息。
*   **代码:**
    *   `xMessageBufferCreate()` 创建一个消息缓冲区。
    *   `xMessageBufferSend()` 向消息缓冲区发送数据。
    *   `xMessageBufferReceive()` 从消息缓冲区接收数据。

**`demo_11`：流缓冲区 (Stream Buffers)**

*   **知识点:**  `xStreamBufferCreate()`, `xStreamBufferSend()`, `xStreamBufferReceive()`
*   **功能:** 演示如何使用流缓冲区进行任务间通信，流缓冲区适用于传递字节流数据。
*   **代码:**
    *   `xStreamBufferCreate()` 创建一个流缓冲区。
    *   `xStreamBufferSend()` 向流缓冲区发送数据。
    *   `xStreamBufferReceive()` 从流缓冲区接收数据。

**`demo_12`：静态分配 (Static Allocation)**

*   **知识点:**  `StaticTask_t`, `StackType_t`, `StaticTimer_t`, `xTaskCreateStatic()`, `xTimerCreateStatic()`
*   **功能:** 演示如何使用静态分配的方式创建任务和定时器，与动态分配 (`xTaskCreate`, `xTimerCreate`) 相对应。
*   **代码:**
    *   `StaticTask_t` 和 `StackType_t` 用于静态分配任务控制块和堆栈。
    *   `StaticTimer_t` 用于静态分配定时器控制块。
    *   `xTaskCreateStatic()` 使用静态分配的内存创建任务。
    *   `xTimerCreateStatic()` 使用静态分配的内存创建定时器。

**`demo_13`：低功耗定时器 (Tickless Idle)**

*   **知识点:**  `CONFIG_FREERTOS_USE_TICKLESS_IDLE`, `esp_sleep_enable_timer_wakeup()`
*   **功能:** 演示如何启用 FreeRTOS 的 Tickless Idle 模式，并通过 `vTaskDelay()` 进入低功耗状态，以及如何使用 ESP32 的定时器唤醒功能。
*   **代码:**
    *   需要启用 `CONFIG_FREERTOS_USE_TICKLESS_IDLE` 选项,在 `sdkconfig` 文件中启用或者使能 `Component config -> FreeRTOS -> Tickless idle support`。
    *   `esp_sleep_enable_timer_wakeup()` 配置 ESP32 在深度睡眠模式下使用定时器唤醒, 需要启用 `Component config -> Power Management -> Timer wakeup`。

**`demo_14`：中断处理 (Interrupt Handling)**

*   **知识点:**  `gpio_config()`, `gpio_install_isr_service()`, `gpio_isr_handler_add()`, `IRAM_ATTR`
*   **功能:** 演示如何在 ESP32 上配置 GPIO 中断，并在中断服务程序 (ISR) 中进行简单的处理。
*   **代码:**
    *   `gpio_config()` 配置 GPIO 引脚。
    *   `gpio_install_isr_service()` 安装 GPIO 中断服务。
    *   `gpio_isr_handler_add()` 将中断服务程序 (ISR) 挂钩到指定的 GPIO 引脚。
    *   `IRAM_ATTR` 属性将 ISR 函数放到 IRAM 中，以提高中断响应速度。

**`demo_15`：软件定时器服务程序任务 (Timer Service/Daemon Task)**

*   **知识点:**  `configTIMER_TASK_PRIORITY`, 软件定时器服务程序任务
*   **功能:** 演示 FreeRTOS 中软件定时器服务程序任务 (Timer Service/Daemon Task) 的存在，并打印其优先级。
*   **代码:**
    *   创建并启动定时器。
    *   使用 `configTIMER_TASK_PRIORITY` 宏打印软件定时器服务程序任务的优先级。

**`demo_16`：事件组与队列组合使用 (Combining Event Groups and Queues)**

*   **知识点:**  `xEventGroupCreate()`, `xQueueCreate()`, `xEventGroupSetBits()`, `xEventGroupWaitBits()`, `xQueueSend()`, `xQueueReceive()`
*   **功能:** 演示如何将事件组和队列组合使用，实现更复杂的任务间通信和同步。
*   **代码:**
    *   一个生产者任务向队列发送数据，并根据特定条件设置事件组中的事件位。
    *   一个消费者任务等待事件位，并在事件发生时从队列中接收数据。

**`demo_17`：系统状态监控**

*   **知识点:** `vTaskGetInfo()`, `uxTaskGetSystemState()`, `xTaskGetTickCount()`, `xTaskGetIdleTaskHandle()`,  `heap_caps_get_info()`, `pvPortMalloc()`, `vPortFree()`, `configTICK_RATE_HZ`, `tskKERNEL_VERSION_NUMBER`
*   **功能:** 演示如何使用 FreeRTOS API 获取系统信息、任务信息和内存信息，并定时打印这些信息。
*   **代码:**
    *   `printTaskInfo()`: 打印任务信息，包括任务名称、状态、优先级、堆栈信息和任务编号。
    *   `printSystemInfo()`: 打印系统信息，包括 FreeRTOS 版本、Tick 频率、运行时间和空闲任务堆栈使用情况。
    *   `printMemoryInfo()`: 打印内存信息，包括堆内存大小、已分配内存块数、空闲内存大小和最小空闲内存大小。
    *   `monitorTask()`:  主监控任务，定时打印以上信息。

