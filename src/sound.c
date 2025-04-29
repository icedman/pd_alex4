#include "options.h"
#include "port.h"

#include "data_files.h"

static sound_source_t *sound_sources[S_MAX];
static sound_t music_sources[MSC_MAX];

extern const char *_sound_mapping[S_MAX];
extern const char *_music_mapping[MSC_MAX];

void stop_sound_id(int id);

bool init_sound(Toptions *o) {
  for (int i = 0; i < S_MAX; i++) {
    char *src = _sound_mapping[i];
    char tmpQoa[1024];
    strcpy(tmpQoa, src);
    strcpy(tmpQoa + (strlen(tmpQoa) - 3), "qoa");
    // printf("./qoaconv assets/%s ./build/%s\n", src, tmpQoa);

    sound_sources[i] = sound_source(tmpQoa);
    // stop_sound_id(i);
  }

  for (int i = 0; i < MSC_MAX; i++) {
    char *src = _music_mapping[i];
    char tmpQoa[1024];
    strcpy(tmpQoa, src);
    strcpy(tmpQoa + (strlen(tmpQoa) - 3), "qoa");
    // printf("./qoaconv assets/%s ./build/%s\n", src, tmpQoa);

    music_sources[i] = sound(sound_source(tmpQoa));
  }
  return false;
}

void uninit_sound() {}

void ResetChan(int channel) {}

void load_sfx() {}

void load_music(int id) {
  sound_t *music = &music_sources[id];
  if (music) {
    sound_set_loop(*music, true);
  }
}

void unload_music() {}

void free_sfx() {}

void set_sound_volume(int vol) {}

void set_music_volume(int vol) {}

// plays a sample using default settings (from an index)
void play_sound_id(int id) {
  sound_source_t *sfx = sound_sources[id];
  if (sfx) {
    sound_play(sfx);
  }
}

// plays a sample using user settings (from an index)
void play_sound_id_ex(int id, int vol, int freq, int loop) {
  sound_source_t *sfx = sound_sources[id];
  if (sfx) {
    sound_play(sfx);
  }
}

// stops a sample (providing an id)
void stop_sound_id(int id) {
  sound_source_t *sfx = sound_sources[id];
  if (sfx) {
    sound_stop(sound(sfx));
  }
}

// adjusts a sample (from an index) according to player position
void adjust_sound_id_ex(int id, int player_x, int x) {}

// stops any mod playing
void stop_music(void) {}

// starts the mod at position x
void start_music(int startorder) {}

void pause_music(bool p) {}