#ifndef PLAYDATE_PORT_H
#define PLAYDATE_PORT_H

#include "options.h"
#include <stdint.h>

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

// types
typedef struct {
  void *tex;
  int w;
  int h;
} BITMAP;

// key
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_LCONTROL 5
#define KEY_ALT 6
#define KEY_ESC 7
#define KEY_ENTER 8
#define KEY_SPACE 9
#define KEY_BACKSPACE 10
#define KEY_1 11
#define KEY_2 12
#define KEY_3 13
#define KEY_4 14
#define KEY_F1 15
#define KEY_F12 16
#define KEY_DEL 17
#define KEY_INSERT 18

typedef uint32_t Uint32;
typedef uint8_t Uint8;

void update_platform_controls();
extern Uint8 key[256];

// init
void init_platform();
void make_window(Toptions *o);
void uninit_platform();

#ifdef PLATFORM_PLAYDATE

#include "pd_api.h"
extern PlaydateAPI *playdate;

#define fopen(name, mode)                                                      \
  (playdate->file->open(name, (mode[0] == 'w' ? kFileWrite : kFileRead)))
#define fclose(File) playdate->file->close(File)
#define fread(buf, len, cz, File) playdate->file->read(File, buf, (len * cz))
#define fwrite(buf, len, cz, File) playdate->file->write(File, buf, (len * cz))
#define ftell(File) (playdate->file->tell(File))
#define fseek(File, pos, whence) playdate->file->seek(File, pos, whence)
#define fgets(buf, len, File) _readLineFileFunction(File, buf, len)
#endif

char *_readLineFileFunction(void *fp, void *buf, unsigned int len);

#define SOUND_FREQ 64

#endif // PLAYDATE_PORT_H