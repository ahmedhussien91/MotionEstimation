#include "tss.h"

 tss :: tss(image* image_o, image* next_image, tss_conf_t* tss_conf) {
	 m_block_hight = tss_conf->block_hight;
	 m_block_width = tss_conf->block_width;
	 m_block_size = m_block_hight * m_block_width;
	 m_image_width = image_o->pixels_width;
	 m_image_hight = image_o->pixels_hight;
	 m_num_of_blocks = (m_image_width * m_image_hight) / (m_block_hight * m_block_width);

	 uint32_t numofblocks_in_width = m_image_width / m_block_width;
	 uint32_t numofblocks_in_hight = m_image_hight / m_block_hight;

	 motion_vector.number_of_vectors = m_num_of_blocks - 2 * numofblocks_in_width - 2 * numofblocks_in_hight;

	 /*loop on image blocks*/
	 uint32_t block_anchor;
	 int16_t* t_block = new int16_t[m_block_size];
	 int16_t* t_1_block = new int16_t[m_block_size];
	 for (uint32_t i = 0; i < m_num_of_blocks; i++)
	 {
		 /* Exclude blocks at the edge */
		 if ((i < numofblocks_in_width) ||
			 (i%numofblocks_in_width == 0) ||
			 (i%numofblocks_in_width == (numofblocks_in_width-1)) ||
			 (i>((m_image_width * m_image_hight)- m_image_width))) {
			 continue;
		 }
		 /*get block of values from t-image*/
		 block_anchor = (i % numofblocks_in_width) + ((i - (i % numofblocks_in_width)) / numofblocks_in_width) * m_block_hight * m_image_width;
		 get_block(block_anchor, t_block, image_o);

		 /*get block of values from t+1 image*/
		 block_anchor = (i % numofblocks_in_width) + ((i - (i % numofblocks_in_width)) / numofblocks_in_width) * m_block_hight * m_image_width;
		 get_block(block_anchor, t_1_block, next_image);

		 /*match two blocks @ (0,0) */ 


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
