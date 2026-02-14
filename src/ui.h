#ifndef UI_H
#define UI_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"

void textKey(float speed);
void cursor(Vector2 mousePos);

#endif