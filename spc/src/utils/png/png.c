#include "png.h"

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    return;
  }
  unsigned int i;
  unsigned int limit = 65536; // 2^16
  unsigned int *values = malloc(sizeof *values * limit);
  while (1) {
    if (feof(fp)) break;
    char c = fgetc(fp);
    values[i] = (unsigned char)c;
    if (i == limit) { // If we don't have enough space, resize the array to fit all of our data
      unsigned int counter = limit;
      limit *= 2;
      unsigned int *temp = malloc(sizeof *temp * limit);
      while (counter != 0) {
        temp[counter] = values[counter];
        counter--;
      }
      free(values);
      values = malloc(sizeof *values * limit);
      while (counter < limit / 2) {
        values[counter] = temp[counter];
        counter++;
      }
      free(temp);
    } i++;
  } fclose(fp);

  // Resize our array to be EXACTLY as much space as we need
  // To be a bit more efficient we can just use temp going forwards tbh
  unsigned int counter = i;
  unsigned int *temp = malloc(sizeof *temp * i);
  while (counter != 0) {
    temp[counter] = values[counter];
    counter--;
  }
  free(values);
  values = malloc(sizeof *temp * i);
  while (counter < limit / 2) {
    values[counter] = temp[counter];
    counter++;
  }
  free(temp);

  printf("%d\n", i);
  free(values);
}
