#include "map.h"

// TileHash — stable per-position hash, never touches global RNG.
// Uses Murmur3 finalizer mix for good distribution.
static unsigned int TileHash(int x, int y) {
    unsigned int h = (unsigned int)(x * 2246822519u ^ y * 3266489917u);
    h ^= h >> 16; h *= 0x85ebca6bu;
    h ^= h >> 13; h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// Obstascle Blueprint {x offset, y offset, tileID}
static const std::vector<BlueprintPiece> verticalPillarBlueprint = {{0,  0, 4}, {0, -1, 5}, {0, -2, 6}, {0, -3, 7}, {0, -4, 8}};
static const std::vector<BlueprintPiece> horizontalPillarBlueprint = {{0, 0, 9},{+1, 0, 10},{+2, 0, 11},{+3, 0, 12}, {+4, 0, 13}};

// Pool of all decoration
static const std::vector<DecorationDef>& GetDecorationPool() {
    static std::vector<DecorationDef> decorationPool;
    if (decorationPool.empty()) {
        decorationPool.push_back({ {{0,0,14}, {0,0,15}, {0,0,16}, {0,0,17}}, false }); // vase
        decorationPool.push_back({ {{0,0,18}, {1,0,19}, {0,1,20}, {1,1,21}}, true });  // broken tiles
        decorationPool.push_back({ {{0,0,22}, {0,0,23}, {0,0,24}, {0,0,25}}, false }); // candle
        decorationPool.push_back({ {{0,0,26}}, false });                               // spike
    }
    return decorationPool;
}

// GetCrop — converts a tile ID to its source rectangle on the tileset
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

// SpawnDecoration — internal helper
static void SpawnDecoration(std::vector<Decoration>& decorations, const DecorationDef& def, Vector2 origin, int tileSize)
{
    if (def.pieces.empty()) return;

    if (!def.multiTile) {
        // Single-tile variant: choose one piece at random
        int pick = GetRandomValue(0, (int)def.pieces.size() - 1);
        const BlueprintPiece& piece = def.pieces[pick];
            decorations.push_back({
                {origin.x + piece.offsetX * tileSize, 
                 origin.y + piece.offsetY * tileSize}, 
                 piece.ID
            });
    } else {
        // Multi-tile: place every piece at its offset
        for (const auto& piece : def.pieces)
            decorations.push_back({
                { origin.x + piece.offsetX * tileSize,
                  origin.y + piece.offsetY * tileSize },
                piece.ID
            });
    }
}

template<typename T>
static void DespawnDistant(std::vector<T>& list, Vector2 playerPos, float maxDistSq) {
    for (int i = list.size() - 1; i >= 0; i--) {
        float distX = playerPos.x - list[i].pos.x;
        float distY = playerPos.y - list[i].pos.y;
        if ((distX * distX) + (distY * distY) > maxDistSq) {
            list.erase(list.begin() + i);
        }
    }
}

void Map::LoadAssets() {
    tileset = LoadTexture("assets\\tileset.png");
    if (tileset.id == 0) TraceLog(LOG_ERROR, "FAILED TO LOAD TILESET!");
    else TraceLog(LOG_INFO, "TILESET LOADED SUCCESSFULLY!");
}

void Map::UnloadAssets() {
    UnloadTexture(tileset);
}

void Map::UpdateMap(Vector2 playerPos) {
    const float despawnRadiusSq = 1200.0f * 1200.0f;
    // 1. Erase further obstascles
    DespawnDistant(obstacles, playerPos, despawnRadiusSq);
    DespawnDistant(decorations, playerPos, despawnRadiusSq);

    // lambda: random spawn position near player
    auto randomNearPos = [&]() -> Vector2 {
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist  = {GenerateMap ? (float)GetRandomValue(10,600) : (float)GetRandomValue(600,800)};
        return {
            roundf((playerPos.x + cosf(angle) * dist) / tileSize) * tileSize,
            roundf((playerPos.y + sinf(angle) * dist) / tileSize) * tileSize
        };
    };

    // ---- Spawn new obstacles (pillars) ----
    if (obstacles.size() < maxObstacles) {
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
    if (decorations.size() < maxDecorations) {
        Vector2 newPos = randomNearPos();
        float   cx     = newPos.x + tileSize * 0.5f;
        float   cy     = newPos.y + tileSize * 0.5f;

        if (!IsWall(cx, cy)) {
            const auto& pool = GetDecorationPool();
            // Pick a random decoration type from the pool
            int pick = GetRandomValue(0, (int)pool.size() - 1);
            SpawnDecoration(decorations, pool[pick], newPos, tileSize);
        }
    }
}

void Map::Draw(Vector2 playerPos, int screenWidth, int screenHeight) {
    // Floor
    static constexpr int FLOOR_VARIANTS = 4;
    
    // Find top left corner
    float startX = playerPos.x - (screenWidth / 2.0f);
    float startY = playerPos.y - (screenHeight / 2.0f);
    
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

    // Decorations
    for (const auto& dec : decorations) {
        Rectangle crop = GetCrop(dec.ID);
        Rectangle dest = { dec.pos.x, dec.pos.y, (float)tileSize, (float)tileSize };
        DrawTexturePro(tileset, crop, dest, {0,0}, 0.0f, WHITE);
    }

    // Obstacles
    for (const auto& obs : obstacles) {
        Rectangle crop = GetCrop(obs.ID);
        Rectangle dest = { obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize };
        DrawTexturePro(tileset, crop, dest, {0,0}, 0.0f, WHITE);
    }
}

bool IsEntityColliding(Vector2 pos, Vector2 size, Map& map) {
    return map.IsWall(pos.x, pos.y) || 
           map.IsWall(pos.x + size.x, pos.y) || 
           map.IsWall(pos.x, pos.y + size.y) || 
           map.IsWall(pos.x + size.x, pos.y + size.y);
}

bool Map::IsWall(float x, float y) {
    for (const auto& obs : obstacles) {
        Rectangle obsRec = { obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize };
        if (CheckCollisionPointRec({x, y}, obsRec)) return true;
    }
    return false;
}

bool Map::HitSpike(float x, float y) {
    for (const auto& dec : decorations) {
        // ID 26 (spike)
        if (dec.ID == 26) {
            Rectangle decRec = { dec.pos.x, dec.pos.y, (float)tileSize, (float)tileSize };
            if (CheckCollisionPointRec({x, y}, decRec)) return true;
        }
    }
    return false;
}