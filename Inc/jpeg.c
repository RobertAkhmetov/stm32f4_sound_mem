#include "stm32f10x_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include "spi_ili9341.h"
//#include "picojpeg.h"
#include "signal.h"
//#include "bw.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void drawRGBBitmap(void *bitmap, int poX, int poY, int width, int height, int components)
{
	int x, y;

	for(y=height-1; y!=-1; y--) {
		for(x=0; x<width; ++x){
			uint8 *d = (uint8 *)bitmap + (y * width + x) * components;

			TFT_setPixel(poX + x, poY + y, TFT_rgb2color(d[0], d[1], d[2]));
		}
	}

}


unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
   uint n;
   jpeg_buffer_t *jpeg_buffer = (jpeg_buffer_t*)pCallback_data;

   n = MIN(jpeg_buffer->length - jpeg_buffer->position, buf_size);
   memcpy(pBuf, jpeg_buffer->buffer + jpeg_buffer->position, n);
   *pBytes_actually_read = n;

   jpeg_buffer->position += n;

   return 0;
}

void *jpeg_decode_buffer(pjpeg_image_info_t image_info)
{
	unsigned char status;
	//pjpeg_image_info_t image_info;
	int mcu_x = 0;
	int mcu_y = 0;
	uint row_pitch;
	uint decoded_width, decoded_height;
	uint row_blocks_per_mcu, col_blocks_per_mcu;
	char *pImage;

	decoded_width = image_info.m_width;
	decoded_height = image_info.m_height;

	row_pitch = decoded_width * image_info.m_comps;

	row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
	col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

	pImage = (uint8 *)malloc(row_pitch * decoded_height);

	for (;;)
	{
		int y, x;
		uint8 *pDst_row;

		status = pjpeg_decode_mcu();

		if (status)
		{
			if (status != PJPG_NO_MORE_BLOCKS)
			{
				free(pImage);
				return NULL;
			}

			break;
		}

		if (mcu_y >= image_info.m_MCUSPerCol)
		{
			free(pImage);
			return NULL;
		}

		// Copy MCU's pixel blocks into the destination bitmap.
		pDst_row = pImage + (mcu_y * image_info.m_MCUHeight) * row_pitch + (mcu_x * image_info.m_MCUWidth * image_info.m_comps);

		for (y = 0; y < image_info.m_MCUHeight; y += 8)
		{
			const int by_limit = MIN(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

			for (x = 0; x < image_info.m_MCUWidth; x += 8)
			{
				uint8 *pDst_block = pDst_row + x * image_info.m_comps;

				// Compute source byte offset of the block in the decoder's MCU buffer.
				uint src_ofs = (x * 8U) + (y * 16U);
				const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
				const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
				const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

				const int bx_limit = MIN(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

				if (image_info.m_scanType == PJPG_GRAYSCALE)
				{
					int bx, by;
					for (by = 0; by < by_limit; by++)
					{
						uint8 *pDst = pDst_block;

						for (bx = 0; bx < bx_limit; bx++)
						*pDst++ = *pSrcR++;

						pSrcR += (8 - bx_limit);

						pDst_block += row_pitch;
					}
				}
				else
				{
					int bx, by;
					for (by = 0; by < by_limit; by++)
					{
						uint8 *pDst = pDst_block;

						for (bx = 0; bx < bx_limit; bx++)
						{
							pDst[0] = *pSrcR++;
							pDst[1] = *pSrcG++;
							pDst[2] = *pSrcB++;

							pDst += 3;
						}

						pSrcR += (8 - bx_limit);
						pSrcG += (8 - bx_limit);
						pSrcB += (8 - bx_limit);

						pDst_block += row_pitch;
					}
				}
			}

			pDst_row += (row_pitch * 8);
		 }

		mcu_x++;
		if (mcu_x == image_info.m_MCUSPerRow)
		{
			mcu_x = 0;
			mcu_y++;
		}
	}

	return pImage;
}

void jpeg_show(pjpeg_image_info_t image_info, int poX, int poY)
{
	unsigned char status;
	//pjpeg_image_info_t image_info;
	int mcu_x = 0;
	int mcu_y = 0;
	uint row_pitch;
	uint decoded_width, decoded_height;
	uint row_blocks_per_mcu, col_blocks_per_mcu;

	decoded_width = image_info.m_width;
	decoded_height = image_info.m_height;

	row_pitch = decoded_width * image_info.m_comps;

	row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
	col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

	for (;;)
	{
		int y, x;
		uint8 *pDst_row;

		status = pjpeg_decode_mcu();

		if (status)
		{
			if (status != PJPG_NO_MORE_BLOCKS)
			{
				return;
			}

			break;
		}

		if (mcu_y >= image_info.m_MCUSPerCol)
		{
			return;
		}

		for (y = 0; y < image_info.m_MCUHeight; y += 8)
		{
			const int by_limit = MIN(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

			for (x = 0; x < image_info.m_MCUWidth; x += 8)
			{
				// Compute source byte offset of the block in the decoder's MCU buffer.
				uint src_ofs = (x * 8U) + (y * 16U);
				const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
				const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
				const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

				const int bx_limit = MIN(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

				if (image_info.m_scanType == PJPG_GRAYSCALE)
				{
					int bx, by;
					for (by = 0; by < by_limit; by++)
					{
						for (bx = 0; bx < bx_limit; bx++)
						{
							int Gr = *pSrcR++;
							TFT_setPixel((mcu_x * image_info.m_MCUWidth) + x + bx + poX, (mcu_y * image_info.m_MCUHeight)+ y + by + poY, TFT_rgb2color(Gr, Gr, Gr));
						}

						pSrcR += (8 - bx_limit);
					}
				}
				else
				{
					int bx, by;
					for (by = 0; by < by_limit; by++)
					{
						for (bx = 0; bx < bx_limit; bx++)
						{
							int R = *pSrcR++;
							int G = *pSrcG++;
							int B = *pSrcB++;
							TFT_setPixel((mcu_x * image_info.m_MCUWidth) + x + bx + poX, (mcu_y * image_info.m_MCUHeight)+ y + by + poY, TFT_rgb2color(R, G, B));
						}

						pSrcR += (8 - bx_limit);
						pSrcG += (8 - bx_limit);
						pSrcB += (8 - bx_limit);
					}
				}
			}

			pDst_row += (row_pitch * 8);
		 }

		mcu_x++;
		if (mcu_x == image_info.m_MCUSPerRow)
		{
			mcu_x = 0;
			mcu_y++;
		}
	}

}