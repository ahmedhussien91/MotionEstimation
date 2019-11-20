#include "tss.h"

pos_t loop_pos[] = { {0,0}, {0,1},{0,-1},{1,0},{-1,0} };
uint8_t loop_pos_count = 5;

 tss :: tss(image* image_o, image* next_image, tss_conf_t* tss_conf) {
	 m_block_hight = tss_conf->block_hight;
	 m_block_width = tss_conf->block_width;
	 m_block_size = m_block_hight * m_block_width;
	 m_image_width = image_o->pixels_width;
	 m_image_hight = image_o->pixels_hight;
	 m_num_of_blocks = (m_image_width * m_image_hight) / (m_block_hight * m_block_width);

	 uint32_t numofblocks_in_width = m_image_width / m_block_width;
	 uint32_t numofblocks_in_hight = m_image_hight / m_block_hight;

	 motion_vector.number_of_vectors = m_num_of_blocks - 2 * numofblocks_in_width - 2 * numofblocks_in_hight +4;
	 motion_vector.block_motion_vector_array = new tss_block_motion_vector_t[motion_vector.number_of_vectors];

	 /*loop on image blocks*/
	 uint32_t block_anchor;
	 int16_t* t_block = new int16_t[m_block_size];
	 int16_t* t_1_block = new int16_t[m_block_size];
	 uint32_t motion_idx = 0;
	 for (uint32_t i = 0; i < m_num_of_blocks; i++)
	 {
		 /* Exclude blocks at the edge */
		 if ((i < numofblocks_in_width) ||
			 (i%numofblocks_in_width == 0) ||
			 (i%numofblocks_in_width == (numofblocks_in_width-1)) ||
			 (i > ((numofblocks_in_width * numofblocks_in_hight)- numofblocks_in_width))) {
			 continue;
		 }

		/*get block of values from t-image*/
		block_anchor = (i % numofblocks_in_width)*m_block_width + ((i - (i % numofblocks_in_width)) / numofblocks_in_width) * m_block_hight * m_image_width;
		motion_vector.block_motion_vector_array[motion_idx].original_block_index = block_anchor;
		get_block(block_anchor, t_block, image_o);


		/* 3- steps */
		pos_t min_pos = { 0,0 };
		float Error[5];
		float min_error = 0xFFFFFFF;
		uint8_t min_error_idx = 0;
		for (uint8_t j = 0; j < 3; j++) {
			 /*4- movments */
			 uint32_t Errors_count = 0;
			 for (uint8_t idx = 0; idx < loop_pos_count; idx++) {
				 /*get block of values from t+1 image*/
				 block_anchor = ((i % numofblocks_in_width)* m_block_width + (tss_conf->step[j] * loop_pos[idx].x + min_pos.x ))+ /*width*/
						((((i - (i % numofblocks_in_width)) / numofblocks_in_width))* m_block_hight * m_image_width + (tss_conf->step[j] * loop_pos[idx].y + min_pos.y)*m_image_width); /*hight*/
				 get_block(block_anchor, t_1_block, next_image);

				 /*match two blocks @ (0,step_1) */
				 switch (tss_conf->matching_type)
				 {
				 case TSS_MAD:
					 Error[Errors_count] = MAD_Calc(t_block, t_1_block);
					 break;
				 case TSS_PSNR:
					 Error[Errors_count] = PSNR_Calc(t_block, t_1_block);
					 break;

				 default:
					 _ASSERT(false);
					 break;
				 }
				 Errors_count++;
			 }
			 /*Select min Error Pos */
			 for (uint8_t err_idx = 0; err_idx < Errors_count; err_idx++) {
				 if (Error[err_idx] < min_error) {
					 min_error = Error[err_idx];
					 min_error_idx = err_idx;
				 }
			 }
			 min_pos.x += tss_conf->step[j] * loop_pos[min_error_idx].x;
			 min_pos.y += tss_conf->step[j] * loop_pos[min_error_idx].y;
			 if (min_pos.x > 7 || min_pos.y > 7)
			 {
				 min_pos.x += tss_conf->step[j] * loop_pos[min_error_idx].x;
			 }
		}
		/* motion vector of this block */
		motion_vector.block_motion_vector_array[motion_idx].offset.x = min_pos.x;
		motion_vector.block_motion_vector_array[motion_idx].offset.y = min_pos.y;
		motion_vector.block_motion_vector_array[motion_idx].Error = min_error;
		motion_idx++;
	 }
}

