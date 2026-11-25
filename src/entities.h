#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float energy;
    float max_energy;
    uint8_t signature;
    float radius;
} Player;

void init_player(Player *p);
void update_player(Player *p, float dt);
void draw_player(const Player *p);

#endif