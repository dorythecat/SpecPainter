#include "png.h"

#define SAFE_FREE(p) free(p); p = NULL;
#define SAFE_MOVE(from, to) to = from; from = NULL;

typedef struct {
    unsigned char *ptr;
    size_t written;
} DecompContext;

static int tinfl_put_buf_func(const void *pBuf, int len, void *pUser) {
  DecompContext *pCtx = (DecompContext *)pUser;
  memcpy(pCtx->ptr, pBuf, len);
  pCtx->ptr += len;
  pCtx->written +=len;
  return 1;
}

void decode_png() {
  FILE *fp = fopen("logo.png", "r");
  if (fp == NULL || ferror(fp)) { // File could not be loaded
    printf("Error encountered when opening file!\n");
    fclose(fp);
    return;
  }
  unsigned int limit = 65536; // 2^16
  unsigned char *values = malloc(sizeof *values * limit);
  if (values == NULL) {
    printf("Error encountered when allocating memory for image data!\n");
    fclose(fp);
    return;
  }
  unsigned int i = 0;
  while (!feof(fp)) {
    if (i == limit) { // If we don't have enough space, resize the array to fit all of our data
      limit *= 2;
      unsigned char *temp = realloc(values, sizeof *values * limit);
      if (temp == NULL) {
        printf("Error encountered when growing memory for image data!\n");
        SAFE_FREE(values);
        fclose(fp);
        return;
      } values = temp;
    } values[i++] = (unsigned char)fgetc(fp);
  } fclose(fp);
  unsigned int values_size = i - 2;

  // Resize our array to be EXACTLY as much space as we need
  // To be a bit more efficient we can just use temp going forwards tbh
  unsigned char *temp = realloc(values, sizeof *values * values_size);
  if (temp == NULL) {
    printf("Error encountered when shrinking memory for image data!\n");
    SAFE_FREE(values);
    return;
  } SAFE_MOVE(temp, values);

  printf("File size: %d\n", values_size);
  if (values[0] != 137 || values[1] != 80 || values[2] != 78 || values[3] != 71 ||
      values[4] != 13  || values[5] != 10 || values[6] != 26 || values[7] != 10) {
    printf("Invalid file signature!\n");
    SAFE_FREE(values);
    return;
  }

  // Load IHDR chunk
  char *name = malloc(sizeof *name * 4);
  if (name == NULL) {
    printf("Error encountered when allocating memory for chunk name!\n");
    SAFE_FREE(values);
    return;
  }
  unsigned char *data = malloc(sizeof *data * 13);
  if (data == NULL) {
    printf("Error encountered when allocating memory for chunk data!\n");
    SAFE_FREE(name);
    SAFE_FREE(values);
    return;
  }
  unsigned int size = 0;
  unsigned int index = chunk_read(values, 8, name, data, 13, &size);
  if (name[0] != 73 || name[1] != 72 || name[2] != 68 || name[3] != 82 || size != 13) {
    printf("Invalid first chunk!\n");
    SAFE_FREE(name);
    SAFE_FREE(data);
    SAFE_FREE(values);
    return;
  } SAFE_FREE(name);

  unsigned int width = ((data[0] * 256 + data[1]) * 256 + data[2]) * 256 + data[3];
  unsigned int height = ((data[4] * 256 + data[5]) * 256 + data[6]) * 256 + data[7];
  char bit_depth = data[8];
  char color_type = data[9];
  char compression = data[10];
  char filter = data[11];
  char interlace = data[12];
  SAFE_FREE(data);

  if (color_type != 0 && color_type != 2 && color_type != 3 && color_type != 4 && color_type != 6) {
    printf("Image color type is unsupported!\n");
    SAFE_FREE(values);
    return;
  }

  if ((color_type == 0 && bit_depth != 1 && bit_depth != 2 && bit_depth != 4 && bit_depth != 8 && bit_depth != 16) ||
      ((color_type == 2 || color_type == 4 || color_type == 6) && bit_depth != 8 && bit_depth != 16) ||
      (color_type == 3 && bit_depth != 1 && bit_depth != 2 && bit_depth != 4 && bit_depth != 8)) {
    printf("Image bit depth is unsupported!\n");
    SAFE_FREE(values);
    return;
  }

  if (compression != 0) {
    printf("Image compression method is unsupported!\n");
    SAFE_FREE(values);
    return;
  }

  if (filter != 0) {
    printf("Image filter method is unsupported!\n");
    SAFE_FREE(values);
    return;
  }

  if (interlace != 0 && interlace != 1) {
    printf("Image interlace method is unsupported!\n");
    SAFE_FREE(values);
    return;
  }

  printf("Image width: %d\n", width);
  printf("Image height: %d\n", height);

  name = malloc(sizeof *name * 4);
  if (name == NULL) {
    printf("Error encountered when allocating memory for chunk name!\n");
    SAFE_FREE(values);
    return;
  }

  data = malloc(sizeof *data * 8192);
  if (data == NULL) {
    printf("Error encountered when allocating memory for chunk data!\n");
    SAFE_FREE(name);
    SAFE_FREE(values);
    return;
  }

  // Load all of the following chunks, ignore unknown chunk formats and handle all known cases
  unsigned char *palette = NULL; // In case we need to have a palette
  unsigned int palette_size = 0; // Size of the palette, in number of entries
  unsigned char *transparency = NULL;
  unsigned char *idat = NULL;
  size_t idat_size = 0;
  while (1) {
    index = chunk_read(values, index, name, data, 8192, &size); 
    printf("Chunk name and size: (%s, %d)\n", name, size, index);
    if (index == 0) break;

    if (name[0] == 80 && name[1] == 76 && name[2] == 84 && name[3] == 69) { // PLTE
      if (size % 3) {
        printf("Invalid PLTE chunk size!\n");
        break;
      }

      palette_size = size / 3;
      palette = malloc(sizeof *palette * size);
      if (palette == NULL) {
        printf("Error encountered when allocating memory for palette data!\n");
        break;
      } for (unsigned int i = 0; i < size; i++) palette[i] = data[i];
    } else if (name[0] == 116 && name[1] == 82 && name[2] == 78 && name[3] == 83) { // tRNS
      if (color_type == 4 || color_type == 6) {
        printf("Transparency chunk present on image with transparency data!\n");
        break;
      }

      if (color_type == 0 || color_type == 2) {
        transparency = malloc(sizeof *transparency * (size / 2));
        if (transparency == NULL) {
          printf("Error encountered when allocating memory for transparency data!\n");
          break;
        } for (unsigned int i = 0; i < size; i += 2) transparency[i] = data[i] * 256 + data[i + 1];
      } else {
        transparency = malloc(sizeof *transparency * size);
        if (transparency == NULL) {
          printf("Error encountered when allocating memory for transparency data!\n");
          break;
        } for (unsigned int i = 0; i < size; i++) transparency[i] = data[i];
      }
    } else if (name[0] == 73 && name[1] == 68 && name[2] == 65 && name[3] == 84) { // IDAT
      idat_size += size;
      unsigned char *temp = realloc(idat, sizeof *idat * idat_size);
      if (temp == NULL) {
        printf("Error encountered when growing memory for image data!\n");
        break;
      } idat = temp;
      for (unsigned int i = 0; i < size; i++) idat[idat_size - size + i] = data[i];
    } if (name[0] == 73 && name[1] == 69 && name[2] == 78 && name[3] == 68) { // IEND
      break; // If we don't do this, we WILL get a segmentation fault if the CRC of the IEND chunk is incorrect!
    }

    if (name[0] & 32 != 32) { // Critical chunk that's not a defined critical chunk
      // As per the PNGv3 specification, we're required to throw an error
      printf("Found critical chunk with unrecognized name!\n");
      break;
    }
  }
  SAFE_FREE(data);
  SAFE_FREE(name);
  SAFE_FREE(values);

  if (index == (unsigned int)-1) { // Error ocurred
    printf("Fatal error found while loading chunk, aborting...\n");
    SAFE_FREE(palette);
    SAFE_FREE(transparency);
    SAFE_FREE(idat);
    return;
  }

  if (color_type == 3 && palette == NULL) {
    printf("Palette not found for indexed color type!\n");
    return;
  }

  // Decompress IDAT chunks
  if ((idat[0] & 0x0F) != 8 || ((idat[0] << 8) | idat[1]) % 31 || idat[1] & 0x20) {
    printf("Error with ZLIB header of IDAT data!\n");
    SAFE_FREE(palette);
    SAFE_FREE(transparency);
    SAFE_FREE(idat);
    return;
  }

  printf("Using miniz.c version: %s\n", MZ_VERSION);

  unsigned char *idat_decomp = malloc(sizeof *idat_decomp * idat_size * 1032); // Roughly the maximum theoretical compression factor
  if (idat_decomp == NULL) {
    printf("Error encountered when allocating memory for decompressed image data!\n");
    SAFE_FREE(palette);
    SAFE_FREE(transparency);
    SAFE_FREE(idat);
    return;
  }

  DecompContext ctx;
  ctx.ptr = idat_decomp;
  ctx.written = 0;
  int status = tinfl_decompress_mem_to_callback(idat, &idat_size, tinfl_put_buf_func, &ctx, TINFL_FLAG_PARSE_ZLIB_HEADER);
  free(idat);
  if (!status) {
    printf("tinfl_decompress_mem_to_callback() failed with status %i!\n", status);
    SAFE_FREE(palette);
    SAFE_FREE(transparency);
    SAFE_FREE(idat_decomp);
    return;
  }

  printf("Decompressed %ld bytes successfully!\n", ctx.written);

  data = NULL; // Reusing an old name, technically a sin I guess? :/
  if (color_type == 3) {
    data = malloc(sizeof *data * ctx.written * 3);
    if (data == NULL) {
      printf("Error encountered when allocating memory for parsed image data!\n");
      SAFE_FREE(palette);
      SAFE_FREE(transparency);
      SAFE_FREE(idat_decomp);
      return;
    }

    for (unsigned int i = 0; i < ctx.written; i++) {
      unsigned int dindex = 3 * i;
      unsigned char pindex = 3 * idat_decomp[i];
      if (dindex > 3 * ctx.written | pindex > palette_size) {
        printf("Error encountered while parsing palette!\n");
        SAFE_FREE(palette);
        SAFE_FREE(transparency);
        SAFE_FREE(idat_decomp);
        return;
      } for (unsigned int j = 0; j < 3; j++) data[dindex++] = palette[pindex++];
    }
  } else SAFE_MOVE(idat_decomp, data);
  SAFE_FREE(palette);
  SAFE_FREE(idat_decomp);

  SAFE_FREE(transparency);
}

