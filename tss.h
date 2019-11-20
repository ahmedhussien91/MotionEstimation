#pragma once
#include "image.h"

typedef enum tss_matching_type_e {
	TSS_MAD,
	TSS_PSNR
};

typedef struct pos_t
{
	int32_t x;
	int32_t y;
};

typedef struct tss_conf_t {
	uint32_t block_width;
	uint32_t block_hight;
	int32_t step[3];
	tss_matching_type_e matching_type;
};

typedef struct tss_block_motion_vector_t {
	uint32_t original_block_index;
	pos_t    offset;
	uint32_t  Error;
};
typedef struct tss_motion_vector_t {
	tss_block_motion_vector_t* block_motion_vector_array;
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

	tss_motion_vector_t motion_vector;
private:
	uint32_t m_block_width;
	uint32_t m_block_hight;
	uint32_t m_block_size;
	uint32_t m_image_width;
	uint32_t m_image_hight;
	uint32_t m_num_of_blocks;
};

