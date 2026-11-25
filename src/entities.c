#include "entities.h"
#include "simulation.h"

void init_player(Player *p) {
    p->pos = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    p->vel = (Vector2){ 0.0f, 0.0f };
    p->max_energy = 100.0f;
    p->energy = 100.0f;
    p->signature = 0x01;
    p->radius = 12.0f;
}

void update_player(Player *p, float dt) {
    float accel = 600.0f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) p->vel.x += accel * dt;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) p->vel.x -= accel * dt;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) p->vel.y += accel * dt;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) p->vel.y -= accel * dt;

    p->vel.x *= 0.94f;
    p->vel.y *= 0.94f;

    p->pos.x += p->vel.x * dt;
    p->pos.y += p->vel.y * dt;

    p->energy -= 1.5f * dt;
    if (p->energy < 0) p->energy = 0;
}

void draw_player(const Player *p) {
    DrawCircleV(p->pos, p->radius, SKYBLUE);
}