#pragma once
#include <lvgl.h>
#include <math.h>

class Utils {
public:
    static float getAngleBetweenPoints(lv_point_t p1, lv_point_t p2, lv_point_t center);
};
