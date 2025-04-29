/**************************************************************
 *         _____    __                       _____            *
 *        /  _  \  |  |    ____  ___  ___   /  |  |           *
 *       /  /_\  \ |  |  _/ __ \ \  \/  /  /   |  |_          *
 *      /    |    \|  |__\  ___/  >    <  /    ^   /          *
 *      \____|__  /|____/ \___  >/__/\_ \ \____   |           *
 *              \/            \/       \/      |__|           *
 *                                                            *
 **************************************************************
 *    (c) Free Lunch Design 2003                              *
 *    by Johan Peitz - http://www.freelunchdesign.com         *
 *    SDL2 port by carstene1ns - https:/f4ke.de/dev/alex4     *
 **************************************************************
 *    This source code is released under the The GNU          *
 *    General Public License (GPL). Please refer to the       *
 *    document license.txt in the source directory or         *
 *    http://www.gnu.org for license information.             *
 **************************************************************/

#include "map.h"
#include "alex4.h"
#include "data.h"
#include "port.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// creates one splendid map
Tmap *create_map(int w, int h) {
  Tmap *m;

  m = malloc(sizeof(Tmap));
  if (m == NULL)
    return NULL;

  strcpy(m->name, "noname");

  m->boss_level = 0;
  m->num_enemies = 0;
  m->start_x = 2;
  m->start_y = 4;
  m->win_conditions = MAP_WIN_EXIT;
  m->win_conditions_fullfilled = 0;

  m->offset_x = 0;
  m->offset_y = 0;
  m->width = w;
  m->height = h;
  m->dat = malloc(w * h * sizeof(Tmappos));

  if (m->dat == NULL) {
    free(m);
    return NULL;
  }

  // clear map
  memset(m->dat, 0, w * h * sizeof(Tmappos));

  return m;
}

#if 0

// generic map loader
Tmap *load_map_from_RWops(SDL_RWops *rw) {
	Tmap *m;
	char header[6];

	if (!rw) return NULL;

	// does the header match?
	if (SDL_RWread(rw, header, 6, 1) != 1) {
		SDL_RWclose(rw);
		return NULL;
	}
	if (header[0] != 'A' && header[1] != 'X' && header[2] != '4' && header[3] != 'M' && header[4] != 'A' && header[5] != 'P') {
		SDL_RWclose(rw);
		return NULL;
	}

	// get memory
	m = malloc(sizeof(Tmap));
	if (m == NULL) {
		SDL_RWclose(rw);
		return NULL;
	}

	Sint64 pos = SDL_RWtell(rw);

	// read datastruct
	// a mapfile contain a raw dump of the Tmap struct made on an i386
	// the code below reads these struct dumps in an arch neutral manner
	// Note this dumps contains pointers, these are not used because these
	// ofcourse point to some no longer valid address.
	SDL_RWread(rw, m, 64, 1);       // first 64 bytes data
	m->width = SDL_ReadLE32(rw);
	m->height = SDL_ReadLE32(rw);
	SDL_RWseek(rw, 4, RW_SEEK_CUR); // skip first pointer
	m->offset_x = SDL_ReadLE32(rw);
	m->offset_y = SDL_ReadLE32(rw);
	SDL_RWseek(rw, 4, RW_SEEK_CUR); // skip second pointer
	m->start_x = SDL_ReadLE32(rw);
	m->start_y = SDL_ReadLE32(rw);
	if(SDL_RWtell(rw) != pos + 96) {
		SDL_RWclose(rw);
		free(m);
		return NULL;
	}

	// read map data
	m->dat = malloc(m->width * m->height * sizeof(Tmappos));
	if (m->dat == NULL) {
		SDL_RWclose(rw);
		free(m);
		return NULL;
	}

	if (SDL_RWread(rw, m->dat, sizeof(Tmappos), m->width * m->height) != (Sint64)m->width * m->height) {
		SDL_RWclose(rw);
		free(m->dat);
		free(m);
		return NULL;
	}

	return m;
}

