#include <lvgl.h>
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"
#include "remote_controller.h"

void setup() {
    Serial.begin(115200);
    Serial.println("XIAO round screen - Remote Control");
    
    lv_init();
    lv_xiao_disp_init();
    lv_xiao_touch_init();
    
    RemoteController::createMainScreen();
}

void loop() {
    lv_timer_handler();
    delay(5);
}