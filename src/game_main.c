#include "engine.h"
#include "font.h"
#include "input.h"
#include "platform.h"

#include "../alex/map.h"
#include "game_main.h"
#include "script.h"
#include "token.h"

#define INSIDE_DATA
#include "data_files.h"

#include "control.h"
extern Tcontrol ctrl;
extern Toptions options;

extern const char *_map_mapping[MAP_MAX];
extern const char *_image_mono_mapping[I_MAX];
BITMAP __bitmaps[I_MAX];
BITMAP *_bitmaps[I_MAX];
extern BITMAP **bitmaps_mono;
extern BITMAP **bitmaps;

static float ft = 0.003;
static float ticks = 0;
static float wait_ticks = 0;

bool init_sound(Toptions *o);
void load_music(int id);
bool load_scripts();
int begin_script(int id);
int run_script(int id);

Uint32 fps_counter(Uint32 interval, void *param);
Uint32 cycle_counter(Uint32 interval, void *param);

global_t g;

entity_vtab_t entity_vtab_other = {};

Tmap *theMap = NULL;
int level = 0;

int init_game(const char *map_file);
void new_game(int reset_player_data);
void new_level(const char *fname, int level_id, int draw);
int play(void);

void draw_frame(BITMAP *bmp, int _status_bar);

void _load_level() {
  char *map_file = _map_mapping[level];
  new_level(map_file, -1, 0);
}

void draw_select_starting_level(BITMAP *bmp, int level, int min, int max);

static void select_level_init(void) {
  wait_ticks = 1.0;
  init_game("??");
  new_game(0);
  char *map_file = _map_mapping[0];
  new_level(map_file, -1, 0);
}

int select_starting_level();
static void select_level_update(void) {
  ticks += engine.tick;
  if (ticks >= ft) {
    fps_counter(0, NULL);
    cycle_counter(0, NULL);
    ticks -= ft;
  }

  wait_ticks -= engine.tick;

  int ret = select_starting_level();
  if (ret > 0) {
    level = (ret - 1);
  }

  if (wait_ticks > 0) return;
  if (ret == -2) {
    engine_set_scene(&scene_game);
  } else if (ret == -1) {
    engine_set_scene(&scene_menu);
  }
}

static void select_level_draw(void) {
  // draw_frame(NULL, 0);
  draw_select_starting_level(NULL, level + 1, 1, options.max_levels);
}

scene_t scene_select_level = {
    .init = select_level_init,
    .update = select_level_update,
    .draw = select_level_draw,
};

void draw_title(BITMAP *bmp, int tick);
int do_main_menu();

static void menu_init(void) {
  wait_ticks = 0.5;

  init_game("??");
  new_game(0);
  char *map_file = _map_mapping[0];
  new_level(map_file, -1, 0);
}

static void menu_update(void) {
  ticks += engine.tick;
  if (ticks >= ft) {
    fps_counter(0, NULL);
    cycle_counter(0, NULL);
    ticks -= ft;
  }

  wait_ticks--;
  if (wait_ticks > 0) return;

  if (do_main_menu() == 5 /* GS_PLAY */) {
    if (options.max_levels > 1) {
      engine_set_scene(&scene_select_level);
    } else {
      engine_set_scene(&scene_game);
    }
  }
}

void draw_menu_scroller();
static void menu_draw(void) {
  draw_frame(NULL, 0);
  draw_title(NULL, (int)(ticks * 100));
}

scene_t scene_menu = {
    .init = menu_init,
    .update = menu_update,
    .draw = menu_draw,
};

static float go_ticks = 0;
static bool go_esc = false;
static void game_over_init(void) {
  go_ticks = 0;
  go_esc = false;
}

static void game_over_update(void) {
  int status = play();
  ticks += engine.tick;
  if (ticks >= ft) {
    fps_counter(0, NULL);
    cycle_counter(0, NULL);
    ticks -= ft;
  }

  go_ticks += engine.tick;
  if (go_ticks > 1.5) {
    update_platform_controls();
    if (key[ctrl.key_jump] || key[ctrl.key_fire]) {
      go_esc = true;
    }
  }

  if ((!key[ctrl.key_jump] && !key[ctrl.key_fire]) && go_esc) {
    engine_set_scene(&scene_menu);
  }
}

void show_game_over();

static void game_over_draw(void) {
  draw_frame(NULL, 0);
  // render_push();
  // render_translate(vec2(20, 0));
  show_game_over();
  // render_pop();
}

scene_t scene_game_over = {
    .init = game_over_init,
    .update = game_over_update,
    .draw = game_over_draw,
};

int *get_level_stats(int level);

static void cut_init(void) {
  options.max_levels = (level + 1) + 1;
  int *stats = get_level_stats(level);
  options.stars[level] = stats[1];
  options.cherries[level] = stats[2];
  save_options(&options, "config");
}

static void cut_update(void) {
  int status = play();
  ticks += engine.tick;
  if (ticks >= ft) {
    fps_counter(0, NULL);
    cycle_counter(0, NULL);
    ticks -= ft;
  }

  update_platform_controls();

  if (key[ctrl.key_fire]) {
    level++;
    if (level >= MAP_MAX) {
      engine_set_scene(&scene_outro);
    } else {
      engine_set_scene(&scene_game);
    }
  }
}

void draw_cutscene(BITMAP *bmp, int org_level, int _level, int _lives,
                   int _stars, int _cherries);

static void cut_draw(void) {
  draw_frame(NULL, 0);
  render_push();
  render_translate(vec2(20, 0));
  int *stats = get_level_stats(level);
  draw_cutscene(NULL, level, stats[0], stats[1], stats[1], stats[2]);
  render_pop();
}