// loads one splendind map from disk
Tmap *load_map(const char *fname) {
	Tmap *m;
	// open file
	SDL_RWops* rw = SDL_RWFromFile(fname, "rb");
	if (rw == NULL) return NULL;

	m = load_map_from_RWops(rw);
	if(!m) return NULL;

	// close file
	SDL_RWclose(rw);

	return m;
}

// loads one splendind map from memory
Tmap *load_map_from_memory(int id) {
	Tmap *m;
	SDL_RWops* rw = SDL_RWFromConstMem(maps[id].dat, maps[id].size);
	m = load_map_from_RWops(rw);
	if(!m) return NULL;

	// close file
	SDL_RWclose(rw);

	return m;
}

Tmap *load_scriptmap_from_memory(int id) {
	Tmap *m;
	SDL_RWops* rw = SDL_RWFromConstMem(scriptmaps[id].dat, scriptmaps[id].size);
	m = load_map_from_RWops(rw);
	if(!m) return NULL;

	// close file
	SDL_RWclose(rw);

	return m;
}

// saves a map to file
int save_map(Tmap *m, char *fname) {
	char header[6] = "AX4MAP";
	
	// open file
	SDL_RWops* rw = SDL_RWFromFile(fname, "wb");
	if (rw == NULL) return false;

	// write header
	if (SDL_RWwrite(rw, header, 6, 1) != 1) {
		SDL_RWclose(rw);
		return false;
	}

	Sint64 pos = SDL_RWtell(rw);

	// write datastruct
	// a mapfile should contain a raw dump of the Tmap struct as made on an
	// i386 the code below writes a struct dump as an i386 in an arch
	// neutral manner
	SDL_RWwrite(rw, m, 64, 1);      // first 64 bytes data
	SDL_WriteLE32(rw, m->width);
	SDL_WriteLE32(rw, m->height);
	SDL_RWwrite(rw, "SKIP", 4, 1);  // skip the first pointer
	SDL_WriteLE32(rw, m->offset_x);
	SDL_WriteLE32(rw, m->offset_y);
	SDL_RWwrite(rw, "SKIP", 4, 1);  // skip the second pointer
	SDL_WriteLE32(rw, m->start_x);
	SDL_WriteLE32(rw, m->start_y);
	if(SDL_RWtell(rw) != pos + 96) {
		SDL_RWclose(rw);
		return false;
	}

	// write map data
	if (SDL_RWwrite(rw, m->dat, sizeof(Tmappos), m->width * m->height) != (Sint64)m->width * m->height) {
		SDL_RWclose(rw);
		return false;
	}

	// close file
	SDL_RWclose(rw);
	
	return true;
}

#else

// Generic map loader using standard FILE* instead of SDL_RWops
Tmap *load_map_from_FILE(FILE *file) {
  printf("load_map_from_FILE\n");
  Tmap *m;
  char header[6];

  if (!file)
    return NULL;

  // Read header and validate
  if (fread(header, 6, 1, file) == 0) {
    fclose(file);
    return NULL;
  }

  if (header[0] != 'A' || header[1] != 'X' || header[2] != '4' ||
      header[3] != 'M' || header[4] != 'A' || header[5] != 'P') {
    fclose(file);
    return NULL;
  }

  for (int i = 0; i < 6; i++) {
    printf("%c", header[i]);
  }
  printf("\n");

  // Allocate memory for map struct
  m = malloc(sizeof(Tmap));
  if (!m) {
    fclose(file);
    return NULL;
  }

  // Read map structure
  fread(m, 64, 1, file); // Read first 64 bytes
  fread(&m->width, sizeof(int32_t), 1, file);
  fread(&m->height, sizeof(int32_t), 1, file);
  fseek(file, 4, SEEK_CUR); // Skip pointer
  fread(&m->offset_x, sizeof(int32_t), 1, file);
  fread(&m->offset_y, sizeof(int32_t), 1, file);
  fseek(file, 4, SEEK_CUR); // Skip pointer
  fread(&m->start_x, sizeof(int32_t), 1, file);
  fread(&m->start_y, sizeof(int32_t), 1, file);

  // printf("%d %d %d %d\n", m->width, m->height, m->start_x, m->start_y);

  // Ensure file position is correct
  // #ifndef PLATFORM_PLAYDATE
  // if (ftell(file) != 96) {
  // 	fclose(file);
  // 	free(m);
  // 	return NULL;
  // }
  // #endif

  // Allocate and read map data
  m->dat = malloc(m->width * m->height * sizeof(Tmappos));
  if (!m->dat) {
    fclose(file);
    free(m);
    return NULL;
  }

  // #ifndef PLATFORM_PLAYDATE
  // if (fread(m->dat, sizeof(Tmappos), m->width * m->height, file) !=
  // (size_t)(m->width * m->height)) { 	fclose(file); 	free(m->dat); 	free(m);
  // 	return NULL;
  // }
  // #else
  fread(m->dat, sizeof(Tmappos), m->width * m->height, file);
  // #endif

  fclose(file); // Ensure file is closed
  return m;
}

