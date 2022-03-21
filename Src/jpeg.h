
#include <stdio.h>
#include <stdlib.h>
#include "picojpeg.h"
#include "signal.h"

// прототипы
void drawRGBBitmap(void *bitmap, int poX, int poY, int width, int height, int components);
unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
void *jpeg_decode_buffer(pjpeg_image_info_t image_info);
void jpeg_show(pjpeg_image_info_t image_info, int poX, int poY);


