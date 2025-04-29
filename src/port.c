#include "port.h"
#include "data_files.h"
#include "types.h"

#include "game_main.h"

#include <stdarg.h>

extern BITMAP **bitmaps;
BITMAP fontBitmaps[F_MAX];

int font_char_width = 7;
int font_char_height = 8;
int font_padding_width = 9;
int font_padding_height = 8;

typedef struct font_rect_t {
  int x;
  int y;
  int w;
  int h;
} font_rect_t;

font_rect_t font_charRects[256];

BITMAP *create_bitmap(int w, int h) { return NULL; }

BITMAP *load_bmp_from_mem(void *data, size_t size) { return NULL; }

void destroy_bitmap(BITMAP *bmp) {}

void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x,
          int dest_y, int width, int height) {}

void stretch_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int w, int h) {}

void draw_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y) {
  if (sprite && sprite->tex) {
    // printf("spr %d %d\n", sprite->w, sprite->h);
    image_t *img = sprite->tex;
    vec2_t pos = vec2(x, y);
    image_draw(img, pos);
  }
}

void draw_tile(BITMAP *bmp, int tile_id, int x, int y) {
  BITMAP *tileset = bitmaps[I_TILESET];
  if (tileset && tileset->tex) {
    image_t *img = tileset->tex;
    vec2_t pos = vec2(x, y);
    image_draw_tile(img, tile_id, vec2i(16, 16), pos);
  }
}

void draw_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
  draw_sprite(bmp, sprite, x, y);
  if (sprite && sprite->tex) {
    // printf("spr %d %d\n", sprite->w, sprite->h);
    image_t *img = sprite->tex;
    vec2_t pos = vec2(x, y);
    image_draw_tile_ex(img, 0, vec2i(sprite->w, sprite->h), pos, 0, 1,
                       rgba_white());
  }
}

void draw_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
  if (sprite && sprite->tex) {
    // printf("spr %d %d\n", sprite->w, sprite->h);
    image_t *img = sprite->tex;
    vec2_t pos = vec2(x, y);
    image_draw_tile_ex(img, 0, vec2i(sprite->w, sprite->h), pos, 1, 0,
                       rgba_white());
  }
}

void draw_sprite_vh_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
  draw_sprite_v_flip(bmp, sprite, x, y);
}

void rotate_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int angle) {
  draw_sprite(bmp, sprite, x, y);
}

void rotate_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y,
                          int angle) {
  draw_sprite_v_flip(bmp, sprite, x, y);
}

void _drawRect(vec2_t v1, vec2_t v2, int color);

void rectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
#ifdef RENDER_PLAYDATE
  int width = x2 - x1;
  int height = y2 - y1;
  if (color != 4) {
    playdate->graphics->fillRect(x1, y1, width, height, kColorBlack);
  } else {
    playdate->graphics->fillRect(x1, y1, width, height, kColorWhite);
  }
#else
  _drawRect(vec2(x1, y1), vec2(x2, y2), color);
#endif
}

void _drawLine(vec2_t v1, vec2_t v2, int color);

void line(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
  _drawLine(vec2(x1, y1), vec2(x2, y2), color);
}

void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {}

void putpixel(BITMAP *bmp, int x, int y, int color) {}

void set_clip_rect(BITMAP *bmp, int x1, int y1, int x2, int y2) {}

void reset_clip_rect(BITMAP *bmp) {}

void clear_bitmap(BITMAP *bmp) {}

void clear_screen() {}

void clear_to_color(BITMAP *bmp, int color) {}

void draw_character_ex(BITMAP *bmp, BITMAP *sprite, int x, int y, int color) {}

int gui_fg_color, gui_bg_color;

void switch_palette(bool colorize) {}

extern const char *_font_mapping[F_MAX];

// text
void prepare_font(int id) {
  for (int i = 0; i < F_MAX; i++) {
    BITMAP *font = &fontBitmaps[i];
    char *path = _font_mapping[i];
    char tmpQoi[1024];
    strcpy(tmpQoi, path);
    strcpy(tmpQoi + (strlen(tmpQoi))-3, "qoi");

    image_t *img = image(tmpQoi);
    printf("%s\n", tmpQoi);
    if (img) {
      printf("loaded %s\n", tmpQoi);
      vec2i_t size = image_size(img);
      font->tex = img;
      font->w = size.x;
      font->h = size.y;
    }
  }

  int char_code = 0;
  // empty first 31 (control chars)
  for (int c = 0; c < 32; ++c) {
    font_charRects[char_code].x = 0;
    font_charRects[char_code].y = 0;
    font_charRects[char_code].w = 0;
    font_charRects[char_code].h = 0;
    ++char_code;
  }

  for (int r = 0; r < 14; ++r) {
    for (int c = 0; c < 16; ++c) {
      font_charRects[char_code].x =
          1 + font_char_width * c + (font_padding_width * c);
      font_charRects[char_code].y =
          1 + font_char_height * r + (font_padding_height * r);
      font_charRects[char_code].w = font_char_width;
      font_charRects[char_code].h = font_char_height;
      ++char_code;
    }
  }

  // width fixup
  font_charRects['W'].w += 2;
  font_charRects['w'].w += 2;
  font_charRects['M'].w += 2;
  font_charRects['m'].w += 2;
}

int text_length(const char *txt) { return strlen(txt) * font_char_width; }