// Load map from a file on disk
Tmap *load_map(const char *fname) {
  char *basePath = platform_dirname(platform_executable_path());
  char tmpPath[1024];
  sprintf(tmpPath, "%s%s", basePath, fname);

  FILE *file = fopen(tmpPath, "rb");
  if (!file)
    return NULL;

  printf("load_map %s\n", tmpPath);

  Tmap *m = load_map_from_FILE(file);
  // fclose(file); // Ensure file is closed
  return m;
}

// Load map from memory (using a raw pointer instead of SDL_RWops)
Tmap *load_map_from_memory_data(const void *data, size_t size) {
  Tmap *m;
  const unsigned char *ptr = data;
  size_t offset = 0;

  // Validate header
  if (size < 6)
    return NULL;
  if (ptr[0] != 'A' || ptr[1] != 'X' || ptr[2] != '4' || ptr[3] != 'M' ||
      ptr[4] != 'A' || ptr[5] != 'P') {
    return NULL;
  }
  offset += 6;

  // Allocate memory for map struct
  m = malloc(sizeof(Tmap));
  if (!m)
    return NULL;

  // Read structure manually (since no FILE* is available)
  if (offset + 64 > size) {
    free(m);
    return NULL;
  }
  memcpy(m, ptr + offset, 64);
  offset += 64;

  // Read width and height
  if (offset + 8 > size) {
    free(m);
    return NULL;
  }
  m->width = *(int32_t *)(ptr + offset);
  m->height = *(int32_t *)(ptr + offset + 4);
  offset += 8;

  // Skip first pointer
  offset += 4;

  // Read offsets
  if (offset + 8 > size) {
    free(m);
    return NULL;
  }
  m->offset_x = *(int32_t *)(ptr + offset);
  m->offset_y = *(int32_t *)(ptr + offset + 4);
  offset += 8;

  // Skip second pointer
  offset += 4;

  // Read start positions
  if (offset + 8 > size) {
    free(m);
    return NULL;
  }
  m->start_x = *(int32_t *)(ptr + offset);
  m->start_y = *(int32_t *)(ptr + offset + 4);
  offset += 8;

  // Ensure we are at expected position
  // if (offset != 96) {
  // 	free(m);
  // 	return NULL;
  // }

  // Allocate and copy map data
  if (offset + (m->width * m->height * sizeof(Tmappos)) > size) {
    free(m);
    return NULL;
  }

  m->dat = malloc(m->width * m->height * sizeof(Tmappos));
  if (!m->dat) {
    free(m);
    return NULL;
  }

  memcpy(m->dat, ptr + offset, m->width * m->height * sizeof(Tmappos));

  return m;
}

Tmap *load_map_from_memory(int id) {
  printf("load_map_from_memory %d\n", id);
  return load_map_from_memory_data(maps[id].dat, maps[id].size);
}

