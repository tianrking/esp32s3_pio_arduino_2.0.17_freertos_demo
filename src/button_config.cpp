#include "button_config.h"

ButtonConfig ButtonManager::buttons[] = {
    {LV_SYMBOL_POWER, "Power", 0, 0xFF0000, nullptr},
    {LV_SYMBOL_WIFI, "Light", 1, 0x00FF00, nullptr},
    {LV_SYMBOL_REFRESH, "Fan", 2, 0x0000FF, nullptr},
    {LV_SYMBOL_VOLUME_MAX, "Volume+", 3, 0xFF6B00, nullptr},
    {LV_SYMBOL_MUTE, "Volume-", 4, 0x9400D3, nullptr},
    {LV_SYMBOL_PREV, "Prev", 5, 0x00FFFF, nullptr},
    {LV_SYMBOL_NEXT, "Next", 6, 0xFFFF00, nullptr},
    {LV_SYMBOL_SETTINGS, "Settings", 7, 0xFF1493, nullptr},
    {LV_SYMBOL_LEFT, "Back", 8, 0x808080, nullptr}
};

const ButtonConfig* ButtonManager::getButtons() {
    return buttons;
}

int ButtonManager::getButtonCount() {
    return sizeof(buttons) / sizeof(buttons[0]);
}