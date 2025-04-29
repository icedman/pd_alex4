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

// the generated header
#define INSIDE_DATA
#include "data_files.h"
#undef INSIDE_DATA

#include "data.h"
#include "miniz.h"
#include "port.h"
#include <stdbool.h>

// data files
BITMAP **bitmaps;
BITMAP **bitmaps_mono;
BITMAP **bitmaps_color;
DATAFILE *fonts;
DATAFILE *maps;
DATAFILE *scripts;
DATAFILE *scriptmaps;
DATAFILE *sounds;
DATAFILE *musics;

DATAFILE _scripts[SCR_MAX];

uint8_t *platform_load_asset(const char *name, uint32_t *bytes_read);

bool load_scripts() {
  scripts = _scripts;

  uint32_t sz;
  _scripts[0].dat = (void *)platform_load_asset("scripts/intro.txt", &sz);
  _scripts[0].size = sz;

  _scripts[1].dat = (void *)platform_load_asset("scripts/outro.txt", &sz);
  _scripts[1].size = sz;

  // printf("%d\n", scripts[0].size);

  return true;
}

bool extract(mz_zip_archive *z, const char *file, void **ptr, size_t *size) {
  *ptr = mz_zip_reader_extract_file_to_heap(z, file, size, 0);
  if (!ptr) {
    printf("Failed to extract file \"%s\"!\n", file);
    size = 0;
    return false;
  }
  return true;
}

bool load_datafile(const char *filename) {
  mz_zip_archive zip_archive;
  mz_zip_zero_struct(&zip_archive);

  mz_bool status = mz_zip_reader_init_file(&zip_archive, filename, 0);
  if (!status) {
    printf("Could not initialize ZIP reader!\n");
    return false;
  }

// images
#define extract_bmps(type, mapping)                                            \
  type = calloc(I_MAX, sizeof(BITMAP *));                                      \
  for (int i = 0; i < I_MAX; i++) {                                            \
    size_t size;                                                               \
    void *data;                                                                \
    if (extract(&zip_archive, mapping[i], &data, &size)) {                     \
      type[i] = load_bmp_from_mem(data, size);                                 \
      mz_free(data);                                                           \
      if (!type[i]) {                                                          \
        printf("Failed to load bmp: %s\n", mapping[i]);                        \
        mz_zip_reader_end(&zip_archive);                                       \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
  }

  extract_bmps(bitmaps_mono, _image_mono_mapping)
      extract_bmps(bitmaps_color, _image_color_mapping)

#undef extract_bmps

          bitmaps = bitmaps_mono; // default

#define extract_data(type, max, mapping)                                       \
  type = calloc(max, sizeof(DATAFILE));                                        \
  for (int i = 0; i < max; i++) {                                              \
    if (!extract(&zip_archive, mapping[i], &type[i].dat, &type[i].size)) {     \
      mz_zip_reader_end(&zip_archive);                                         \
      return false;                                                            \
    }                                                                          \
  }

  // sounds
  extract_data(sounds, S_MAX, _sound_mapping)
      // maps
      extract_data(maps, MAP_MAX, _map_mapping)
      // fonts
      extract_data(fonts, F_MAX, _font_mapping)
      // music
      extract_data(musics, MSC_MAX, _music_mapping)

#undef extract_data

      // scripts and scriptmaps
      scripts = calloc(SCR_MAX, sizeof(DATAFILE));
  scriptmaps = calloc(SCR_MAX, sizeof(DATAFILE));
  for (int i = 0; i < SCR_MAX; i++) {
    const char *file = i ? "outro" : "intro";
    char script_path[32];

    sprintf(script_path, "scripts/%s.txt", file);
    if (!extract(&zip_archive, script_path, &scripts[i].dat,
                 &scripts[i].size)) {
      mz_zip_reader_end(&zip_archive);
      return false;
    }
    sprintf(script_path, "scripts/%s.map", file);
    if (!extract(&zip_archive, script_path, &scriptmaps[i].dat,
                 &scriptmaps[i].size)) {
      mz_zip_reader_end(&zip_archive);
      return false;
    }
  }

  mz_zip_reader_end(&zip_archive);
  return true;
}

void unload_data() {
// images
#define free_bmps(type)                                                        \
  if (type) {                                                                  \
    for (int i = 0; i < I_MAX; i++) {                                          \
      if (type[i])                                                             \
        destroy_bitmap(type[i]);                                               \
    }                                                                          \
  }                                                                            \
  free(type);

  free_bmps(bitmaps_mono) free_bmps(bitmaps_color)

#undef free_bmps

#define free_data(type, max)                                                   \
  if (type) {                                                                  \
    for (int i = 0; i < max; i++) {                                            \
      mz_free(type[i].dat);                                                    \
    }                                                                          \
  }                                                                            \
  free(type);

      // sounds
      free_data(sounds, S_MAX)
      // maps
      free_data(maps, MAP_MAX)
      // fonts
      free_data(fonts, F_MAX)
      // music
      free_data(musics, MSC_MAX)

#undef free_data

      // scripts and scriptmaps
      for (int i = 0; i < SCR_MAX; i++) {
    if (scripts)
      mz_free(scripts[i].dat);
    if (scriptmaps)
      mz_free(scriptmaps[i].dat);
  }
  free(scripts);
  free(scriptmaps);
}

void switch_bitmaps(bool colorize) {
  if (colorize) {
    bitmaps = bitmaps_color;
  } else {
    bitmaps = bitmaps_mono;
  }
}