Tmap *load_scriptmap_from_memory(int id) {
  printf("load_scriptmap_from_memory %d\n", id);
  return load_map_from_memory_data(scriptmaps[id].dat, scriptmaps[id].size);
}

int save_map(Tmap *m, char *fname) { return 0; }

#endif

// frees the memory of a map
void destroy_map(Tmap *m) {
  if (m == NULL)
    return;

  // free data
  if (m->dat != NULL)
    free(m->dat);

  // free the map itself
  free(m);
}

// draws a submap to a bitmap
void draw_map(BITMAP *bmp, Tmap *sm, int dx, int dy, int w, int h, int edit) {
  int x, y;
  int tox = sm->offset_x >> 4; // Tile Offset X
  int toy = sm->offset_y >> 4; // Tile Offset Y
  int tx, ty;
  int pos;
  int ax = (tox << 4) - sm->offset_x; // pixel adjustments in x
  int ay = (toy << 4) - sm->offset_y; // pixel adjustments in y

  if (sm == NULL)
    return;

  set_clip_rect(bmp, dx, dy, dx + w, dy + h);

  for (y = 0; y < 7; y++) {
    // for(x=0;x<11;x++) {
    for (x = 0; x < 14; x++) {
      tx = x + tox;
      ty = y + toy;
      if (tx < sm->width && ty < sm->height && tx >= 0 && ty >= 0) {
        pos = tx + ty * sm->width;

        if (sm->dat[pos].tile) {
          draw_tile(bmp, sm->dat[pos].tile - 1, dx + x * 16 + ax,
                    dy + y * 16 + ay);
        }

        if (sm->dat[pos].item) {
          if (sm->dat[pos].item == MAP_EGG) {
            draw_sprite(bmp, bitmaps[I_EGG], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          } else if (sm->dat[pos].item == MAP_1UP) {
            draw_sprite(bmp, bitmaps[I_ONEUP], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          } else if (sm->dat[pos].item == MAP_STAR) {
            draw_sprite(bmp, bitmaps[I_STAR], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          } else if (sm->dat[pos].item == MAP_HEART) {
            draw_sprite(bmp, bitmaps[I_HEART], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          } else if (sm->dat[pos].item == MAP_CHERRY) {
            draw_sprite(bmp, bitmaps[I_CHERRY], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          } else if (sm->dat[pos].item == MAP_LOLA) {
            draw_sprite(bmp, bitmaps[I_LOLA], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          }
        }

        if (sm->dat[pos].type) {
          if (sm->dat[pos].type == MAP_SPIN) {
            draw_sprite(bmp, bitmaps[I_SPIN1 + ((ABS(game_count) >> 2) % 4)],
                        dx + x * 16 + ax, dy + y * 16 + ay);
          }
          if (sm->dat[pos].type == MAP_WATER) {
            blit(bitmaps[I_WATER], bmp, ((ABS(game_count) >> 2) % 4) << 4, 0,
                 dx + x * 16 + ax, dy + y * 16 + ay, 16, 16);
          }
          if (sm->dat[pos].type == MAP_SURFACE) {
            masked_blit(bitmaps[I_WATER_SURFACE], bmp,
                        ((ABS(game_count) >> 2) % 8) << 4, 0, dx + x * 16 + ax,
                        dy + y * 16 + ay, 16, 16);
          }
        }

        if (edit) {
          if (sm->dat[pos].type == MAP_DEAD) {
            textout_ex(bmp, "D", dx + x * 16 + ax, dy + y * 16 + ay, 0, -1);
          }
          if (sm->dat[pos].type == MAP_EXIT) {
            textout_ex(bmp, "XT", dx + x * 16 + ax, dy + y * 16 + ay, 255, -1);
          }
          if (sm->dat[pos].type == MAP_BRK) {
            textout_ex(bmp, "GL", dx + x * 16 + ax, dy + y * 16 + ay, 255, -1);
          }
          if (sm->dat[pos].type == MAP_ENEMY1) {
            draw_sprite(bmp,
                        bitmaps[I_ENEMY1_01 + ((ABS(game_count) >> 3) % 4)],
                        dx + x * 16 + ax, dy + y * 16 + ay);
          }
          if (sm->dat[pos].type == MAP_ENEMY2) {
            draw_sprite(bmp,
                        bitmaps[I_ENEMY2_01 + ((ABS(game_count) >> 3) % 4)],
                        dx + x * 16 + ax, dy + y * 16 + ay - 8);
          }
          if (sm->dat[pos].type == MAP_ENEMY3) {
            draw_sprite(bmp, bitmaps[I_ENEMY3], dx + x * 16 + ax,
                        dy + y * 16 + ay - 96);
          }
          if (sm->dat[pos].type == MAP_ENEMY4) {
            draw_sprite(bmp, bitmaps[I_ENEMY4], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          }
          if (sm->dat[pos].type == MAP_ENEMY5) {
            draw_sprite(bmp, bitmaps[I_ENEMY5_01], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          }
          if (sm->dat[pos].type == MAP_ENEMY6) {
            draw_sprite(bmp, bitmaps[I_ENEMY6], dx + x * 16 + ax,
                        dy + y * 16 + ay);
          }
          if (sm->dat[pos].type == MAP_GUARD1) {
            draw_sprite(bmp, bitmaps[I_GUARD1_1], dx + x * 16 + ax,
                        dy + y * 16 + ay - 16);
          }
          if (sm->dat[pos].type == MAP_GUARD2) {
            draw_sprite(bmp, bitmaps[I_GUARD2_1A], dx + x * 16 + ax,
                        dy + y * 16 + ay - 32);
          }
        }
#ifdef SHOW_TILES
        if (sm->dat[pos].tile)
          textprintf_centre_ex(bmp, dx + x * 16 + ax + 8, dy + y * 16 + ay + 4,
                               5, -1, "%d", sm->dat[pos].tile);
#endif
      }
    }
  }

  reset_clip_rect(bmp);
}

// gets a pointer to the mappos at tx, ty
Tmappos *get_mappos(Tmap *m, int tx, int ty) {
  if (m == NULL)
    return NULL;
  if (tx < 0 || tx >= m->width)
    return NULL;
  if (ty < 0 || ty >= m->height)
    return NULL;

  return &m->dat[tx + ty * m->width];
}

// copies a part of a map to another part
void copy_map(Tmap *src, Tmap *dst, int sx, int sy, int dx, int dy, int w,
              int h) {
  int x, y;
  for (x = 0; x < w; x++)
    for (y = 0; y < h; y++)
      dst->dat[dx + x + (dy + y) * dst->width] =
          src->dat[sx + x + (sy + y) * src->width];
}

// changes the size of the sub map according to dw and dh
// dir_flags hold on which sides to shrink and/or expand
void change_map_size(Tmap *sm, int dw, int dh, int dir_flags) {
  Tmap tsm;
  int i;

  if (sm == NULL)
    return;
  if (!dir_flags)
    return;

  if (dh) {
    // save old map and create new
    tsm.dat = sm->dat;
    tsm.width = sm->width;
    tsm.height = sm->height;
    sm->height += dh;
    sm->dat = malloc(sm->width * sm->height * sizeof(Tmappos));
    for (i = 0; i < sm->width * sm->height; i++)
      sm->dat[i].tile = sm->dat[i].mask = sm->dat[i].type = sm->dat[i].item = 0;

    // copy relevant parts
    if (dh > 0) {
      if (dir_flags & SM_UP)
        copy_map(&tsm, sm, 0, 0, 0, dh, tsm.width, tsm.height);
      else if (dir_flags & SM_DOWN)
        copy_map(&tsm, sm, 0, 0, 0, 0, tsm.width, tsm.height);
    } else if (dh < 0) {
      if (dir_flags & SM_UP)
        copy_map(&tsm, sm, 0, -dh, 0, 0, tsm.width, tsm.height + dh);
      else if (dir_flags & SM_DOWN)
        copy_map(&tsm, sm, 0, 0, 0, 0, tsm.width, tsm.height + dh);
    }

    free(tsm.dat);
  }

  if (dw) {
    // save old map and create new
    tsm.dat = sm->dat;
    tsm.width = sm->width;
    tsm.height = sm->height;
    sm->width += dw;
    sm->dat = malloc(sm->width * sm->height * sizeof(Tmappos));
    for (i = 0; i < sm->width * sm->height; i++)
      sm->dat[i].tile = sm->dat[i].mask = sm->dat[i].type = sm->dat[i].item = 0;

    // copy relevant parts
    if (dw > 0) {
      if (dir_flags & SM_LEFT)
        copy_map(&tsm, sm, 0, 0, dw, 0, tsm.width, tsm.height);
      else if (dir_flags & SM_RIGHT)
        copy_map(&tsm, sm, 0, 0, 0, 0, tsm.width, tsm.height);
    } else if (dw < 0) {
      if (dir_flags & SM_LEFT)
        copy_map(&tsm, sm, -dw, 0, 0, 0, tsm.width + dw, tsm.height);
      else if (dir_flags & SM_RIGHT)
        copy_map(&tsm, sm, 0, 0, 0, 0, tsm.width + dw, tsm.height);
    }

    free(tsm.dat);
  }
}

// returns the mask id if there is ground at a,y
// can handel masks [0, 8]
int is_ground(Tmap *sm, int x, int y) {
  int tile_x = x >> 4;
  int tile_y = y >> 4;
  int ox, oy;
  int pos, mask;

  // return FALSE if outside map
  if (tile_x < 0 || tile_x >= sm->width)
    return 1;
  if (tile_y < 0 || tile_y >= sm->height)
    return 0;

  // cals position and mask
  pos = tile_x + tile_y * sm->width;
  mask = sm->dat[pos].mask;

  // the simple ones
  if (mask < 2)
    return mask; // 0 = no mask, 1 = full mask

  // calculate offset from top left corner of tile
  ox = x - (tile_x << 5);
  oy = y - (tile_y << 5);

  // the semi-simple ones
  if (mask == 2) { // half full block
    if (oy < 16)
      return 0;
    else
      return mask;
  }

  if (mask == 5 && oy > 31 - ox)
    return mask; // 45 degree slope /     (med)
  if (mask == 6 && oy > ox)
    return mask; // 45 degree slope \ (med)

  // the not so simple ones
  if (mask == 3 && oy > 31 - ox / 2)
    return mask; // 22 degree slope / (low)
  if (mask == 4 && oy > 15 - ox / 2)
    return mask; // 22 degree slope / (hi)
  if (mask == 7 && oy > ox / 2)
    return mask; // 22 degree slope \ (hi)
  if (mask == 8 && oy > 16 + ox / 2)
    return mask; // 22 degree slope \ (low)

  // return FALSE for all other cases
  return 0;
}

// returns delta to new y coord
int adjust_ypos(Tmap *sm, int x, int y, int ground, int dy) {
  int oy = y;
  if (ground)
    while (is_ground(sm, x, y))
      y += dy;
  else
    while (!is_ground(sm, x, y))
      y += dy;
  return y - oy;
}

// returns delta to new x coord
int adjust_xpos(Tmap *sm, int x, int y, int ground, int dx) {
  int ox = x;
  if (ground)
    while (is_ground(sm, x, y))
      x += dx;
  else
    while (!is_ground(sm, x, y))
      x += dx;
  return x - ox;
}

// sets the tile id
// returns 1 if all went well
int set_tile(Tmap *m, int tx, int ty, int tile) {
  if (tx < 0 || ty < 0)
    return 0;
  if (tx >= m->width || ty >= m->height)
    return 0;
  m->dat[tx + ty * m->width].tile = tile;
  return 1;
}

// returns the tile id
int get_tile(Tmap *m, int tx, int ty) {
  if (tx < 0 || ty < 0)
    return 0;
  if (tx >= m->width || ty >= m->height)
    return 0;
  return m->dat[tx + ty * m->width].tile;
}
