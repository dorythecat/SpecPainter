// PNG Decoder, as per https://www.w3.org/TR/png-3/

#pragma once

#include <stdio.h>
#include <stdlib.h>

void decode_png();
void chunk_read(unsigned char *values, unsigned int size, unsigned int chunk_index, char *name);
