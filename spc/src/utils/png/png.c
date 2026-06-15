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

  printf("Image size: %d\n", i);
  if (
      values[0] != 137 ||
      values[1] != 80  ||
      values[2] != 78  ||
      values[3] != 71  ||
      values[4] != 13  ||
      values[5] != 10  ||
      values[6] != 26  ||
      values[7] != 10
  ) {
    printf("Invalid file signature!\n");
    return;
  }
  char *name;
  chunk_read(values, i, 8, name);
  free(values);
}

void chunk_read(unsigned int *values, unsigned int size, unsigned int chunk_index, char *name) {
  unsigned int data_length = 0;
  for (unsigned int i = 0; i < 4; i++) {
    data_length *= 255;
    data_length += values[chunk_index++];
  }
  printf("Chunk data length: %d\n", data_length);
  name = malloc(sizeof *name * 4);
  for (unsigned int i = 0; i < 4; i++) name[i] = values[chunk_index++];
  char critical = ((unsigned char)name[0] & 32) != 32;
  char private  = ((unsigned char)name[1] & 32) == 32;
  if (((unsigned char)name[2] & 32) == 32) {
    printf("PNG image cannot be read (it does not conform to PNGv3 standard)!\n");
    return;
  }
  char safecopy = ((unsigned char)name[3] & 32) == 32;
  printf("Critical chunk: %d\n", critical);
  printf("Private chunk: %d\n", private);
  printf("Safe to copy chunk: %d\n", safecopy);
}
