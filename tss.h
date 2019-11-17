#pragma once
#include "image.h"

typedef enum matching_type_e {
	MAD,
	PSNR
};

typedef struct tss_conf_t {
	uint32_t block_width;
	uint32_t block_hight;
	uint32_t step_1;
	uint32_t step_2;
	uint32_t step_3;
	matching_type_e matching_type;
};

typedef struct block_motion_vector_t {
	uint32_t original_block_index;
	uint32_t block_index;
	uint32_t windows_index;
	uint32_t distance;
};
typedef struct motion_vector_t {
	block_motion_vector_t* block_motion_vector_array;
	uint32_t number_of_vectors;
};

class tss
{
public:
	tss(image* image_o, image* next_image, tss_conf_t* tss_conf);
	void get_block(uint32_t offset, int16_t* block, image* image_input);
	void get_block_U(uint32_t offset, int16_t* block, image* image_input);
	void get_block_V(uint32_t offset, int16_t* block, image* image_input);
	void write_block(uint32_t block_offset_output, uint32_t block_offset_input, image* output_image, image* input_image);
	float_t MAD_Calc(int16_t* image_block, int16_t* next_image_block);
	float_t MSE_Calc(int16_t* image_block, int16_t* next_image_block);
	float_t PSNR_Calc(int16_t* image_block, int16_t* next_image_block);

	motion_vector_t motion_vector;
private:
	uint32_t m_block_width;
	uint32_t m_block_hight;
	uint32_t m_block_size;
	uint32_t m_image_width;
	uint32_t m_image_hight;
	uint32_t m_num_of_blocks;
};

