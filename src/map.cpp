#include "map.h"

// ============================================================
// TileHash — pure position-based hash, NO global RNG state touched.
//
// Returns a stable pseudo-random unsigned int for any (x, y) pair.
// Same input always gives the same output — safe to call every frame.
//
// Uses the Murmur3 finalizer mix: fast, good distribution, no collisions
// in practice for tile coordinates.
//
// Usage:
//   int variant = TileHash(tileX, tileY) % NUM_VARIANTS;
// ============================================================
static unsigned int TileHash(int x, int y) {
    unsigned int h = (unsigned int)(x * 2246822519u ^ y * 3266489917u);
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// ============================================================
// GetCrop — converts a tile ID to its source rectangle on the tileset
// Add new ID ranges here whenever you add new tile types
// ============================================================
static Rectangle GetCrop(int id) {
    if (id >= 0  && id <= 3)  return {  id * 16.0f,        0.0f, 16.0f, 16.0f }; // floor
    if (id >= 4  && id <= 8)  return { (id - 4)  * 16.0f, 16.0f, 16.0f, 16.0f }; // vertical pillar
    if (id >= 9  && id <= 13) return { (id - 9)  * 16.0f, 32.0f, 16.0f, 16.0f }; // horizontal pillar
    if (id >= 14 && id <= 17) return { (id - 14) * 16.0f, 48.0f, 16.0f, 16.0f }; // vase decorations
    if (id >= 18 && id <= 21) return { (id - 18) * 16.0f, 64.0f, 16.0f, 16.0f }; // broken tile decorations
    if (id >= 22 && id <= 25) return { (id - 21) * 16.0f, 80.0f, 16.0f, 16.0f }; // candle decorations
    if (id == 26)             return {         0 * 16.0f, 96.0f, 16.0f, 16.0f }; // spike
    return { 0.0f, 0.0f, 16.0f, 16.0f }; // fallback
}

void Map::LoadAssets() {
    tileset = LoadTexture("assets\\tileset.png");
    if (tileset.id == 0) tileset = LoadTexture("src\\assets\\tileset.png");
    if (tileset.id == 0) TraceLog(LOG_ERROR, "FAILED TO LOAD TILESET! Check path: assets/tileset.png");
    else TraceLog(LOG_INFO, "TILESET LOADED SUCCESSFULLY!");
}

void Map::UnloadAssets() {
    UnloadTexture(tileset);
}

// ============================================================
// River helpers
// ============================================================

// Returns the world-space X centre of the ravine at tile row tileY.
// Two sine waves layered together give a natural winding feel.
float Map::GetRavineCenterX(int tileY) const {
    float worldY = tileY * (float)tileSize;
    return ravineBaseX
         + 150.0f * sinf(worldY * 0.0020f)   // slow, wide bends
         +  60.0f * sinf(worldY * 0.0051f);  // faster, tighter wiggles
}

// Returns true when this tile row is part of a walkable ford gap.
bool Map::IsCrossingRow(int tileY) const {
    // Normalise to positive so modulo works for negative tileY too
    int n     = ((tileY % CROSSING_EVERY) + CROSSING_EVERY) % CROSSING_EVERY;
    int start = (CROSSING_EVERY - CROSSING_WIDTH) / 2;
    return (n >= start && n < start + CROSSING_WIDTH);
}

// True when the tile at (tileX, tileY) is a blocking abyss tile.
bool Map::IsRavineTile(int tileX, int tileY) const {
    if (IsCrossingRow(tileY)) return false; // ledge rows are always walkable

    float centerWorldX = GetRavineCenterX(tileY);
    int   centerTileX  = (int)floorf(centerWorldX / (float)tileSize);
    int   dist         = abs(tileX - centerTileX);

    return (dist <= RAVINE_HALF_WIDTH);
}

// Returns a tint color that gets darker as you approach the abyss centre.
//
//   distFromCenter == RAVINE_HALF_WIDTH  →  cliff edge (dim floor color)
//   distFromCenter == 0                  →  abyss centre (near black)
//
// The ravine is drawn by tinting the normal floor tile, so you don't need
// any special tileset tiles for the abyss — it reuses tile ID 0.
Color Map::GetRavineTint(int distFromCenter) const {
    // Clamp to valid range
    if (distFromCenter < 0)                   distFromCenter = 0;
    if (distFromCenter > RAVINE_HALF_WIDTH)   distFromCenter = RAVINE_HALF_WIDTH;

    // Brightness gradient: 0 (abyss) → very dark,  RAVINE_HALF_WIDTH (edge) → dim
    // Adjust these RGB values to match your tileset's color palette.
    //
    //  dist 0 : {18,  12,  8,  255}  — near-black abyss
    //  dist 1 : {45,  30, 20,  255}
    //  dist 2 : {90,  60, 40,  255}
    //  dist 3 : {145, 100, 70, 255}  — cliff edge (darkened floor)
    //
    static const Color gradient[] = {
        {  18,  12,   8, 255 },  // dist 0 — deepest abyss
        {  45,  30,  20, 255 },  // dist 1
        {  90,  60,  40, 255 },  // dist 2
        { 145, 100,  70, 255 },  // dist 3 — cliff rim
        { 180, 140, 100, 255 },  // dist 4 (if you ever widen RAVINE_HALF_WIDTH)
        { 210, 170, 130, 255 },  // dist 5
        { 235, 200, 160, 255 },  // dist 6
    };

    // Safety: cap array access
    int maxIdx = sizeof(gradient) / sizeof(gradient[0]) - 1;
    if (distFromCenter > maxIdx) distFromCenter = maxIdx;

    return gradient[distFromCenter];
}

// ============================================================
// SpawnDecoration — internal helper
//
//  multiTile = false → pick ONE random piece from def.pieces and place it at origin
//  multiTile = true  → place EVERY piece at its (offsetX, offsetY) from origin
// ============================================================
static void SpawnDecoration(std::vector<Decoration>& decorations, const DecorationDef& def, Vector2 origin, int tileSize)
{
    if (def.pieces.empty()) return;

    if (!def.multiTile) {
        // Single-tile variant: choose one piece at random
        int pick = GetRandomValue(0, (int)def.pieces.size() - 1);
        const BlueprintPiece& piece = def.pieces[pick];
        decorations.push_back({{origin.x + piece.offsetX * tileSize, origin.y + piece.offsetY * tileSize}, piece.ID});
    } else {
        // Multi-tile: place every piece at its offset
        for (const auto& piece : def.pieces) {
            decorations.push_back({
                { origin.x + piece.offsetX * tileSize,
                  origin.y + piece.offsetY * tileSize },
                piece.ID
            });
        }
    }
}

void Map::UpdateMap(Vector2 playerPos) {
    const float despawnRadiusSq = 1200.0f * 1200.0f; // ถ้ายืนห่างเกินระยะนี้ ลบสิ่งกีดขวางทิ้ง

    // 1. ลบสิ่งกีดขวางที่อยู่ไกลเกินไป (ลดภาระเครื่อง)
    for (int i = obstacles.size() - 1; i >= 0; i--) {
        float distX = playerPos.x - obstacles[i].pos.x;
        float distY = playerPos.y - obstacles[i].pos.y;
        float distSq = (distX * distX) + (distY * distY);
        
        if (distSq > despawnRadiusSq) {
            obstacles.erase(obstacles.begin() + i);
        }
    }

    for (int i = decorations.size() - 1; i >= 0; i--) {
        float distX = playerPos.x - decorations[i].pos.x;
        float distY = playerPos.y - decorations[i].pos.y;
        float distSq = (distX * distX) + (distY * distY);
        
        if (distSq > despawnRadiusSq) {
            decorations.erase(decorations.begin() + i);
        }
    }

    // Obstascle Blueprint {x offset, y offset, tileID}
    std::vector<BlueprintPiece> verticalPillarBlueprint = {{0,  0, 4}, {0, -1, 5}, {0, -2, 6}, {0, -3, 7}, {0, -4, 8}};

    std::vector<BlueprintPiece> horizontalPillarBlueprint = {{0, 0, 9},{+1, 0, 10},{+2, 0, 11},{+3, 0, 12}, {+4, 0, 13}};

    // Decoration definitions
    DecorationDef vaseDef;
    vaseDef.pieces    = {{0,0,14}, {0,0,15}, {0,0,16}, {0,0,17}};

    DecorationDef brokenTilesDef;
    brokenTilesDef.multiTile = true;
    brokenTilesDef.pieces    = {{0,0,18}, {1,0,19}, {0,1,20}, {1,1,21}};

    DecorationDef candleDef;
    candleDef.pieces    = {{0,0,22}, {0,0,23}, {0,0,24}, {0,0,25}};

    DecorationDef spikeDef;
    spikeDef.pieces    = {{0,0,26}};

    // --- Pool of all decoration types ---
    // Add new DecorationDefs here to include them in random spawning.
    std::vector<DecorationDef> decorationPool = { vaseDef, brokenTilesDef, candleDef, spikeDef };

    // ---- Helper lambda: random spawn position near player ----
    auto randomNearPos = [&]() -> Vector2 {
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist  = (float)GetRandomValue(600, 800);
        return {
            roundf((playerPos.x + cosf(angle) * dist) / tileSize) * tileSize,
            roundf((playerPos.y + sinf(angle) * dist) / tileSize) * tileSize
        };
    };

    // ---- Spawn new obstacles (pillars) ----
    if (obstacles.size() < 50) {
        Vector2 newPos = randomNearPos();

        int randStructure = GetRandomValue(0, 1);
        const auto& blueprint = (randStructure == 0) ? verticalPillarBlueprint : horizontalPillarBlueprint;

        bool canPlace = true;
        for (const auto& piece : blueprint) {
            float cx = newPos.x + piece.offsetX * tileSize + tileSize * 0.5f;
            float cy = newPos.y + piece.offsetY * tileSize + tileSize * 0.5f;
            if (IsWall(cx, cy)) { canPlace = false; break; }
        }
        if (canPlace) for (const auto& piece : blueprint)
            obstacles.push_back({{ newPos.x + piece.offsetX * tileSize, newPos.y + piece.offsetY * tileSize }, piece.ID});
        }

    // ---- Spawn decorations ----
    if (decorations.size() < 50) {
        Vector2 newPos = randomNearPos();
        float   cx     = newPos.x + tileSize * 0.5f;
        float   cy     = newPos.y + tileSize * 0.5f;

        if (!IsWall(cx, cy)) {
            // Pick a random decoration type from the pool
            int pick = GetRandomValue(0, (int)decorationPool.size() - 1);
            SpawnDecoration(decorations, decorationPool[pick], newPos, tileSize);
        }
    }
}

void Map::Draw(Vector2 playerPos, int screenWidth, int screenHeight) {
    // ---- 1. Floor ----
    // TileHash gives a stable variant per tile coordinate — no flickering possible.
    // Change FLOOR_VARIANTS to however many floor tile variants you have in your tileset.
    static constexpr int FLOOR_VARIANTS = 4;
    
    // หาพิกัดมุมซ้ายบนของกล้อง (สมมติกล้องอยู่กึ่งกลางผู้เล่น)
    float startX = playerPos.x - (screenWidth / 2.0f);
    float startY = playerPos.y - (screenHeight / 2.0f);
    
    // ปัดเศษให้ลงล็อค tileSize เพื่อไม่ให้พื้นกระตุกเวลาเดิน
    int firstCol = (int)floorf(startX / tileSize);
    int firstRow = (int)floor(startY / tileSize);
    int colsToDraw = (screenWidth / tileSize) + 2;
    int rowsToDraw = (screenHeight / tileSize) + 2;

    for (int y = firstRow; y < firstRow + rowsToDraw; y++) {
        for (int x = firstCol; x < firstCol + colsToDraw; x++) {
            int tileIndex = (int)(TileHash(x, y) % FLOOR_VARIANTS);
            
            Rectangle crop = {tileIndex * 16.0f, 0.0f, 16.0f, 16.0f};
            Rectangle dest = {(float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize};
            DrawTexturePro(tileset, crop, dest, {0, 0}, 0.0f, WHITE);
        }
    }

    // ---- 2. Ravine ----
    //
    // The ravine is drawn by re-rendering the floor tile on top of itself
    // with a dark tint — no special tileset tiles needed.
    //
    // Layout per row (RAVINE_HALF_WIDTH = 3 as example):
    //
    //   dist:  3    2    1    0    1    2    3
    //        [dim][drk][dkr][ABY][dkr][drk][dim]
    //
    // Crossing rows: the centre CROSSING_WIDTH tiles draw as normal floor (ledge).
    //
    for (int y = firstRow; y < firstRow + rowsToDraw; y++) {
        float centerWorldX = GetRavineCenterX(y);
        int   centerTileX  = (int)floorf(centerWorldX / (float)tileSize);
        bool  isCrossing   = IsCrossingRow(y);

        for (int dx = -RAVINE_HALF_WIDTH; dx <= RAVINE_HALF_WIDTH; dx++) {
            int x    = centerTileX + dx;
            int dist = abs(dx);

            // On crossing rows, only the narrow centre ledge stays as floor;
            // the rest of the ravine width still falls away into the abyss.
            bool isLedge = isCrossing && (dist <= CROSSING_WIDTH / 2);

            if (isLedge) continue; // floor tile already drawn in step 1 — nothing to do

            // Reuse TileHash so the floor tile underneath the tint is identical to step 1
            int tileIndex = (int)(TileHash(x, y) % FLOOR_VARIANTS);
            Rectangle crop = {tileIndex * 16.0f, 0.0f, 16.0f, 16.0f};
            Rectangle dest = {(float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize};

            // Tint gets darker toward centre
            Color tint = GetRavineTint(dist);
            DrawTexturePro(tileset, crop, dest, {0, 0}, 0.0f, tint);
        }
    }

    // --- 3. Decorations (drawn under obstacles so pillars appear "in front") ---
    for (const auto& dec : decorations) {
        Rectangle crop = GetCrop(dec.ID);
        Rectangle dest = { dec.pos.x, dec.pos.y, (float)tileSize, (float)tileSize };
        DrawTexturePro(tileset, crop, dest, {0,0}, 0.0f, WHITE);
    }

    // --- 4. Obstacles ---
    for (const auto& obs : obstacles) {
        Rectangle crop = GetCrop(obs.ID);
        Rectangle dest = { obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize };
        DrawTexturePro(tileset, crop, dest, {0,0}, 0.0f, WHITE);
    }
}

bool Map::IsWall(float x, float y) {
    // เช็คว่าพิกัดที่เดินไป ชนกับสิ่งกีดขวางใน Vector หรือไม่
    for (const auto& obs : obstacles) {
        Rectangle obsRec = { obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize };
        if (CheckCollisionPointRec({x, y}, obsRec)) return true;
    }

    // Check ravine (ford crossings are NOT walls)
    int tileX = (int)floorf(x / (float)tileSize);
    int tileY = (int)floorf(y / (float)tileSize);
    if (IsRavineTile(tileX, tileY)) return true;

    return false;
}

bool Map::HitSpike(float x, float y) {
    // เช็คว่าพิกัดที่เดินไป ชนกับของตกแต่งที่เป็นอันตรายใน Vector หรือไม่
    for (const auto& dec : decorations) {
        // ID 26 คือ spike
        if (dec.ID == 26) {
            Rectangle decRec = { dec.pos.x, dec.pos.y, (float)tileSize, (float)tileSize };
            if (CheckCollisionPointRec({x, y}, decRec)) return true;
        }
    }

    return false;
}