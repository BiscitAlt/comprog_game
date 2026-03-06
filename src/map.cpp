#include "map.h"

Map::Map() {
    GenerateNewRoom();
}

void Map::LoadAssets() {
    tileset = LoadTexture("src\\assets\\tileset.png");
    
    // ถ้าโหลดไม่สำเร็จ id ของ texture จะเป็น 0
    if (tileset.id == 0) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD TILESET! Check path: src\\assets\\tileset.png");
    } else {
        TraceLog(LOG_INFO, "TILESET LOADED SUCCESSFULLY!");
    }
}

void Map::UnloadAssets() {
    UnloadTexture(tileset);
}

void Map::GenerateNewRoom() {
    // 1. เริ่มต้นแมพใหม่ (กำแพงรอบนอก + พื้นข้างใน)
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (y == 0 || y == rows - 1 || x == 0 || x == cols - 1) {
                data[y][x] = 1; // กำแพง
            } else {
                data[y][x] = 0; // พื้น
            }
        }
    }

    // 2. สุ่มเพิ่มกล่องในแมพ (ไม่เกิน maxBoxes กล่อง)
    SetRandomSeed(GetTime());
    for (int i = 0; i < maxBoxes; i++) {
        int randX = GetRandomValue(2, cols - 2); // ไม่ให้สุ่มไปที่ขอบ
        int randY = GetRandomValue(2, rows - 2);
        data[randY][randX] = 1; // เพิ่มกล่องเป็นกำแพง
    }

    data[0][cols/2] = 2;          // ประตูบน
    data[rows-1][cols/2] = 2;     // ประตูล่าง
    data[rows/2][0] = 2;          // ประตูซ้าย
    data[rows/2][cols-1] = 2;     // ประตูขวา
}
void Map::Draw() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            float posX = x * tileSize;
            float posY = y * tileSize;

            int tileID = data[y][x];

            // --- ระบบสำรอง: ถ้าโหลดรูปไม่ขึ้น (id == 0) ให้เทสีพื้นแทน จะได้ไม่จอดำ! ---
            if (tileset.id == 0) {
                if (tileID == 0) {
                    DrawRectangle(posX, posY, tileSize, tileSize, DARKGRAY); // พื้น
                } else if (tileID == 1) {
                    DrawRectangle(posX, posY, tileSize, tileSize, LIGHTGRAY); // กำแพง
                } else if (tileID == 2) {
                    DrawRectangle(posX, posY, tileSize, tileSize, BROWN); // ประตู
                }
                DrawRectangleLines(posX, posY, tileSize, tileSize, BLACK); // ตีเส้นตารางให้ดูง่ายขึ้น
            } 
            // --- ระบบปกติ: ถ้าระบบหาไฟล์รูปเจอ ก็ตัดรูปมาวาดตามปกติ ---
            else {
                Rectangle crop;
                crop.width = 16;
                crop.height = 16;
                crop.y = 0; 

                if (tileID == 0) crop.x = 0; 
                else if (tileID == 1) crop.x = 16; 
                else if (tileID == 2) crop.x = 32; 

                // วาดภาพโดยขยายจาก 16x16 ให้เต็มช่อง tileSize (32)
                Rectangle dest = {posX, posY, (float)tileSize, (float)tileSize};
                DrawTexturePro(tileset, crop, dest, {0, 0}, 0.0f, WHITE);
            }
        }
    }
}

bool Map::IsWall(float x, float y) {
    // 1. แปลง Pixel เป็นตำแหน่งใน Array (Grid)
    int col = (int)(x / tileSize);
    int row = (int)(y / tileSize);

    // 2. เช็คกัน Error: ถ้าพิกัดอยู่นอกขอบเขตแมพ ให้ถือว่าเป็นกำแพงไว้ก่อน
    if (row < 0 || row >= rows || col < 0 || col >= cols) return true; 

    // 3. คืนค่าตามข้อมูลใน Array (1 คือกำแพง/จริง, 0 คือพื้น/เท็จ)
    return data[row][col] == 1;
}

bool Map::IsDoor(float x, float y) {
    int col = (int)(x / tileSize);
    int row = (int)(y / tileSize);
    if (row < 0 || row >= rows || col < 0 || col >= cols) return false;
    return data[row][col] == 2; // เช็คว่าเหยียบ ID 2 ไหม
}