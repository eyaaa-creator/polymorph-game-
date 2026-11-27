#include "ai.h"
#include <math.h>
#include <stddef.h>

// Normalise un angle entre -PI et +PI
static float normalize_angle(float angle) {
    while (angle > PI)  angle -= 2.0f * PI;
    while (angle < -PI) angle += 2.0f * PI;
    return angle;
}

// Vérifie si une cible se trouve dans le cône radar d'un garde
static bool is_in_radar(const Guard *g, Vector2 target_pos) {
    float dx = target_pos.x - g->pos.x;
    float dy = target_pos.y - g->pos.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > g->radar_range) {
        return false;
    }

    float angle_to_target = atan2f(dy, dx);
    float diff = normalize_angle(angle_to_target - g->angle);

    return fabsf(diff) <= (g->radar_fov * 0.5f);
}

// Cherche la cellule contenant la trace la plus forte autour du garde
static bool find_strongest_trace(const Guard *g, const float traces[ROWS][COLS], Vector2 *out_target) {
    if (traces == NULL) return false;

    int gx = (int)(g->pos.x / CELL_SIZE);
    int gy = (int)(g->pos.y / CELL_SIZE);
    int search_radius = 4; // Rayon de détection des traces en cellules

    float max_trace = 0.05f; // Seuil minimal pour ignorer les traces trop faibles
    bool found = false;

    for (int r = gy - search_radius; r <= gy + search_radius; r++) {
        for (int c = gx - search_radius; c <= gx + search_radius; c++) {
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                if (traces[r][c] > max_trace) {
                    max_trace = traces[r][c];
                    out_target->x = (c + 0.5f) * CELL_SIZE;
                    out_target->y = (r + 0.5f) * CELL_SIZE;
                    found = true;
                }
            }
        }
    }
    return found;
}

void init_ai(AIManager *ai) {
    ai->guard_count = 3;
    ai->blacklist = 0x00;

    // Garde 0 : patrouille dans le quart haut-gauche
    ai->guards[0] = (Guard){
        .pos = { 200.0f, 150.0f },
        .speed = 110.0f,
        .radius = 14.0f,
        .radar_range = 190.0f,
        .radar_fov = 60.0f * DEG2RAD,
        .waypoints = { {200, 150}, {520, 150}, {520, 340}, {200, 340} },
        .waypoint_count = 4,
        .current_waypoint = 1,
        .alert_state = false
    };

    // Garde 1 : patrouille dans la zone droite
    ai->guards[1] = (Guard){
        .pos = { 980.0f, 200.0f },
        .speed = 120.0f,
        .radius = 14.0f,
        .radar_range = 200.0f,
        .radar_fov = 55.0f * DEG2RAD,
        .waypoints = { {980, 200}, {980, 620}, {720, 620}, {720, 200} },
        .waypoint_count = 4,
        .current_waypoint = 1,
        .alert_state = false
    };

    // Garde 2 : patrouille dans le bas-gauche
    ai->guards[2] = (Guard){
        .pos = { 220.0f, 650.0f },
        .speed = 105.0f,
        .radius = 14.0f,
        .radar_range = 180.0f,
        .radar_fov = 65.0f * DEG2RAD,
        .waypoints = { {220, 650}, {560, 650}, {390, 460} },
        .waypoint_count = 3,
        .current_waypoint = 1,
        .alert_state = false
    };
}

void update_ai(AIManager *ai, const Player *player, const float traces[ROWS][COLS], float dt, bool *game_over) {
    for (int i = 0; i < ai->guard_count; i++) {
        Guard *g = &ai->guards[i];
        Vector2 target;
        g->alert_state = false;

        // 1. Détection visuelle par cône radar -> ajout à la liste noire partagée
        bool sees_player = is_in_radar(g, player->pos);
        if (sees_player) {
            ai->blacklist |= player->signature;
        }

        bool player_is_wanted = (ai->blacklist & player->signature) != 0;

        // 2. Choix de la cible de déplacement (Poursuite > Pistage de traces > Patrouille)
        if (sees_player && player_is_wanted) {
            target = player->pos;
            g->alert_state = true;
        } else if (find_strongest_trace(g, traces, &target)) {
            g->alert_state = true;
        } else {
            target = g->waypoints[g->current_waypoint];
            float dx_wp = target.x - g->pos.x;
            float dy_wp = target.y - g->pos.y;
            if (sqrtf(dx_wp * dx_wp + dy_wp * dy_wp) < 12.0f) {
                g->current_waypoint = (g->current_waypoint + 1) % g->waypoint_count;
                target = g->waypoints[g->current_waypoint];
            }
        }

        // 3. Rotation progressive et déplacement vers la cible
        float dx = target.x - g->pos.x;
        float dy = target.y - g->pos.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 1.0f) {
            float desired_angle = atan2f(dy, dx);
            float angle_diff = normalize_angle(desired_angle - g->angle);
            g->angle = normalize_angle(g->angle + angle_diff * 6.0f * dt);

            float current_speed = g->alert_state ? (g->speed * 1.35f) : g->speed;
            g->vel.x = cosf(g->angle) * current_speed;
            g->vel.y = sinf(g->angle) * current_speed;

            g->pos.x += g->vel.x * dt;
            g->pos.y += g->vel.y * dt;
        }

        // 4. Vérification de capture (Game Over uniquement si signature mémorisée)
        if (player_is_wanted && CheckCollisionCircles(g->pos, g->radius, player->pos, player->radius)) {
            *game_over = true;
        }
    }
}

void draw_ai(const AIManager *ai) {
    for (int i = 0; i < ai->guard_count; i++) {
        const Guard *g = &ai->guards[i];

        // Dessin du cône radar (rouge en alerte, jaune translucide en patrouille)
        float start_deg = (g->angle - g->radar_fov * 0.5f) * RAD2DEG;
        float end_deg   = (g->angle + g->radar_fov * 0.5f) * RAD2DEG;
        Color cone_color = g->alert_state
            ? (Color){ 230, 41, 55, 75 }
            : (Color){ 253, 249, 0, 50 };

        DrawCircleSector(g->pos, g->radar_range, start_deg, end_deg, 24, cone_color);

        // Dessin du corps du garde et de son indicateur de direction
        DrawCircleV(g->pos, g->radius, g->alert_state ? RED : MAROON);
        DrawCircleLines((int)g->pos.x, (int)g->pos.y, g->radius, WHITE);

        Vector2 eye = {
            g->pos.x + cosf(g->angle) * g->radius,
            g->pos.y + sinf(g->angle) * g->radius
        };
        DrawLineEx(g->pos, eye, 3.0f, WHITE);
    }

    // Affichage de la mémoire partagée (liste noire) en haut à droite
    DrawText(TextFormat("Liste noire (masque) : 0x%02X", ai->blacklist), SCREEN_WIDTH - 290, 20, 20, LIGHTGRAY);
}