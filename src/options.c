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

#include "options.h"
#include "iniparser.h"
#include "port.h"
#include <stdio.h>

// helpers

// saves the data structure to disk
void save_options(Toptions *o, const char *filename) {
  char *basePath = platform_dirname(platform_executable_path());
  char tmpPath[1024];
  sprintf(tmpPath, "%s%s", basePath, filename);

  FILE *file = fopen(tmpPath, "w");
  char line[1024];
  sprintf(line, "max_levels = %d\n", o->max_levels);
  fwrite(line, sizeof(char), strlen(line), file);
  for (int i = 0; i < o->max_levels; i++) {
    sprintf(line, "level%02d_cherries = %d\n", i, o->cherries[i]);
    fwrite(line, sizeof(char), strlen(line), file);
    sprintf(line, "level%02d_stars = %d\n", i, o->stars[i]);
    fwrite(line, sizeof(char), strlen(line), file);
  }
  fclose(file);
}

// loads the data structure from disk
bool load_options(Toptions *o, const char *filename) {
  char *basePath = platform_dirname(platform_executable_path());
  char tmpPath[1024];
  sprintf(tmpPath, "%s%s", basePath, filename);

  memset(o, 0, sizeof(Toptions));

  FILE *file = fopen(tmpPath, "r");
  if (!file) return false;
  
  char buf[1024];
  while (fgets(buf, 1024, file)) {
    char *token = strtok(buf, "=");
    if (token != NULL) {
      char name[32];
      strcpy(name, token);
      float fp[] = {0, 0, 0, 0, 0, 0, 0, 0};
      int fidx = 0;
      token = strtok(NULL, " ");
      while (token != NULL) {
        float number;
        if (sscanf(token, "%f", &number) == 1) {
          fp[fidx++] = number;
        } else {
          printf("Error parsing float: %s\n", token);
        }
        token = strtok(NULL, " ");
      }

      // if (strcmp("width", name) == 0) {
      //   o->width = fp[0];
      // }
      // if (strcmp("height", name) == 0) {
      //   o->height = fp[0];
      // }

      if (strstr(name, "max_levels") == name) {
        o->max_levels = fp[0];
      }
      if (strstr(name, "level") == name) {
        char num[] = {name[5], name[6], 0};
        int n = atoi(num);
        if (strstr(name, "cherries") != NULL) {
          o->cherries[n] = (int)fp[0];
        }
        if (strstr(name, "stars") != NULL) {
          o->stars[n] = (int)fp[0];
        }
      }
    }
  }

  fclose(file);
  return true;
}

// resets all data
void reset_options(Toptions *o) {}
