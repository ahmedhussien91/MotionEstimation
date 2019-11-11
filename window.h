#pragma once
#include"image.h"

typedef struct block_motion_vector_t{
	uint32_t original_block_index;
	uint32_t block_index;
	uint32_t windows_index;
	uint32_t distance;
};
typedef struct motion_vector_t {
	block_motion_vector_t * block_motion_vector_array;
	uint32_t number_of_vectors; 
};

typedef struct window_conf_t {
	uint16_t block_width;
	uint16_t block_hight;
	uint16_t window_width; // multiple of block width
	uint16_t window_hight; // multiple of block hight 
	matching_type_e matching_type;

};

typedef enum matching_type_e {
	MAD,
	PSNR
};
class window
{
public:

	window(image* image_o, image* next_image, window_conf_t* win_conf);
	uint32_t get_win_index(uint32_t window_index);
	uint32_t get_middle_block_index(uint32_t window_index);
	uint32_t get_block_index(uint32_t window_index, uint32_t index_inside_win);
	void get_block(uint32_t offset, int16_t * block, image* image_input);
	uint32_t get_distance_between_blocks(uint32_t index_1,uint32_t index_2);
	// void rearrange_array(int16_t* arr_1D);
	float_t MAD_Calc(int16_t* image_block, int16_t* next_image_block);
	float_t MSE_Calc(int16_t* image_block, int16_t* next_image_block);
	float_t PSNR_Calc(int16_t* image_block, int16_t* next_image_block);

	motion_vector_t vector;

private:
	int16_t** image_o_blocks;
	uint32_t image_o_num_of_blocks;
	uint32_t no_of_windows;
	uint32_t no_of_pixels_win; // number of pixels that can anchor a block in a window
	uint32_t no_of_windows_in_width;
	uint32_t no_of_windows_in_hight;
	uint32_t no_of_blocks_win_width;
	uint32_t no_of_blocks_win_hight;
	uint32_t no_of_blocks_win;
	uint32_t block_size;
	window_conf_t* m_win_conf;
	image* m_image_o;
	image* m_next_image;
	motion_vector_t motion_vector;
};

