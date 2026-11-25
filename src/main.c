#include "raylib.h"
#include "simulation.h"
#include "entities.h"
#include "ai.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Polymorph");
    SetTargetFPS(60);

    Player player;
    init_player(&player);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        update_player(&player, dt);

        BeginDrawing();
        ClearBackground((Color){ 15, 18, 25, 255 });

        draw_player(&player);

        DrawRectangle(20, 20, 200, 20, DARKGRAY);
        DrawRectangle(20, 20, (int)(200 * (player.energy / player.max_energy)), 20, GREEN);
        DrawRectangleLines(20, 20, 200, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}