#include <Arduino.h>
#include <SCServo.h>
#include <HardwareSerial.h>

// 创建串口和舵机对象
HardwareSerial ServoSerial(1);  // UART1，使用D6(GPIO43, TX)和D7(GPIO44, RX)
SMS_STS sms_sts;

// 定义最大扫描ID
const int MAX_ID = 32;
// 定义扫描间隔（毫秒）
const int SCAN_INTERVAL = 3000;

void setup() {
  // 初始化调试串口
  Serial.begin(115200);
  while(!Serial) delay(10);
  
  // 初始化舵机串口
  ServoSerial.begin(115200, SERIAL_8N1, 44, 43); // RX=44(D7), TX=43(D6)
  sms_sts.pSerial = &ServoSerial;
  
  delay(1000);
  Serial.println("舵机扫描程序启动...");
}

void scanServos() {
  bool found_any = false;
  Serial.println("\n开始扫描舵机...");
  
  // 扫描ID 0-MAX_ID
  for(int id = 0; id <= MAX_ID; id++) {
    int result = sms_sts.Ping(id);
    if(result != -1) {
      found_any = true;
      Serial.print("发现舵机 ID: ");
      Serial.println(id);
      
      // 读取舵机位置
      int pos = sms_sts.ReadPos(id);
      if(pos != -1) {
        Serial.print("当前位置: ");
        Serial.println(pos);
      } else {
        Serial.println("读取位置失败");
      }
      
      // 读取舵机电压
      int voltage = sms_sts.ReadVoltage(id);
      if(voltage != -1) {
        Serial.print("电压: ");
        Serial.print(voltage);
        Serial.println("mV");
      } else {
        Serial.println("读取电压失败");
      }
      
      // 读取温度
      int temp = sms_sts.ReadTemper(id);
      if(temp != -1) {
        Serial.print("温度: ");
        Serial.print(temp);
        Serial.println("°C");
      } else {
        Serial.println("读取温度失败");
      }
      
      // 读取返回延时时间
    //   int delay_time = sms_sts.ReadReturnDelay(id);
    //   if(delay_time != -1) {
    //     Serial.print("返回延时时间: ");
    //     Serial.print(delay_time);
    //     Serial.println("us");
    //   }
      
      Serial.println("-------------------");
      delay(100);  // 短暂延时，避免通信太快
    }
  }
  
  if(!found_any) {
    Serial.println("未发现任何舵机!");
    Serial.println("请检查:");
    Serial.println("1. 接线是否正确 (RX->D7, TX->D6)");
    Serial.println("2. 电源是否接通");
    Serial.println("3. 波特率是否正确 (115200)");
  }
}

void loop() {
  scanServos();
  delay(SCAN_INTERVAL);
}