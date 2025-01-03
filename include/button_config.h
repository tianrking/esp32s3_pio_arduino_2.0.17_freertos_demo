#pragma once
#include <lvgl.h>

struct ButtonConfig {
    const char *symbol;
    const char *name;
    int id;
    uint32_t color;
    lv_obj_t *btn;
};

class ButtonManager {
public:
    static const ButtonConfig* getButtons();
    static int getButtonCount();
private:
    static ButtonConfig buttons[];
};