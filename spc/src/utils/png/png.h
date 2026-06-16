// PNG Decoder, as per https://www.w3.org/TR/png-3/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decode_png();
unsigned int chunk_read(unsigned char *values, unsigned int index, char *name, unsigned char *data, unsigned int max_size, unsigned int *data_size);
