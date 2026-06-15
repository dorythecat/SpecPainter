#include "png.h"

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL || ferror(fp)) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    return;
  }
  unsigned int i = 1;
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
  unsigned int *temp = malloc(sizeof *temp * (i - 1));
  while (counter != 0) {
    temp[counter] = values[counter];
    counter--;
  } counter++;
  free(values);
  values = malloc(sizeof *temp * (i - 2));
  while (counter < i - 1) {
    values[counter - 1] = temp[counter];
    counter++;
  } i -= 3;
  free(temp);

  printf("%d\n", i);
  printf("%d\n", values[0]);
  printf("%d\n", values[1]);
  printf("%d\n", values[2]);
  printf("%d\n", values[3]);
  printf("%d\n", values[4]);
  printf("%d\n", values[5]);
  printf("%d\n", values[6]);
  printf("%d\n", values[7]);
  printf("%d\n", values[i]);
  free(values);
}
