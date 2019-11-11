#include "window.h"
#include <math.h>       /* log */

void window::rearrange_array(int16_t* arr_1D) { 
	//block_size = 64 (8x8)
	//image_width = 512 
	// create 64 x arr_Blocks_2D_size containers for 1d arr
	uint32_t block_size = m_win_conf->block_hight*m_win_conf->block_width;
	uint32_t Image_width = m_image_o->pixels_width;
	image_o_num_of_blocks = ((m_image_o->pixels_hight*m_image_o->pixels_width) / block_size);
	image_o_blocks = new int16_t * [image_o_num_of_blocks];
	for (uint32_t i = 0; i < image_o_num_of_blocks; i++) {
		image_o_blocks[i] = new int16_t[block_size];
	}

	// fill cotainer with 2d data
	uint32_t b_h = m_win_conf->block_hight, b_w = m_win_conf->block_width;
	uint32_t image_h = m_image_o->pixels_hight;
	uint32_t col_idx = -1, hight_offset = 0;
	hight_offset = (b_h * Image_width);
	for (uint32_t h = 0; h < image_h / b_h; h++) {
		for (uint32_t i = 0; i < Image_width / b_w; i++) {
			col_idx = -1;
			for (uint32_t j = 0; j < block_size; j++) {

				if (j % b_w == 0)
				{
					col_idx++;
				}
				image_o_blocks[i + (h * (Image_width / b_w))][j] = arr_1D[(j % b_w) + (col_idx * Image_width) + (i * b_w) + hight_offset * h];
			}
		}
	}
}

uint32_t window::MAD_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_win_conf->block_hight * m_win_conf->block_width;
	uint32_t i, sum=0;
	uint32_t MAD;
	for (i = 0; i < block_size; i++)
	{
		sum += abs(image_block[i] - next_image_block[i]);
	}
	 MAD = (1 / block_size) * sum;
	 return MAD;
}

uint32_t window::MSE_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_win_conf->block_hight * m_win_conf->block_width;
	uint32_t i, sum = 0;
	uint32_t MSE;
	for (i = 0; i < block_size; i++)
	{
		sum += (image_block[i] - next_image_block[i])* (image_block[i] - next_image_block[i]);
	}
	MSE = (1 / block_size) * sum;
	return MSE;
}

uint32_t window::PSNR_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t MSE;
	MSE = MSE_Calc(image_block, next_image_block);
	return (10 * log10((255 * 255) / MSE));
}