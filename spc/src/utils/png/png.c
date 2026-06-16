#include "png.h"

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL || ferror(fp)) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    fclose(fp);
    return;
  }
  unsigned int i = 0;
  unsigned int limit = 65536; // 2^16
  unsigned char *values = malloc(sizeof *values * limit);
  if (values == NULL) {
    printf("Error encountered when allocating memory for image data!\n");
    fclose(fp);
    return;
  }
  while (1) {
    if (feof(fp)) break;
    values[i] = (unsigned char)fgetc(fp);
    if (i == limit) { // If we don't have enough space, resize the array to fit all of our data
      limit *= 2;
      unsigned char *temp = realloc(values, sizeof *values * limit);
      if (temp == NULL) {
        printf("Error encountered when growing memory for image data!\n");
        free(values);
        fclose(fp);
        return;
      } values = temp;
    } i++;
  } fclose(fp);
  i -= 2;

  // Resize our array to be EXACTLY as much space as we need
  // To be a bit more efficient we can just use temp going forwards tbh
  unsigned char *temp = realloc(values, sizeof *values * i);
  if (temp == NULL) {
    printf("Error encountered when shrinking memory for image data!\n");
    free(values);
    return;
  } values = temp;

  printf("Image size: %d\n", i);
  if (values[0] != 137 ||
      values[1] != 80  ||
      values[2] != 78  ||
      values[3] != 71  ||
      values[4] != 13  ||
      values[5] != 10  ||
      values[6] != 26  ||
      values[7] != 10) {
    printf("Invalid file signature!\n");
    free(values);
    return;
  }

  // Load IHDR chunk
  char *name = malloc(sizeof *name * 4);
  if (name == NULL) {
    printf("Error encountered when allocating memory for chunk name!\n");
    free(values);
    return;
  }
  unsigned char *data = malloc(sizeof *data * 13);
  if (data == NULL) {
    printf("Error encountered when allocating memory for chunk data!\n");
    free(values);
    free(name);
    return;
  }
  unsigned int size = 0;
  unsigned int index = chunk_read(values, i, 8, name, data, 13, &size);
  if (name[0] != 73 ||
      name[1] != 72 ||
      name[2] != 68 ||
      name[3] != 82 ||
      size != 13) {
    printf("Invalid first chunk!\n");
    free(name);
    free(data);
    free(values);
    return;
  } free(name);

  unsigned int width = ((data[0] * 256 + data[1]) * 256 + data[2]) * 256 + data[3];
  unsigned int height = ((data[4] * 256 + data[5]) * 256 + data[6]) * 256 + data[7];
  char bit_depth = data[8];
  char color_type = data[9];
  char compression = data[10];
  char filter = data[11];
  char interlace = data[12];
  free(data);

  if (color_type != 0 && color_type != 2 && color_type != 3 && color_type != 4 && color_type != 6) {
    printf("Image color type is unsupported!\n");
    free(values);
    return;
  }

  if (
      (color_type == 0 && bit_depth != 1 && bit_depth != 2 && bit_depth != 4 && bit_depth != 8 && bit_depth != 16) ||
      ((color_type == 2 || color_type == 4 || color_type == 6) && bit_depth != 8 && bit_depth != 16) ||
      (color_type == 3 && bit_depth != 1 && bit_depth != 2 && bit_depth != 4 && bit_depth != 8)
  ) {
    printf("Image bit depth is unsupported!\n");
    free(values);
    return;
  }

  if (compression != 0) {
    printf("Image compression method is unsupported!\n");
    free(values);
    return;
  }

  if (filter != 0) {
    printf("Image filter method is unsupported!\n");
    free(values);
    return;
  }

  if (interlace != 0 && interlace != 1) {
    printf("Image interlace method is unsupported!\n");
    free(values);
    return;
  }

  printf("Image width: %d\n", width);
  printf("Image height: %d\n", height);

  // Load all of the following chunks, ignore unknown chunk formats and handle all known cases
  while (index < i) {
    char *name = malloc(sizeof *name * 4);
    if (name == NULL) {
      printf("Error encountered when allocating memory for chunk name!\n");
      free(values);
      return;
    }
    unsigned char *data = malloc(sizeof *data * 2147483647); // Support up to the maximum size possible
    if (data == NULL) {
      printf("Error encountered when allocating memory for chunk data!\n");
      free(values);
      free(name);
      return;
    }
    index = chunk_read(values, i, index, name, data, 2147483647, &size);
    printf("Chunk name: %s\n", name);
    free(name);
    free(data);
  }

  free(values);
}

unsigned int chunk_read(unsigned char *values, unsigned int values_size, unsigned int index, char *name, unsigned char *data, unsigned int max_size, unsigned int *data_size) {
  *data_size = ((values[index] * 256 + values[index + 1]) * 256 + values[index + 2]) * 256 + values[index + 3];
  if (*data_size > max_size) {
    printf("Data size exceeds expected maximum for this chunk!\n");
    return values_size;
  } index += 4;
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
