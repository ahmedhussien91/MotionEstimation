#pragma once
#include"image.h"

typedef struct motion_vector_t {

};

typedef struct window_conf_t {
	uint16_t block_width;
	uint16_t block_hight;
	uint16_t window_width; // multiple of block width
	uint16_t window_hight; // multiple of block hight 

};

typedef enum matching_type_e {
	MAD,
	PSNR
};
class window
{
public:

	window(image* image_o, image* next_image, window_conf_t* win_conf) {
		m_image_o = image_o;
		m_next_image = next_image;
		m_win_conf = win_conf;
		
		rearrange_array(m_image_o->YUV_image_in.Y_buff); // fill image_o_blocks, image_o_num_of_blocks
		
		/* calculate num_of_windows */
		no_of_pixels_win = (m_win_conf->window_hight) *(m_win_conf->window_width); // block change every 1 pixel
		no_of_windows_in_width = (m_next_image->pixels_width / m_win_conf->window_width) + (2*(uint32_t)(m_win_conf->window_width/2));
		no_of_windows_in_hight = m_next_image->pixels_hight / m_win_conf->window_hight + (2 * (uint32_t)(m_win_conf->window_hight / 2));
		no_of_windows = no_of_windows_in_hight * no_of_windows_in_hight;
		no_of_blocks_win_width = m_win_conf->window_width / m_win_conf->block_width;
		no_of_blocks_win_hight = m_win_conf->window_hight / m_win_conf->block_hight;
		no_of_blocks_win = no_of_blocks_win_width * no_of_blocks_win_hight;
		block_size = m_win_conf->block_hight * m_win_conf->block_width;
		
		/* loop over windows in the m_next_image*/
		uint32_t i = 0;
		uint32_t middle_index= 0;
		for (i = 0; i < no_of_windows; i++)
		{
			/*get windows index (anchor top-left)*/
			uint32_t win_index = get_win_index(i);
			/*get the middle block id - to access in image_o_blocks*/
			middle_index = get_middle_block_index(win_index);
			if (middle_index == 0)
			{ // wrong index not found 
				_ASSERT(false);
			}

			/*loop on blocks in the window - blocks move 1 pixel*/
			uint32_t j = 0;
			for (j = 0; j < no_of_pixels_win;j++)
			{
				/*get block given pixel index of top left array*/

				/*compare the two arrays using MAD_Calc or MSE_Calc or PSNR_Calc*/

				/*update motion vector for this block depending on output */
			}
		}

	};


	uint32_t get_win_index(uint32_t window_index) {
		uint32_t index = 0;

		index = (window_index % no_of_windows_in_width) * m_win_conf->window_width + (window_index - (window_index % no_of_windows_in_width)) * m_win_conf->window_hight;

		return index;
	}

	uint32_t get_middle_block_index(uint32_t window_index) {
		uint32_t index = 0;
		uint32_t middle_h_off, middle_w_off;

		middle_w_off = no_of_blocks_win_width/2;
		middle_h_off = no_of_blocks_win_hight/2;
		index = window_index + middle_w_off*m_win_conf->block_width + middle_h_off * m_win_conf->block_hight * m_next_image->pixels_width;
		return index;
	}

	void get_block(uint32_t offset, int16_t * block) {
		uint32_t index = 0;
		uint32_t i;
		for (i=0; i<block_size; i++)
		{

		}
	}
	void rearrange_array(int16_t* arr_1D);
	uint32_t MAD_Calc(int16_t* image_block, int16_t* next_image_block);
	uint32_t MSE_Calc(int16_t* image_block, int16_t* next_image_block);
	uint32_t PSNR_Calc(int16_t* image_block, int16_t* next_image_block);

	motion_vector_t vector;

private:
	int16_t** image_o_blocks;
	uint32_t image_o_num_of_blocks;
	uint32_t no_of_windows;
	uint32_t no_of_pixels_win;
	uint32_t no_of_windows_in_width;
	uint32_t no_of_windows_in_hight;
	uint32_t no_of_blocks_win_width;
	uint32_t no_of_blocks_win_hight;
	uint32_t no_of_blocks_win;
	uint32_t block_size;
	window_conf_t* m_win_conf;
	image* m_image_o;
	image* m_next_image;
};

