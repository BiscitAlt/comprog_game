#include "map.h"

Map::Map() {
    GenerateNewRoom();
}

void Map::LoadAssets() {
    tileset = LoadTexture("assets\\tileset.png");
    
    // ถ้าโหลดไม่สำเร็จ id ของ texture จะเป็น 0
    if (tileset.id == 0) tileset = LoadTexture("src\\assets\\tileset.png");
    if (tileset.id == 0) TraceLog(LOG_ERROR, "FAILED TO LOAD TILESET! Check path: assets/tileset.png");
    else TraceLog(LOG_INFO, "TILESET LOADED SUCCESSFULLY!");
}

void Map::UnloadAssets() {
    UnloadTexture(tileset);
}

void Map::GenerateNewRoom() {
    SetRandomSeed(GetTime());
    // เริ่มต้นแมพใหม่ (กำแพงรอบนอก + พื้นข้างใน)
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (y == 0 && x > 0 && x < cols - 1) data[y][x] = GetRandomValue(4, 7); // กำแพง บน
            else if (x == 0 && y > 0 && y < rows - 1) data[y][x] = GetRandomValue(8, 10); // กำแพง ซ้าย
            else if (x == cols - 1 && y > 0 && y < rows - 1) data[y][x] = GetRandomValue(11, 13); // กำแพง ขวา
            else if (y == 0 && x == 0) data[y][x] = 14; // กำแพง บน-ซ้าย
            else if (y == 0 && x == cols - 1) data[y][x] = 15; // กำแพง บน-ขวา
            else if (y == rows - 1 && x > 0 && x < cols - 1) data[y][x] = GetRandomValue(16, 19); // กำแพง ล่าง
            else if(y == rows - 1 && x == 0) data[y][x] = 20; // กำแพง ล่าง-ซ้าย
            else if(y == rows - 1 && x == cols - 1) data[y][x] = 21; // กำแพง ล่าง-ขวา
            else data[y][x] = GetRandomValue(0, 3); // พื้น
        }
    }

    // สุ่มเพิ่มสิ่งกีดขวางในแมพ (ไม่เกิน maxObstacles)
    for (int i = 0; i < maxObstacles; i++) {
        int randX = GetRandomValue(2, cols - 2); // ไม่ให้สุ่มไปที่ขอบ
        int randY = GetRandomValue(2, rows + 7);
        data[randY][randX] = 23; // เพิ่มสิ่งกีดขวาง
        data[randY-1][randX] = 24;
        data[randY-2][randX] = 25;
        data[randY-3][randX] = 26;
        data[randY-4][randX] = 27;
    }
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
                } else if (tileID == 4) {
                    DrawRectangle(posX, posY, tileSize, tileSize, BROWN); // ประตู
                }
                DrawRectangleLines(posX, posY, tileSize, tileSize, BLACK); // ตีเส้นตารางให้ดูง่ายขึ้น
            } else {
                // Rectangle สำหรับ "ตัด" รูปภาพจาก tileset
                // สมมติว่าภาพแต่ละช่องกว้าง 16x16 pixel
                Rectangle crop;
                crop.width = 16;
                crop.height = 16;
                crop.y = 0;
                
                if (tileID >= 0 && tileID <= 3) { // พื้น
                    crop.y = 0;
                    crop.x = tileID * 16;
                } else if (tileID >= 4 && tileID <= 7) { // กำแพง บน
                    crop.y = 16;
                    crop.x = (tileID - 4) * 16;
                } else if (tileID >= 8 && tileID <= 10) { // กำแพง ซ้าย
                    crop.y = 32;
                    crop.x = (tileID - 8) * 16;
                } else if (tileID >= 11 && tileID <= 13) { // กำแพง ขวา
                    crop.y = 48;
                    crop.x = (tileID - 11) * 16;
                } else if (tileID >= 14 && tileID <= 15) { // กำแพง บน-ซ้าย, บน-ขวา
                    crop.y = 64;
                    crop.x = (tileID - 14) * 16;
                } else if (tileID >= 16 && tileID <= 19) { // กำแพง ล่าง
                    crop.y = 80;
                    crop.x = (tileID - 16) * 16;
                } else if (tileID >= 20 && tileID <= 21) { // กำแพง ล่าง-ซ้าย, ล่าง-ขวา
                    crop.y = 96;
                    crop.x = (tileID - 20) * 16;
                } else if (tileID >= 23 && tileID <= 27) { // สิ่งกีดขวาง
                    crop.y = 112;
                    crop.x = (tileID - 23) * 16;
                }

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

    int id = data[row][col];
    // ถือว่าเป็นกำแพง ถ้า ID เป็นหมวดกำแพง(4-21) หรือสิ่งกีดขวาง(23-27)
    return (id >= 4 && id <= 21) || (id >= 24 && id <= 27);
}