#ifndef MAIN_H
#define MAIN_H

#include "animation.h"
#include "camera.h"
#include "engine.h"
#include "entity.h"
#include "font.h"
#include "input.h"
#include "noise.h"
#include "platform.h"
#include "render.h"
#include "sound.h"
#include "types.h"
#include "utils.h"

#include "data_files.h"

// -----------------------------------------------------------------------------
// Button actions

typedef enum { A_UP, A_DOWN, A_LEFT, A_RIGHT, A_JUMP, A_SHOOT } action_t;

// -----------------------------------------------------------------------------
// Global data

typedef struct {
  font_t *font;
  noise_t *noise;
  sound_t music;
} global_t;

extern global_t g;

// -----------------------------------------------------------------------------
// Scenes

extern scene_t scene_game;
extern scene_t scene_intro;
extern scene_t scene_outro;
extern scene_t scene_menu;
extern scene_t scene_select_level;
extern scene_t scene_cut;
extern scene_t scene_game_over;

void game_set_level_path(char *path);
void game_set_checkpoint(entity_ref_t checkpoint);
void game_respawn(void);

entity_t *game_spawn_particle(vec2_t pos, float vel, float vel_variance,
                              float angle, float angle_variance,
                              anim_def_t *sheet);

#endif