unsigned long crc_table[256];
char crc_table_computed = 0; // Wether the CRC helper table has been computed or not

void make_crc_table(void) {
  for (int n = 0; n < 256; n++) {
    unsigned long c = (unsigned long)n;
    for (char k = 0; k < 8; k++) c = c & 1 ? 0xedb88320L ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  } crc_table_computed = 1;
}

unsigned long update_crc(unsigned long crc, unsigned char *buf, unsigned int len) {
  unsigned long c = crc;
  if (!crc_table_computed) make_crc_table();
  for (unsigned int n = 0; n < len; n++) c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  return c;
}

unsigned long crc(char *name, unsigned char *data, unsigned int data_size) {
  return update_crc(update_crc(0xffffffffL, (unsigned char *)name, 4), data, data_size) ^ 0xffffffffL;
}

unsigned int chunk_read(unsigned char *values, unsigned int index, char *name, unsigned char *data, unsigned int max_size, unsigned int *data_size) {
  *data_size = ((values[index] * 256 + values[index + 1]) * 256 + values[index + 2]) * 256 + values[index + 3];
  if (*data_size > max_size) {
    printf("Data size exceeds expected maximum for this chunk!\n");
    return (unsigned int)-1;
  } index += 4;
  for (char i = 0; i < 4; i++) name[i] = values[index++];
  if (((unsigned char)name[2] & 32) == 32) {
    printf("Image does not conform to PNGv3 standard!\n");
    return (unsigned int)-1;
  }
 
  for (unsigned int i = 0; i < *data_size; i++) data[i] = values[index++];

  unsigned long crc_val = 0;
  for (char i = 0; i < 4; i++) crc_val = crc_val * 256 + values[index++];
  if (crc_val != crc(name, data, *data_size)) {
    printf("%s chunk has incorrect CRC signature!\n", name);
    return (unsigned int)-1;
  }

  if (*data_size == 0) return 0; // Otherwise the IEND chunk will be read twice and throw an error
  return index;
}
