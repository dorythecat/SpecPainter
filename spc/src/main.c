#include "utils/png/png.h"

int main() {
  char *filename = "logo.png";
  unsigned char *data = NULL;
  unsigned long size = 0;
  unsigned int width = 0, height = 0;
  char bit_depth = -1, color_type = -1;
  int result = decode_png(filename, &data, &size, &width, &height, &bit_depth, &color_type);
  free(data); // We have to free it once we've finished using it
  return result;
}
