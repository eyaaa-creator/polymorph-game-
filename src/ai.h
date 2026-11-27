#ifndef AI_H
#define AI_H

#include "raylib.h"
#include "entities.h"
#include "simulation.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_GUARDS 4
#define MAX_WAYPOINTS 4

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float angle;                        // Orientation en radians (0 = droite, PI/2 = bas)
    float speed;                        // Vitesse de déplacement (pixels/sec)
    float radius;                       // Rayon de collision du garde
    float radar_range;                  // Portée du cône radar (en pixels)
    float radar_fov;                    // Angle d'ouverture du cône (en radians)
    Vector2 waypoints[MAX_WAYPOINTS];   // Points de passage de la patrouille
    int waypoint_count;
    int current_waypoint;
    bool alert_state;                   // Vrai si le garde suit une trace ou poursuit le joueur
} Guard;

typedef struct {
    Guard guards[MAX_GUARDS];
    int guard_count;
    uint8_t blacklist;                  // Mémoire partagée des signatures repérées
} AIManager;

void init_ai(AIManager *ai);
void update_ai(AIManager *ai, const Player *player, const float traces[ROWS][COLS], float dt, bool *game_over);
void draw_ai(const AIManager *ai);

#endif