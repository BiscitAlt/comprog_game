#include "raylib.h"

#include <vector>
#include <iostream>

void textKey(float speed)
{
    DrawText("WASD or Arrow keys to move", 10, 700, 20, DARKGRAY);
    DrawText("Mouse Wheel to Zoom in-out", 10, 680, 20, DARKGRAY);
    DrawText("R to reset Zoom", 10, 660, 20, DARKGRAY);
    DrawText(TextFormat("Speed: %f", speed), 10, 10, 20, DARKGRAY);
}

void cursor(Vector2 mousePos)
{
    DrawCircleV(mousePos, 4, BLACK);
    DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), {mousePos.x + 5, mousePos.y + 5}, 20, 2, BLACK);
}