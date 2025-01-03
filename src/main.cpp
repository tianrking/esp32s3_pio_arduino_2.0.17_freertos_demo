#include <lvgl.h>
// uncomment a library for display driver
#define USE_TFT_ESPI_LIBRARY
// #define USE_ARDUINO_GFX_LIBRARY
#include "lv_xiao_round_screen.h"
#include <math.h>

// =========================================================
//                      Global Variables
// =========================================================
lv_obj_t *main_screen;
lv_obj_t *remote_screen;
lv_obj_t *btn_container;
lv_timer_t *rotate_timer = NULL;

static float current_angle = 0.0f;
static float rotation_speed = 0.0f;
static bool is_touching = false;
static lv_point_t last_touch;
static uint32_t last_touch_time = 0;

static lv_style_t style_remote_screen;
static lv_style_t style_btn;
static lv_style_t style_btn_pressed;

typedef struct {
    const char *symbol;
    const char *name;
    int id;
    uint32_t color;
    lv_obj_t *btn;
} ctrl_btn_info_t;

static ctrl_btn_info_t ctrl_btns[] = {
    {LV_SYMBOL_POWER, "Power", 0, 0xFF0000},
    {LV_SYMBOL_WIFI, "Light", 1, 0x00FF00},
    {LV_SYMBOL_REFRESH, "Fan", 2, 0x0000FF},
    {LV_SYMBOL_VOLUME_MAX, "Volume+", 3, 0xFF6B00},
    {LV_SYMBOL_MUTE, "Volume-", 4, 0x9400D3},
    {LV_SYMBOL_PREV, "Prev", 5, 0x00FFFF},
    {LV_SYMBOL_NEXT, "Next", 6, 0xFFFF00},
    {LV_SYMBOL_SETTINGS, "Settings", 7, 0xFF1493},
    {LV_SYMBOL_LEFT, "Back", 8, 0x808080}
};

// =========================================================
//                      Utility Functions
// =========================================================
static float get_angle_between_points(lv_point_t p1, lv_point_t p2, lv_point_t center) {
    float a1 = atan2f(p1.y - center.y, p1.x - center.x);
    float a2 = atan2f(p2.y - center.y, p2.x - center.x);
    float angle = (a2 - a1) * 180.0f / M_PI;
    
    if (angle > 180.0f) angle -= 360.0f;
    if (angle < -180.0f) angle += 360.0f;
    
    return angle;
}

// Forward declarations
static void create_main_screen(void);
static void remote_btn_event_handler(lv_event_t *e);
static void back_to_main_screen_handler(lv_event_t *e);
static void touch_event_cb(lv_event_t *e);
static void rotate_timer_cb(lv_timer_t *timer);
static void update_buttons_position(void);

// =========================================================
//                      Remote Control Init
// =========================================================
static void remote_control_init() {
    current_angle = 0.0f;
    rotation_speed = 0.0f;

    remote_screen = lv_obj_create(NULL);
    lv_scr_load(remote_screen);

    lv_style_init(&style_remote_screen);
    lv_style_set_bg_color(&style_remote_screen, lv_color_hex(0x0a1428));
    lv_style_set_bg_grad_color(&style_remote_screen, lv_color_hex(0x1c3a5a));
    lv_style_set_bg_grad_dir(&style_remote_screen, LV_GRAD_DIR_VER);
    lv_obj_add_style(remote_screen, &style_remote_screen, 0);

    btn_container = lv_obj_create(remote_screen);
    lv_obj_remove_style_all(btn_container);
    lv_obj_set_size(btn_container, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_pos(btn_container, 0, 0);
    lv_obj_clear_flag(btn_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btn_container, touch_event_cb, LV_EVENT_ALL, NULL);

    // Initialize button styles
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, LV_RADIUS_CIRCLE);
    lv_style_set_text_color(&style_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_border_opa(&style_btn, LV_OPA_30);
    lv_style_set_shadow_width(&style_btn, 10);
    lv_style_set_shadow_opa(&style_btn, LV_OPA_50);

    lv_style_init(&style_btn_pressed);
    lv_style_set_bg_opa(&style_btn_pressed, LV_OPA_50);
    lv_style_set_border_width(&style_btn_pressed, 0);

    // Create buttons
    int num_btns = sizeof(ctrl_btns) / sizeof(ctrl_btns[0]);
    for (int i = 0; i < num_btns; i++) {
        lv_obj_t *btn = lv_btn_create(btn_container);
        ctrl_btns[i].btn = btn;
        lv_obj_set_size(btn, 50, 50); // Adjusted button size for better visibility

        static lv_style_t btn_styles[9];
        lv_style_init(&btn_styles[i]);
        
        lv_style_set_radius(&btn_styles[i], LV_RADIUS_CIRCLE);
        lv_style_set_bg_color(&btn_styles[i], lv_color_hex(ctrl_btns[i].color));
        lv_style_set_text_color(&btn_styles[i], lv_color_hex(0xFFFFFF));
        lv_style_set_border_width(&btn_styles[i], 2);
        lv_style_set_border_opa(&btn_styles[i], LV_OPA_30);
        lv_style_set_shadow_width(&btn_styles[i], 10);
        lv_style_set_shadow_opa(&btn_styles[i], LV_OPA_50);
        lv_style_set_shadow_color(&btn_styles[i], lv_color_hex(ctrl_btns[i].color));
        
        lv_obj_add_style(btn, &btn_styles[i], 0);
        lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, ctrl_btns[i].symbol);
        lv_obj_center(label);

        if (i == num_btns - 1) {
            lv_obj_add_event_cb(btn, back_to_main_screen_handler, LV_EVENT_CLICKED, NULL);
        } else {
            lv_obj_add_event_cb(btn, remote_btn_event_handler, LV_EVENT_CLICKED, (void*)&ctrl_btns[i]);
        }
    }

    update_buttons_position();

    if (rotate_timer != NULL) {
        lv_timer_del(rotate_timer);
    }
    rotate_timer = lv_timer_create(rotate_timer_cb, 20, NULL);
}

