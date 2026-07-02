// PNG Decoder, as per https://www.w3.org/TR/png-3/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib/miniz/miniz.h"

int decode_png(char *filename, unsigned char **out, unsigned long *out_size);
unsigned int chunk_read(unsigned char *values, unsigned int index, char *name, unsigned char **data, unsigned int *data_size);
