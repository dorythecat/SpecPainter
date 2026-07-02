#include "utils/png/png.h"

int main() {
  char *filename = "logo.png";
  unsigned char *data = NULL;
  unsigned long size = 0;
  int result = decode_png(filename, &data, &size);
  return result;
}
