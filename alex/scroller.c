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

#include "scroller.h"
#include "port.h"
#include <stdio.h>
#include <string.h>

// initiates a scroller
// the input text (t) will be messed up if horiz = 0 (vertical scroller)
void init_scroller(Tscroller *sc, char *t, int w, int h, int horiz) {
  sc->font_height = text_height();
  sc->height = h;
  sc->horizontal = horiz;
  sc->text = t;
  sc->width = w;
  if (sc->horizontal) {
    sc->length = text_length(sc->text);
    sc->offset = sc->width;
  } else {
    int i;
    int len = strlen(sc->text);

    sc->lines[0] = sc->text;
    sc->rows = 1;
    for (i = 0; i < len; i++)
      if (sc->text[i] == '\n' && sc->rows < MAX_LINES_IN_SCROLLER) {
        sc->lines[sc->rows] = &sc->text[i + 1];
        sc->rows++;
        sc->text[i] = '\0';
      }

    sc->offset = sc->height;
  }
}

// draws the scroller
void draw_scroller(Tscroller *sc, BITMAP *bmp, int x, int y) {
  if (sc->horizontal) {
    if (sc->offset < -sc->length)
      return;
    if (sc->offset > sc->width)
      return;
    // set_clip_rect(bmp, x, y, x + sc->width, y + sc->height);
    textout_ex(bmp, sc->text, x + sc->offset + 1, y + 1, 1, -1);
    textout_ex(bmp, sc->text, x + sc->offset, y, 4, -1);
  } else {
    int i;
    if (sc->offset < -sc->rows * sc->font_height)
      return;
    if (sc->offset > sc->height)
      return;
    // set_clip_rect(bmp, x, y, x + sc->width, y + sc->height);
    for (i = 0; i < sc->rows; i++) {
      if (i * sc->font_height + sc->offset <= sc->height)
        if ((i + 1) * sc->font_height + sc->offset >= 0)
          textout_centre_ex(bmp, sc->lines[i], x + (sc->width >> 1),
                            i * sc->font_height + y + sc->offset, -1, -1);
    }
  }
  // reset_clip_rect(bmp);
}

// scrolls the text #step steps
void scroll_scroller(Tscroller *sc, int step) { sc->offset += step; }

// returns TRUE if the scroller is visible in it's windows
int scroller_is_visible(Tscroller *sc) {
  if (sc->horizontal) {
    if (sc->offset < -sc->length)
      return FALSE;
    if (sc->offset > sc->width)
      return FALSE;
  } else {
    if (sc->offset < -sc->rows * sc->font_height)
      return FALSE;
    if (sc->offset > sc->height)
      return FALSE;
  }

  return TRUE;
}

// restarts the offset
void restart_scroller(Tscroller *sc) {
  if (sc->horizontal)
    sc->offset = sc->width;
  else
    sc->offset = sc->height;
}
