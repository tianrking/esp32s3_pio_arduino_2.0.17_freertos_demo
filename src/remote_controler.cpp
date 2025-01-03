#include "remote_controller.h"
#include <Arduino.h>

float RemoteController::currentAngle = 0.0f;
float RemoteController::rotationSpeed = 0.0f;
bool RemoteController::isTouching = false;
lv_point_t RemoteController::lastTouch = {0, 0};

RemoteController::RemoteController() : 
    remoteScreen(nullptr), 
    btnContainer(nullptr),
    rotateTimer(nullptr) {
}

RemoteController::~RemoteController() {
    cleanup();
}

void RemoteController::init() {
    currentAngle = 0.0f;
    rotationSpeed = 0.0f;

    remoteScreen = lv_obj_create(NULL);
    lv_scr_load(remoteScreen);

    initStyles();
    
    btnContainer = lv_obj_create(remoteScreen);
    lv_obj_remove_style_all(btnContainer);
    lv_obj_set_size(btnContainer, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_pos(btnContainer, 0, 0);
    lv_obj_clear_flag(btnContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btnContainer, touchEventCallback, LV_EVENT_ALL, NULL);

    createButtons();
    updateButtonsPosition();

    if (rotateTimer != NULL) {
        lv_timer_del(rotateTimer);
    }
    rotateTimer = lv_timer_create(rotateTimerCallback, 20, NULL);
}

void RemoteController::initStyles() {
    lv_style_init(&styleRemoteScreen);
    lv_style_set_bg_color(&styleRemoteScreen, lv_color_hex(0x0a1428));
    lv_style_set_bg_grad_color(&styleRemoteScreen, lv_color_hex(0x1c3a5a));
    lv_style_set_bg_grad_dir(&styleRemoteScreen, LV_GRAD_DIR_VER);
    lv_obj_add_style(remoteScreen, &styleRemoteScreen, 0);

    lv_style_init(&styleBtn);
    lv_style_set_radius(&styleBtn, LV_RADIUS_CIRCLE);
    lv_style_set_text_color(&styleBtn, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&styleBtn, 2);
    lv_style_set_border_opa(&styleBtn, LV_OPA_30);
    lv_style_set_shadow_width(&styleBtn, 10);
    lv_style_set_shadow_opa(&styleBtn, LV_OPA_50);

    lv_style_init(&styleBtnPressed);
    lv_style_set_bg_opa(&styleBtnPressed, LV_OPA_50);
    lv_style_set_border_width(&styleBtnPressed, 0);
}

void RemoteController::createButtons() {
    const ButtonConfig* buttons = ButtonManager::getButtons();
    int numButtons = ButtonManager::getButtonCount();
    
    for (int i = 0; i < numButtons; i++) {
        ButtonConfig& btnConfig = const_cast<ButtonConfig&>(buttons[i]);
        lv_obj_t *btn = lv_btn_create(btnContainer);
        btnConfig.btn = btn;
        
        lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);

        static lv_style_t btnStyles[9];
        lv_style_init(&btnStyles[i]);
        
        lv_style_set_radius(&btnStyles[i], LV_RADIUS_CIRCLE);
        lv_style_set_bg_color(&btnStyles[i], lv_color_hex(btnConfig.color));
        lv_style_set_text_color(&btnStyles[i], lv_color_hex(0xFFFFFF));
        lv_style_set_border_width(&btnStyles[i], 2);
        lv_style_set_border_opa(&btnStyles[i], LV_OPA_30);
        lv_style_set_shadow_width(&btnStyles[i], 10);
        lv_style_set_shadow_opa(&btnStyles[i], LV_OPA_50);
        lv_style_set_shadow_color(&btnStyles[i], lv_color_hex(btnConfig.color));
        
        lv_obj_add_style(btn, &btnStyles[i], 0);
        lv_obj_add_style(btn, &styleBtnPressed, LV_STATE_PRESSED);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, btnConfig.symbol);
        lv_obj_center(label);

        if (i == numButtons - 1) {
            lv_obj_add_event_cb(btn, backToMainScreenHandler, LV_EVENT_CLICKED, NULL);
        } else {
            lv_obj_add_event_cb(btn, remoteButtonEventHandler, LV_EVENT_CLICKED, (void*)&btnConfig);
        }
    }
}

