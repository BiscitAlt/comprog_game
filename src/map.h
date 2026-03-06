#ifndef MAP_H
#define MAP_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"
class Map {
public:
    static const int rows = 15;
    static const int cols = 15;
    static const int tileSize = 32; // ขนาดต่อ 1 ช่อง
    int maxBoxes = 0; // จำนวนกล่องสุ่มในแมพ (ปรับได้ตามต้องการ)
    
    int data[rows][cols]; // แมพข้อมูล (0 = พื้น, 1 = กำแพง)
    Texture2D tileset;

    Map();      // Constructor (ใช้ตั้งค่าแมพตอนเริ่มเกม)

    void LoadAssets();   // โหลดรูปภาพ (เรียกครั้งเดียวตอนเริ่มเกม)
    void UnloadAssets(); // คืน Memory (เรียกตอนปิดเกม)

    void GenerateNewRoom(); // สร้างแมพใหม่ (กำแพงรอบนอก + พื้นข้างใน + กล่องสุ่ม)
    bool IsWall(float x, float y); // ฟังก์ชันเช็คว่าตำแหน่ง (x, y) เป็นกำแพงหรือไม่
    bool IsDoor(float x, float y); // ฟังก์ชันเช็คว่าตำแหน่ง (x, y) เป็นประตูหรือไม่
    void Draw(); // ฟังก์ชันวาดแมพ
};

#endif