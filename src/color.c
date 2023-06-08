#include <stdio.h>

#include "color.h"

void color_bold(void) {
    printf("\033[1m");
}

void color_red(void) {
  printf("\033[31m");
}

void color_yellow(void) {
  printf("\033[33m");
}

void color_blue(void) {
  printf("\033[34m");
}

void color_reset(void) {
  printf("\033[0m");
}
