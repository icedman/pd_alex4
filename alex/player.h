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

#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"

// struct for the player
typedef struct {
  Tactor *actor;

  int score;
  int jumping;
  int jump_pressed;
  int eat_pressed;
  int eat_counter;

  int angle;

  int ammo;
  int lives;
  int health;
  int wounded;

  int dy;

  // pick up related
  int cherries;
  int stars;
  int cherries_taken;
  int stars_taken;
} Tplayer;

// the player
extern Tplayer player;

// functions
void draw_player(BITMAP *bmp, Tplayer *p, int x, int y);
void wound_player(Tplayer *p);
void kill_player(Tplayer *p);

#endif