void RemoteController::updateButtonsPosition() {
    const ButtonConfig* buttons = ButtonManager::getButtons();
    int numButtons = ButtonManager::getButtonCount();
    float angleStep = 360.0f / numButtons;

    int centerX = lv_disp_get_hor_res(NULL) / 2;
    int centerY = lv_disp_get_ver_res(NULL) / 2;
    
    for (int i = 0; i < numButtons; i++) {
        if (!buttons[i].btn) continue;

        float angle = i * angleStep + currentAngle;
        float rad = angle * M_PI / 180.0f;
        
        int x = centerX + LAYOUT_RADIUS * cos(rad) - BUTTON_SIZE/2;
        int y = centerY + LAYOUT_RADIUS * sin(rad) - BUTTON_SIZE/2;

        lv_obj_set_pos(buttons[i].btn, x, y);
    }
}

void RemoteController::touchEventCallback(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    lv_indev_t * indev = lv_indev_get_act();
    if(indev == NULL) return;

    lv_point_t center;
    center.x = lv_disp_get_hor_res(NULL) / 2;
    center.y = lv_disp_get_ver_res(NULL) / 2;

    switch(code) {
        case LV_EVENT_PRESSED:
            isTouching = true;
            lv_indev_get_point(indev, &lastTouch);
            rotationSpeed = 0;
            break;
            
        case LV_EVENT_PRESSING:
            if(isTouching) {
                lv_point_t currPoint;
                lv_indev_get_point(indev, &currPoint);
                
                float delta = Utils::getAngleBetweenPoints(lastTouch, currPoint, center);
                if(delta > 0) {
                    rotationSpeed = 2.0f;
                } else if(delta < 0) {
                    rotationSpeed = -2.0f;
                }
                
                currentAngle += rotationSpeed;
                ((RemoteController*)lv_event_get_user_data(e))->updateButtonsPosition();
                lastTouch = currPoint;
            }
            break;
            
        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            isTouching = false;
            rotationSpeed = 0;
            break;
            
        default:
            break;
    }
}

void RemoteController::rotateTimerCallback(lv_timer_t *timer) {
    if (!isTouching && rotationSpeed != 0) {
        currentAngle += rotationSpeed;
        ((RemoteController*)timer->user_data)->updateButtonsPosition();
    }
}

void RemoteController::remoteButtonEventHandler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    ButtonConfig *btnInfo = (ButtonConfig*)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.print("Button Clicked: ");
        Serial.print(btnInfo->name);
        Serial.print(", ID: ");
        Serial.println(btnInfo->id);
    }
}

void RemoteController::backToMainScreenHandler(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        RemoteController *controller = (RemoteController*)lv_event_get_user_data(e);
        if (controller) {
            controller->cleanup();
            createMainScreen();
        }
    }
}

void RemoteController::createMainScreen() {
    lv_obj_t *mainScreen = lv_obj_create(NULL);
    lv_scr_load(mainScreen);
    
    static lv_style_t styleMain;
    lv_style_init(&styleMain);
    lv_style_set_bg_color(&styleMain, lv_color_hex(0x0a1428));
    lv_style_set_bg_grad_color(&styleMain, lv_color_hex(0x1c3a5a));
    lv_style_set_bg_grad_dir(&styleMain, LV_GRAD_DIR_VER);
    lv_obj_add_style(mainScreen, &styleMain, 0);

    lv_obj_t *btn = lv_btn_create(mainScreen);
    lv_obj_set_size(btn, 140, 60);
    lv_obj_center(btn);
    
    static lv_style_t styleMainBtn;
    lv_style_init(&styleMainBtn);
    lv_style_set_radius(&styleMainBtn, 15);
    lv_style_set_bg_color(&styleMainBtn, lv_color_hex(0x2196F3));
    lv_style_set_bg_grad_color(&styleMainBtn, lv_color_hex(0x1976D2));
    lv_style_set_bg_grad_dir(&styleMainBtn, LV_GRAD_DIR_VER);
    lv_style_set_shadow_width(&styleMainBtn, 15);
    lv_style_set_shadow_color(&styleMainBtn, lv_color_hex(0x1976D2));
    lv_style_set_shadow_opa(&styleMainBtn, LV_OPA_50);
    
    lv_obj_add_style(btn, &styleMainBtn, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Remote Control");
    lv_obj_center(label);
    
    static RemoteController* controller = new RemoteController();
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            RemoteController* ctrl = (RemoteController*)lv_event_get_user_data(e);
            if (ctrl) {
                lv_obj_clean(lv_scr_act());
                ctrl->init();
            }
        }
    }, LV_EVENT_CLICKED, controller);
}

void RemoteController::cleanup() {
    if (rotateTimer) {
        lv_timer_del(rotateTimer);
        rotateTimer = nullptr;
    }
    if (remoteScreen) {
        lv_obj_clean(remoteScreen);
    }
}