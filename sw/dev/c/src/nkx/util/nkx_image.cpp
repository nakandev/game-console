#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nkx/util/nkx_image.h>

int pngFileEncodeWrite(ImageData *imageData, const char *filename){
  FILE *fo;
  int j;

  png_structp png;
  png_infop info;
  png_bytepp datap;
  png_byte type;

  fo = fopen(filename, "wb");
  if(fo == NULL){
    printf("%sは開けません\n", filename);
    return -1;
  }

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);

  if(imageData->ch == 3) {
    type = PNG_COLOR_TYPE_RGB;
  } else if(imageData->ch == 4) {
    type = PNG_COLOR_TYPE_RGB_ALPHA;
  } else {
    printf("ch num is invalid!\n");
    png_destroy_write_struct(&png, &info);
    fclose(fo);
    return -1;
  }
  png_init_io(png, fo);
  png_set_IHDR(png, info,
      imageData->width, imageData->height, 8, type,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  datap = (png_bytepp) png_malloc(png, sizeof(png_bytep) * imageData->height);

  png_set_rows(png, info, datap);

  for(j = 0; j < imageData->height; j++){
    datap[j] = (png_bytep) png_malloc(png, imageData->width * imageData->ch);
    memcpy(datap[j], imageData->data + j * imageData->width * imageData->ch, imageData->width * imageData->ch);
  }
  png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

  for(j = 0; j < imageData->height; j++){
    png_free(png, datap[j]);
  }
  png_free(png, datap);

  png_destroy_write_struct(&png, &info);
  fclose(fo);
  return 0;
}

ImageData* newImageData(int w, int h, int ch){
  ImageData* imageData = (ImageData*) malloc(sizeof(ImageData));
  imageData->data = (unsigned char*) malloc(w * h * ch);
  imageData->width = w;
  imageData->height = h;
  imageData->ch = ch;
  return imageData;
}

int freeImageData(ImageData *image){
  if(image->data != NULL){
    free(image->data);
    image->data = NULL;
  }
  return 0;
}
