#include "raylib.h"
#include <vector>
#include <iostream>

int main() {
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib C++17 Example");

    // Define a vector to hold circle positions
    std::vector<Vector2> circles;
    circles.push_back({200.0f, 300.0f});
    circles.push_back({400.0f, 300.0f});
    circles.push_back({600.0f, 300.0f});

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update logic can be added here

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw circles from the vector
        for (const auto& circle : circles) {
            DrawCircleV(circle, 50.0f, BLUE);
        }

        DrawText("C++17 with Raylib", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    // Cleanup and close the window
    CloseWindow();
    return 0;
}
