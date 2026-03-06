#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"

struct Gem {
    Vector2 pos;
    int value;
    bool active;
    Color color;
};

struct Box {
    Rectangle rect;
    Color color;
    bool isSolid;
};

#endif