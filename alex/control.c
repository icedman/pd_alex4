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

#include "control.h"
#include "port.h"

// "constructor"
// sets default values
void init_control(Tcontrol *c) {
  set_control(c, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_LCONTROL, KEY_ALT);

  c->flags = 0;
  c->use_joy = 0;
}

// sets the desired keys for the control unit
void set_control(Tcontrol *c, int up, int down, int left, int right, int fire,
                 int jump) {
  c->key_up = up;
  c->key_down = down;
  c->key_left = left;
  c->key_right = right;
  c->key_fire = fire;
  c->key_jump = jump;
}

// updates the control unit
void poll_control(Tcontrol *c) {
  c->flags = 0;

#if 0
	// TODO
	if (c->use_joy) {
		poll_joystick();
		
		if (joy[0].stick[0].axis[1].d1)	c->flags |= K_UP;
		if (joy[0].stick[0].axis[1].d2)	c->flags |= K_DOWN;
		if (joy[0].stick[0].axis[0].d1)	c->flags |= K_LEFT;
		if (joy[0].stick[0].axis[0].d2)	c->flags |= K_RIGHT;
		if (joy[0].button[1].b)	c->flags |= K_FIRE;
		if (joy[0].button[0].b)	c->flags |= K_JUMP;
	}
#endif

  update_platform_controls();

  if (key[c->key_up])
    c->flags |= K_UP;
  if (key[c->key_down])
    c->flags |= K_DOWN;
  if (key[c->key_left])
    c->flags |= K_LEFT;
  if (key[c->key_right])
    c->flags |= K_RIGHT;
  if (key[c->key_fire])
    c->flags |= K_FIRE;
  if (key[c->key_jump])
    c->flags |= K_JUMP;
}

// check if a key in the control is available
int check_control_key(Tcontrol *c, int key) {
  if (key == c->key_left)
    return TRUE;
  if (key == c->key_right)
    return TRUE;
  if (key == c->key_up)
    return TRUE;
  if (key == c->key_down)
    return TRUE;
  if (key == c->key_fire)
    return TRUE;
  if (key == c->key_jump)
    return TRUE;
  return FALSE;
}

// returns true or false depending on if keys are pressed or not
int is_up(Tcontrol *c) { return (c->flags & K_UP ? TRUE : FALSE); }
int is_down(Tcontrol *c) { return (c->flags & K_DOWN ? TRUE : FALSE); }
int is_left(Tcontrol *c) { return (c->flags & K_LEFT ? TRUE : FALSE); }
int is_right(Tcontrol *c) { return (c->flags & K_RIGHT ? TRUE : FALSE); }
int is_fire(Tcontrol *c) { return (c->flags & K_FIRE ? TRUE : FALSE); }
int is_jump(Tcontrol *c) { return (c->flags & K_JUMP ? TRUE : FALSE); }
int is_any(Tcontrol *c) { return (c->flags ? TRUE : FALSE); }

// saves the control config to disk using FP
void save_control(Tcontrol *c, FILE *fp) {
  printf("unused save_control\n");
#if 0
	pack_iputl(c->use_joy, fp);
	pack_iputl(c->key_left, fp);
	pack_iputl(c->key_right, fp);
	pack_iputl(c->key_up, fp);
	pack_iputl(c->key_down, fp);
	pack_iputl(c->key_fire, fp);
	pack_iputl(c->key_jump, fp);
	pack_putc(c->flags, fp);
#endif
}

// loads the control config from disk using FP
void load_control(Tcontrol *c, FILE *fp) {
  printf("unused load_control\n");
#if 0
	c->use_joy = pack_igetl(fp);
	c->key_left = pack_igetl(fp);
	c->key_right = pack_igetl(fp);
	c->key_up = pack_igetl(fp);
	c->key_down = pack_igetl(fp);
	c->key_fire = pack_igetl(fp);
	c->key_jump = pack_igetl(fp);
	c->flags = pack_getc(fp);
#endif
}