// =========================================================
//                      Button Position Update
// =========================================================
static void update_buttons_position() {
    int num_btns = sizeof(ctrl_btns) / sizeof(ctrl_btns[0]);
    float angle_step = 360.0f / num_btns;
    int radius = 80; // Adjusted radius for better button spacing

    int center_x = lv_disp_get_hor_res(NULL) / 2;
    int center_y = lv_disp_get_ver_res(NULL) / 2;
    
    for (int i = 0; i < num_btns; i++) {
        if (!ctrl_btns[i].btn) continue;

        float angle = i * angle_step + current_angle;
        float rad = angle * M_PI / 180.0f;
        
        int x = center_x + radius * cos(rad) - 25; // 25 is half button size (50/2)
        int y = center_y + radius * sin(rad) - 25;

        lv_obj_set_pos(ctrl_btns[i].btn, x, y);
    }
}

// =========================================================
//                      Event Handlers
// =========================================================
static void touch_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    lv_indev_t * indev = lv_indev_get_act();
    if(indev == NULL) return;

    lv_point_t center;
    center.x = lv_disp_get_hor_res(NULL) / 2;
    center.y = lv_disp_get_ver_res(NULL) / 2;

    switch(code) {
        case LV_EVENT_PRESSED:
            is_touching = true;
            lv_indev_get_point(indev, &last_touch);
            last_touch_time = lv_tick_get();
            rotation_speed = 0;
            break;
            
        case LV_EVENT_PRESSING:
            if(is_touching) {
                lv_point_t curr_point;
                lv_indev_get_point(indev, &curr_point);
                
                float delta = get_angle_between_points(last_touch, curr_point, center);
                // Set a fixed slow rotation speed based on direction
                if(delta > 0) {
                    rotation_speed = 2.0f;  // Slow clockwise rotation
                } else if(delta < 0) {
                    rotation_speed = -2.0f; // Slow counter-clockwise rotation
                }
                
                current_angle += rotation_speed;
                update_buttons_position();
                last_touch = curr_point;
            }
            break;
            
        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            is_touching = false;
            rotation_speed = 0;  // Stop rotation when touch is released
            break;
            
        default:
            break;
    }
}

static void rotate_timer_cb(lv_timer_t *timer) {
    if (!is_touching && rotation_speed != 0) {
        current_angle += rotation_speed;
        update_buttons_position();
    }
}

static void remote_btn_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    ctrl_btn_info_t *btn_info = (ctrl_btn_info_t*)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.print("Button Clicked: ");
        Serial.print(btn_info->name);
        Serial.print(", ID: ");
        Serial.println(btn_info->id);
    }
}

static void back_to_main_screen_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (rotate_timer != NULL) {
            lv_timer_del(rotate_timer);
            rotate_timer = NULL;
        }
        lv_obj_clean(lv_scr_act());
        create_main_screen();
    }
}

// =========================================================
//                      Main Screen
// =========================================================
static void create_main_screen() {
    main_screen = lv_obj_create(NULL);
    lv_scr_load(main_screen);
    
    static lv_style_t style_main;
    lv_style_init(&style_main);
    lv_style_set_bg_color(&style_main, lv_color_hex(0x0a1428));
    lv_style_set_bg_grad_color(&style_main, lv_color_hex(0x1c3a5a));
    lv_style_set_bg_grad_dir(&style_main, LV_GRAD_DIR_VER);
    lv_obj_add_style(main_screen, &style_main, 0);

    lv_obj_t *btn = lv_btn_create(main_screen);
    lv_obj_set_size(btn, 140, 60); // Smaller button for round screen
    lv_obj_center(btn);
    
    static lv_style_t style_main_btn;
    lv_style_init(&style_main_btn);
    lv_style_set_radius(&style_main_btn, 15);
    lv_style_set_bg_color(&style_main_btn, lv_color_hex(0x2196F3));
    lv_style_set_bg_grad_color(&style_main_btn, lv_color_hex(0x1976D2));
    lv_style_set_bg_grad_dir(&style_main_btn, LV_GRAD_DIR_VER);
    lv_style_set_shadow_width(&style_main_btn, 15);
    lv_style_set_shadow_color(&style_main_btn, lv_color_hex(0x1976D2));
    lv_style_set_shadow_opa(&style_main_btn, LV_OPA_50);
    
    lv_obj_add_style(btn, &style_main_btn, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Remote Control");
    lv_obj_center(label);
    
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            lv_obj_clean(lv_scr_act());
            remote_control_init();
        }
    }, LV_EVENT_CLICKED, NULL);
}

// =========================================================
//                      Setup & Loop
// =========================================================
void setup() {
    Serial.begin(115200);
    Serial.println("XIAO round screen - Remote Control");
    
    lv_init();
    lv_xiao_disp_init();
    lv_xiao_touch_init();
    
    create_main_screen();
}

void loop() {
    lv_timer_handler();
    delay(5);
}