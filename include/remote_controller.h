#pragma once
#include <lvgl.h>
#include "button_config.h"
#include "utils.h"

class RemoteController {
public:
    RemoteController();
    ~RemoteController();
    
    void init();
    void cleanup();
    static void createMainScreen();

private:
    static void touchEventCallback(lv_event_t * e);
    static void rotateTimerCallback(lv_timer_t *timer);
    static void remoteButtonEventHandler(lv_event_t *e);
    static void backToMainScreenHandler(lv_event_t *e);
    void updateButtonsPosition();
    void initStyles();
    void createButtons();

    lv_obj_t *remoteScreen;
    lv_obj_t *btnContainer;
    lv_timer_t *rotateTimer;

    static float currentAngle;
    static float rotationSpeed;
    static bool isTouching;
    static lv_point_t lastTouch;

    static constexpr int BUTTON_SIZE = 50;
    static constexpr int LAYOUT_RADIUS = 80;
    
    lv_style_t styleRemoteScreen;
    lv_style_t styleBtn;
    lv_style_t styleBtnPressed;
};