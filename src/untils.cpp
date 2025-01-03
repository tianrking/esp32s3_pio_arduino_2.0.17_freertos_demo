#include "utils.h"

float Utils::getAngleBetweenPoints(lv_point_t p1, lv_point_t p2, lv_point_t center) {
    float a1 = atan2f(p1.y - center.y, p1.x - center.x);
    float a2 = atan2f(p2.y - center.y, p2.x - center.x);
    float angle = (a2 - a1) * 180.0f / M_PI;
    
    if (angle > 180.0f) angle -= 360.0f;
    if (angle < -180.0f) angle += 360.0f;
    
    return angle;
}