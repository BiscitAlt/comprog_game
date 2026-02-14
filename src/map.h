#ifndef MAP_H
#define MAP_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"

class Map {
public:
    // กำหนดขนาด Grid (เช่น 30x30 ช่อง)
    static const int rows = 30;
    static const int cols = 30;
    static const int tileSize = 20; // หนึ่งช่องกว้าง 20 pixel
    
    int data[rows][cols]; // 0 = พื้น, 1 = กำแพง

    Map();      // Constructor (ใช้ตั้งค่าแมพตอนเริ่มเกม)
    bool IsWall(float x, float y); // ฟังก์ชันเช็คว่าตำแหน่ง (x, y) เป็นกำแพงหรือไม่
    void Draw(); // ฟังก์ชันวาดแมพ
};

#endif