void tss::get_block(uint32_t offset, int16_t* block, image* image_input) {
	uint32_t index = 0;
	uint32_t i;
	uint32_t block_col = -1;
	for (i = 0; i < m_block_size; i++)
	{
		if (i % m_block_width == 0) {
			block_col++;
		}
		index = offset + (i % m_block_width) + (block_col * image_input->pixels_width);
		block[i] = image_input->YUV_image_in.Y_buff[index];
	}
}

void tss::get_block_U(uint32_t offset, int16_t* block, image* image_input) {
	uint32_t index = 0;
	uint32_t i;
	uint32_t block_col = -1;
	for (i = 0; i < m_block_size; i++)
	{
		if (i % m_block_width == 0) {
			block_col++;
		}
		index = offset + (i % m_block_width) + (block_col * image_input->pixels_width);
		block[i] = image_input->YUV_image_in.U_buff[index];
	}
}

void tss::get_block_V(uint32_t offset, int16_t* block, image* image_input) {
	uint32_t index = 0;
	uint32_t i;
	uint32_t block_col = -1;
	for (i = 0; i < m_block_size; i++)
	{
		if (i % m_block_width == 0) {
			block_col++;
		}
		index = offset + (i % m_block_width) + (block_col * image_input->pixels_width);
		block[i] = image_input->YUV_image_in.V_buff[index];
	}
}

void tss::write_block(uint32_t block_offset_output, uint32_t block_offset_input, image* output_image, image* input_image) {
	uint32_t index = 0;
	uint32_t i;
	uint32_t block_col = -1;
	int16_t* block2, * blockU, * blockV;

	block2 = new int16_t[m_block_size];
	blockU = new int16_t[m_block_size];
	blockV = new int16_t[m_block_size];

	get_block(block_offset_input, block2, input_image);
	get_block_U(block_offset_input, blockU, input_image);
	get_block_V(block_offset_input, blockV, input_image);

	for (i = 0; i < m_block_size; i++)
	{
		if (i % m_block_width == 0) {
			block_col++;
		}
		index = block_offset_output + (i % m_block_width) + (block_col * output_image->pixels_width);
		output_image->YUV_image_in.Y_buff[index] = block2[i];
		output_image->YUV_image_in.U_buff[index] = blockU[i];
		output_image->YUV_image_in.V_buff[index] = blockV[i];
	}
	delete[] block2;
	delete[] blockU;
	delete[] blockV;
}

float_t tss::MAD_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_block_hight * m_block_width;
	uint32_t i;
	float_t MAD, sum = 0.0;
	for (i = 0; i < block_size; i++)
	{
		sum += abs(image_block[i] - next_image_block[i]);
	}
	MAD = sum / block_size;
	return MAD;
}

float_t tss::MSE_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_block_hight * m_block_width;
	uint32_t i;
	float_t MSE, sum = 0.0;
	for (i = 0; i < block_size; i++)
	{
		sum += (image_block[i] - next_image_block[i]) * (image_block[i] - next_image_block[i]);
	}
	MSE = sum / block_size;
	return MSE;
}

float_t tss::PSNR_Calc(int16_t* image_block, int16_t* next_image_block) {
	float_t MSE;
	MSE = MSE_Calc(image_block, next_image_block);
	return (10 * log10((255 * 255) / MSE));
}
