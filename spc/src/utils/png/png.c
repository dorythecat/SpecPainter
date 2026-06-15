#include "png.h"

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    return;
  }
  char c;
  while ((c = fgetc(fp)) != EOF) {
    printf("%d\n", (unsigned int)(unsigned char)c);
  }
  fclose(fp);
}