scene_t scene_cut = {
    .init = cut_init,
    .update = cut_update,
    .draw = cut_draw,
};

static void intro_init(void) {
  load_scripts();
  begin_script(0);
}

static void intro_update(void) {
  if (run_script(1) == -1) {
    engine_set_scene(&scene_menu);
  }
}

void draw_script();

static void intro_draw(void) {
  render_push();
  render_translate(vec2(20, 0));
  rectfill(NULL, 0, 0, 400, 240, 4);
  draw_script();
  rectfill(NULL, 0, 0, 40, 240, 1);
  rectfill(NULL, 400 - 40, 0, 400, 240, 1);
  render_pop();
}

scene_t scene_intro = {
    .init = intro_init,
    .update = intro_update,
    .draw = intro_draw,
};

static void outro_init(void) {
  load_scripts();
  begin_script(1);
}

static void outro_update(void) {
  if (run_script(1) == -1) {
    engine_set_scene(&scene_menu);
  }
}

void draw_script();

static void outro_draw(void) {
  render_push();
  render_translate(vec2(20, 0));
  rectfill(NULL, 0, 0, 400, 240, 4);
  draw_script();
  rectfill(NULL, 0, 0, 40, 240, 1);
  rectfill(NULL, 400 - 40, 0, 400, 240, 1);
  render_pop();
}

scene_t scene_outro = {
    .init = outro_init,
    .update = outro_update,
    .draw = outro_draw,
};

static void preload_init(void) {
  bitmaps_mono = _bitmaps;
  bitmaps = bitmaps_mono;

  for (int i = 0; i < I_MAX; i++) {
    char *path = _image_mono_mapping[i];
    char tmpQoi[1024];
    strcpy(tmpQoi, path);
    strcpy(tmpQoi + (strlen(tmpQoi))-3, "qoi");

    char tmpPng[1024];
    strcpy(tmpPng, path);
    strcpy(tmpPng + (strlen(tmpPng))-3, "png");

    image_t *img = image(tmpQoi);
    vec2i_t size = image_size(img);

    _bitmaps[i] = &__bitmaps[i];
    bitmaps_mono[i]->tex = img;
    bitmaps_mono[i]->w = size.x;
    bitmaps_mono[i]->h = size.y;
  }

  load_music(MSC_GAME);
  prepare_font(F_GAME);
}

static void preload_update(void) {
  printf("preload\n");
  // engine_set_scene(&scene_game_over);
  engine_set_scene(&scene_intro);
  // engine_set_scene(&scene_menu);
  // engine_set_scene(&scene_select_level);
  // engine_set_scene(&scene_game);
  // engine_set_scene(&scene_cut);
  // engine_set_scene(&scene_outro);
}

static void preload_draw(void) {}

scene_t scene_preload = {
    .init = preload_init,
    .update = preload_update,
    .draw = preload_draw,
};

static void game_init(void) {
  init_game("??");
  new_game(1);
  _load_level();
  go_ticks = 0;
  go_esc = false;
}

static void game_update(void) {
  int status = play();

  if (status == 6 /*GS_LEVEL_DONE*/) {
    // _load_level();
    engine_set_scene(&scene_cut);
  }

  if (status == 8 /* GS_GAME_DIED */) {
    int *stats = get_level_stats(level);
    if (stats[1] == 0) {
      engine_set_scene(&scene_game_over);
      // printf("game over!\n");
    } else {
      _load_level();
    }
  }

  ticks += engine.tick;
  if (ticks >= ft) {
    fps_counter(0, NULL);
    cycle_counter(0, NULL);
    ticks -= ft;
  }

  go_ticks += engine.tick;
}

void show_lets_go();
static void game_draw(void) {
  render_push();
  draw_frame(NULL, 1);
  if (go_ticks < 2.0) {
    show_lets_go();
  }
  render_pop();
}

scene_t scene_game = {
    .init = game_init,
    .update = game_update,
    .draw = game_draw,
};

void main_init(void) {
  init_sound(0);

  // Gamepad
  input_bind(INPUT_GAMEPAD_DPAD_LEFT, A_LEFT);
  input_bind(INPUT_GAMEPAD_DPAD_RIGHT, A_RIGHT);
  input_bind(INPUT_GAMEPAD_L_STICK_LEFT, A_LEFT);
  input_bind(INPUT_GAMEPAD_L_STICK_RIGHT, A_RIGHT);
  input_bind(INPUT_GAMEPAD_X, A_JUMP);
  input_bind(INPUT_GAMEPAD_B, A_JUMP);
  input_bind(INPUT_GAMEPAD_A, A_SHOOT);

  // Keyboard
  input_bind(INPUT_KEY_LEFT, A_LEFT);
  input_bind(INPUT_KEY_RIGHT, A_RIGHT);
  input_bind(INPUT_KEY_UP, A_UP);
  input_bind(INPUT_KEY_DOWN, A_DOWN);
  input_bind(INPUT_KEY_X, A_JUMP);
  input_bind(INPUT_KEY_C, A_SHOOT);

  g.noise = noise(8);
  // g.font = font("assets/font_04b03.qoi", "assets/font_04b03.json");
  // g.music = sound(sound_source("assets/music/biochemie.qoa"));
  // sound_set_loop(g.music, true);
  // sound_set_global_volume(0.75);

  load_options(&options, "config");
  engine_set_scene(&scene_preload);
}

void main_cleanup(void) {}