void textout_ex_helper(int pos, BITMAP *bmp, const char *s, int x, int y,
                       int color, int bg) {
#if 1
  BITMAP *font = &fontBitmaps[F_GAME];

  if (color != 4) {
    font = &fontBitmaps[F_GAME + 1];
  }

  // left
  int current_x = x;
  int current_y = y;
  // center
  if (pos == 0) {
    current_x = x - text_length(s) / 2;
  } else if (pos == 1) {
    // right
    current_x = x - text_length(s);
  }

  size_t length = strlen(s);
  for (size_t i = 0; i < length; ++i) {
    int32_t code = s[i];
    switch (code) {
    case '\n':
      // printf("enter?\n");
      current_x = x;
      current_y += font_char_height + 1;
      break;
    default: {
      // SDL_Rect r = {current_x, current_y, font_charRects[code].w,
      // font_charRects[code].h}; SDL_RenderCopy(renderer, font.texture,
      // &font_charRects[code], &r);

      {
        font_rect_t fr = font_charRects[code];
        image_t *img = font->tex;
        if (img) {
          vec2_t dst_pos = vec2(current_x, current_y);
          vec2_t dst_size = vec2(fr.w, fr.h);
          vec2_t src_pos = vec2(fr.x, fr.y);
          vec2_t src_size = vec2(fr.w, fr.h);
          image_draw_ex(img, src_pos, src_size, dst_pos, dst_size,
                        rgba_white());
        }
      }

      current_x += font_char_width;
      break;
    }
    }
  }
#endif
}

void textprintf_ex_helper(int pos, BITMAP *bmp, int x, int y, int color, int bg,
                          const char *fmt, ...) {
  char buf[100];
  va_list arglist;

  va_start(arglist, fmt);
  vsnprintf(buf, 100, fmt, arglist);
  va_end(arglist);

  textout_ex_helper(pos, bmp, buf, x, y, color, bg);
}

int text_height() { return font_char_height; }

// keyboard

#include "control.h"
extern Tcontrol ctrl;

Uint8 key[256];

void update_platform_controls() {
  memset(key, 0, sizeof(Uint8) * 20);
  ctrl.key_up = A_UP;
  ctrl.key_down = A_DOWN;
  ctrl.key_left = A_LEFT;
  ctrl.key_right = A_RIGHT;
  ctrl.key_jump = A_JUMP;
  ctrl.key_fire = A_SHOOT;

  key[ctrl.key_up] = input_state(A_UP);
  key[ctrl.key_down] = input_state(A_DOWN);
  key[ctrl.key_left] = input_state(A_LEFT);
  key[ctrl.key_right] = input_state(A_RIGHT);
  key[ctrl.key_jump] = input_state(A_JUMP);
  key[ctrl.key_fire] = input_state(A_SHOOT);
}

// SDL_Scancode sdl_key = 0;

bool keypressed() { return false; }

int readkey() { return 0; }

// file
int exists(const char *f) { return 0; }

int GetMouseX() { return 0; }

int GetMouseY() { return 0; }

bool MouseBtn() { return 0; }

// waits for user to strike a key, or x seconds
void wait_key(int seconds) {}

void fade_helper(BITMAP *bmp, bool fadein, Uint8 col, int delay) {}

// fades in from white to 4 color palette
void fade_in_pal(BITMAP *bmp, int delay) {}
// fades 4 color palette to white
void fade_out_pal(BITMAP *bmp, int delay) {}
// fade in from black to 4 colors pal
void fade_in_pal_black(BITMAP *bmp, int delay) {}
// fades 4 color palette to black
void fade_out_pal_black(BITMAP *bmp, int delay) {}

// blits anything to screen
void blit_to_screen(BITMAP *bmp) {}

#if 0
  // fix palette
  for(i = 0; i < 256; i ++) {
    org_pal[i].r = ((RGB *)data[0].dat)[i].r;
    org_pal[i].g = ((RGB *)data[0].dat)[i].g;
    org_pal[i].b = ((RGB *)data[0].dat)[i].b;
  }

  // fix some palette entries
  ((RGB *)data[0].dat)[0].r = 0;
  ((RGB *)data[0].dat)[0].g = 0;
  ((RGB *)data[0].dat)[0].b = 0;
  fix_gui_colors();
  set_palette(data[0].dat);
#endif

// sets up the gui colors
void fix_gui_colors() {}

// lighten or darken a 4 color bitmap
void transform_bitmap(BITMAP *bmp, int steps) {}

void init_platform() {}

void uninit_platform() {}

bool fullscreen = false;
int zoom = 1;

void make_window(Toptions *o) {}

void set_window_title(const char *title) {}

void rest(int ms) {}

int GetScreenW() { return 0; }
int GetScreenH() { return 0; }

void SetZoom(int z, Toptions *o) {}

void IncreaseZoom(Toptions *o) {}

void DecreaseZoom(Toptions *o) {}

void ToggleFullScreen(Toptions *o) {}

// shows a little message
void msg_box(const char *str) {}

void take_screenshot_platform(BITMAP *bmp, const char *filename) {}

int check_and_create_dir(const char *name) { return 0; }

char *get_homedir(void) { return 0; }

#ifdef PLATFORM_PLAYDATE
char *_readLineFileFunction(void *fp, void *buf, unsigned int len) {
  PlaydateAPI *pd = playdate;
  if (!fp) {
    return 0;
  }

  char *buffer = (char *)buf;
  int idx = 0;

  char c;
  while (pd->file->read(fp, &c, 1) == 1) { // Read one byte (char) at a time
    if (c == '\n' || c == '\r') {          // Stop at newline or carriage return
      buffer[idx++] = 0;
      break;
    }
    buffer[idx++] = c;
    if (idx == len)
      break;
  }

  if (idx == 0)
    return NULL;

  buffer[idx] = 0;
  return buf;
}
#endif