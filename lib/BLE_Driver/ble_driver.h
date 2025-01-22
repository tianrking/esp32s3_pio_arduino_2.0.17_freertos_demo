// ble_driver.h
#ifndef BLE_DRIVER_H
#define BLE_DRIVER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_bt_device.h>
#include <map>
#include <functional>

// 创建命令处理器函数类型
using CommandHandler = std::function<String(const String&)>;

class BLEDriver {
public:
    static BLEDriver* getInstance();
    void init();
    void handle();
    bool isConnected();
    void sendMessage(const String& message);
    void registerCommand(const String& command, CommandHandler handler);
    void processCommand(const String& commandString);

private:
    BLEDriver() {}  // 私有构造函数
    static BLEDriver* instance;
    
    BLEServer *pServer = nullptr;
    BLEService *pService = nullptr;
    BLECharacteristic *pCharacteristic = nullptr;
    BLEAdvertising *pAdvertising = nullptr;
    
    bool deviceConnected = false;
    std::map<String, CommandHandler> commandHandlers;
    
    void sendResponse(const String& response);
    void setupBLECallbacks();
};

extern volatile int32_t target_positions[6];  // 从主程序引入
extern volatile bool new_command;             // 从主程序引入

#endif // BLE_DRIVER_H