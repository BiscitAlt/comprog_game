#ifndef ITEM_H
#define ITEM_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"

struct Box {
    Rectangle rect; // เก็บทั้ง x, y, width, height ในตัวเดียว
    Color color;
    bool isSolid;   // ถ้า true คือเดินทะลุไม่ได้
};

#endif