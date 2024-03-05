#ifndef NKX_UTIL_NKX_IMAGE_H
#define NKX_UTIL_NKX_IMAGE_H

#include <stdlib.h>  // for NULL macro

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct{
  unsigned char *data;
  unsigned int width;
  unsigned int height;
  unsigned int ch;
} ImageData;

extern int pngFileEncodeWrite(ImageData *imageData, const char *filename);
extern ImageData* newImageData(int w, int h, int ch);
extern int freeImageData(ImageData *image);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NKX_UTIL_NKX_IMAGE_H */

