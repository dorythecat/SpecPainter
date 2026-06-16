#include "png.h"

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL || ferror(fp)) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    return;
  }
  unsigned int i = 1;
  unsigned int limit = 65536; // 2^16
  unsigned char *values = malloc(sizeof *values * limit);
  while (1) {
    if (feof(fp)) break;
    char c = fgetc(fp);
    values[i] = (unsigned char)c;
    if (i == limit) { // If we don't have enough space, resize the array to fit all of our data
      unsigned int counter = limit;
      limit *= 2;
      unsigned char *temp = malloc(sizeof *temp * limit);
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
  unsigned char *temp = malloc(sizeof *temp * (i - 1));
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
  char *name = malloc(sizeof *name * 4);
  unsigned char *data = malloc(sizeof *data * 13);
  unsigned int size;
  unsigned int index = 8;
  index = chunk_read(values, i, index, name, data, 13, &size);
  if (index >= i) {
    printf("Error encountered when reading chunk!\n");
    return;
  }
  printf("Final index: %d\n", index);
  if (
      name[0] != 73 ||
      name[1] != 72 ||
      name[2] != 68 ||
      name[3] != 82
  ) {
    printf("Invalid first chunk!\n");
    return;
  }

  free(values);
}

unsigned int chunk_read(unsigned char *values, unsigned int values_size, unsigned int index, char *name, unsigned char *data, unsigned int max_size, unsigned int *data_size) {
  *data_size = ((values[index] * 256 + values[index + 1]) * 256 + values[index + 2]) * 256 + values[index + 3];
  index += 4;
  if (*data_size > max_size) {
    printf("Data size exceeds expected maximum for this chunk!\n");
    return values_size;
  }
  for (unsigned int i = 0; i < 4; i++) name[i] = values[index++];
  // These values aren't really used but it's nice having them, nonetheless
  //char critical = ((unsigned char)name[0] & 32) != 32;
  //char private  = ((unsigned char)name[1] & 32) == 32;
  if (((unsigned char)name[2] & 32) == 32) {
    printf("Image does not conform to PNGv3 standard!\n");
    return values_size;
  }
  //char safecopy = ((unsigned char)name[3] & 32) == 32;

  for (unsigned int i = 0; i < *data_size; i++) data[i] = values[index++];
  index += 4; // TODO: Actually check CRC

  return index;
}
