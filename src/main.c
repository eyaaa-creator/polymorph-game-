#include "raylib.h"
#include "simulation.h"
#include "entities.h"
#include "ai.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Polymorph");
    SetTargetFPS(60);

    // 1. Initialisation (avant la boucle)
    Player player;
    init_player(&player);

    AIManager ai;
    init_ai(&ai);

    float traces[ROWS][COLS] = { 0 };
    bool game_over = false;

    // 2. Boucle principale du jeu
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Mise à jour de la logique
        if (!game_over) {
            update_player(&player, dt);
            update_ai(&ai, &player, traces, dt, &game_over);

            // Défaite si la barre d'énergie tombe à zéro
            if (player.energy <= 0.0f) {
                game_over = true;
            }
        } else if (IsKeyPressed(KEY_R)) {
            // Permet de relancer rapidement un test avec la touche R
            init_player(&player);
            init_ai(&ai);
            game_over = false;
        }

        // 3. Affichage graphique (toujours entre BeginDrawing et EndDrawing)
        BeginDrawing();
        ClearBackground((Color){ 15, 18, 25, 255 });

        draw_ai(&ai);       // Dessiné en premier pour que les cônes restent sous le joueur
        draw_player(&player);

        // Barre d'énergie
        DrawRectangle(20, 20, 200, 20, DARKGRAY);
        DrawRectangle(20, 20, (int)(200 * (player.energy / player.max_energy)), 20, GREEN);
        DrawRectangleLines(20, 20, 200, 20, WHITE);

        if (game_over) {
            const char *msg = (player.energy <= 0.0f)
                ? "ENERGIE EPUISEE ! GAME OVER"
                : "CAPTURE ! GAME OVER";
            DrawText(msg, SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 20, 32, RED);
            DrawText("Appuie sur R pour rejouer", SCREEN_WIDTH / 2 - 135, SCREEN_HEIGHT / 2 + 25, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}