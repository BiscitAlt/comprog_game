#include "raylib.h"

#include <vector>
#include <iostream>

struct Enemy {
    Vector2 position;
    bool active;
    int type; // 0 = Slime, 1 = Goblin
};