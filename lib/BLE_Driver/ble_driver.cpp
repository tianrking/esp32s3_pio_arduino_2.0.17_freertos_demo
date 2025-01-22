// ble_driver.cpp
#include "ble_driver.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEDriver* BLEDriver::instance = nullptr;

// 单例模式获取实例
BLEDriver* BLEDriver::getInstance() {
    if (instance == nullptr) {
        instance = new BLEDriver();
    }
    return instance;
}

// BLE 服务器回调类
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("BLE Client connected");
        BLEDriver::getInstance()->sendMessage("Connected");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("BLE Client disconnected");
        pServer->startAdvertising();
    }
};

// BLE 特征值回调类
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            String commandStr = String(value.c_str());
            BLEDriver::getInstance()->processCommand(commandStr);
        }
    }
};

void BLEDriver::init() {
    BLEDevice::init("XIAO_ARM_BLE");
    
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    pService = pServer->createService(SERVICE_UUID);
    
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pCharacteristic->setValue("XIAO Arm Ready");
    
    pService->start();
    
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
    
    // 注册命令处理函数
    registerCommand("SET_JOINT", [](const String& params) {
        // 解析参数：格式 "joint_id,position"
        int commaIndex = params.indexOf(',');
        if (commaIndex != -1) {
            int joint_id = params.substring(0, commaIndex).toInt();
            int position = params.substring(commaIndex + 1).toInt();
            
            if (joint_id >= 0 && joint_id < 6) {
                target_positions[joint_id] = position;
                new_command = true;
                return "OK";
            }
        }
        return "ERROR: Invalid parameters";
    });
    
    registerCommand("SET_ALL", [](const String& params) {
        // 解析参数：格式 "pos1,pos2,pos3,pos4,pos5,pos6"
        String values[6];
        int prevIndex = 0;
        int index = 0;
        
        for (int i = 0; i < params.length() && index < 6; i++) {
            if (params[i] == ',') {
                values[index++] = params.substring(prevIndex, i);
                prevIndex = i + 1;
            }
        }
        values[index] = params.substring(prevIndex);
        
        if (index == 5) {  // 确保有6个值
            for (int i = 0; i < 6; i++) {
                target_positions[i] = values[i].toInt();
            }
            new_command = true;
            return "OK";
        }
        return "ERROR: Need 6 values";
    });
    
    registerCommand("CENTER", [](const String& params) {
        for (int i = 0; i < 6; i++) {
            target_positions[i] = 2048;
        }
        new_command = true;
        return "OK";
    });
}

void BLEDriver::handle() {
    // 处理周期性任务（如果有的话）
}

bool BLEDriver::isConnected() {
    return deviceConnected;
}

void BLEDriver::sendMessage(const String& message) {
    if (pCharacteristic != nullptr) {
        pCharacteristic->setValue(message.c_str());
        pCharacteristic->notify();
    }
}

void BLEDriver::registerCommand(const String& command, CommandHandler handler) {
    commandHandlers[command] = handler;
}

void BLEDriver::processCommand(const String& commandString) {
    int equalIndex = commandString.indexOf('=');
    String command = equalIndex == -1 ? commandString : commandString.substring(0, equalIndex);
    String params = equalIndex == -1 ? "" : commandString.substring(equalIndex + 1);
    
    auto it = commandHandlers.find(command);
    if (it != commandHandlers.end()) {
        String response = it->second(params);
        sendResponse(response);
    } else {
        sendResponse("ERROR: Unknown command");
    }
}

void BLEDriver::sendResponse(const String& response) {
    sendMessage(response);
}