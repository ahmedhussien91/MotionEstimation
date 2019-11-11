#include "window.h"
#include <math.h>       /* log */

// void window::rearrange_array(int16_t* arr_1D) { 
// 	//block_size = 64 (8x8)
// 	//image_width = 512 
// 	// create 64 x arr_Blocks_2D_size containers for 1d arr
// 	uint32_t block_size = m_win_conf->block_hight*m_win_conf->block_width;
// 	uint32_t Image_width = m_image_o->pixels_width;
// 	image_o_num_of_blocks = ((m_image_o->pixels_hight*m_image_o->pixels_width) / block_size);
// 	image_o_blocks = new int16_t * [image_o_num_of_blocks];
// 	for (uint32_t i = 0; i < image_o_num_of_blocks; i++) {
// 		image_o_blocks[i] = new int16_t[block_size];
// 	}

// 	// fill cotainer with 2d data
// 	uint32_t b_h = m_win_conf->block_hight, b_w = m_win_conf->block_width;
// 	uint32_t image_h = m_image_o->pixels_hight;
// 	uint32_t col_idx = -1, hight_offset = 0;
// 	hight_offset = (b_h * Image_width);
// 	for (uint32_t h = 0; h < image_h / b_h; h++) {
// 		for (uint32_t i = 0; i < Image_width / b_w; i++) {
// 			col_idx = -1;
// 			for (uint32_t j = 0; j < block_size; j++) {

// 				if (j % b_w == 0)
// 				{
// 					col_idx++;
// 				}
// 				image_o_blocks[i + (h * (Image_width / b_w))][j] = arr_1D[(j % b_w) + (col_idx * Image_width) + (i * b_w) + hight_offset * h];
// 			}
// 		}
// 	}
// }

window::window(image* image_o, image* next_image, window_conf_t* win_conf) {
	m_image_o = image_o;
	m_next_image = next_image;
	m_win_conf = win_conf;
	
	//rearrange_array(m_image_o->YUV_image_in.Y_buff); // fill image_o_blocks, image_o_num_of_blocks
	
	/* calculate num_of_windows */
	// block change every 1 pixel - (block_hight-1)*windows_width
	no_of_pixels_win = (m_win_conf->window_hight *m_win_conf->window_width)-((m_win_conf->block_hight-1) * m_win_conf->window_width) ; 
	no_of_windows_in_width = (m_next_image->pixels_width / m_win_conf->window_width) + (2*(uint32_t)(m_win_conf->window_width/2));
	no_of_windows_in_hight = m_next_image->pixels_hight / m_win_conf->window_hight + (2 * (uint32_t)(m_win_conf->window_hight / 2));
	no_of_windows = no_of_windows_in_hight * no_of_windows_in_hight;
	no_of_blocks_win_width = m_win_conf->window_width / m_win_conf->block_width;
	no_of_blocks_win_hight = m_win_conf->window_hight / m_win_conf->block_hight;
	no_of_blocks_win = no_of_blocks_win_width * no_of_blocks_win_hight;
	block_size = m_win_conf->block_hight * m_win_conf->block_width;
	
	int16_t* middle_block = new int16_t [block_size];
	int16_t* other_block = new int16_t [block_size];
	motion_vector.block_motion_vector_array = new block_motion_vector_t[no_of_windows];
	motion_vector.number_of_vectors = no_of_windows;

	/* loop over windows in the m_next_image*/
	uint32_t i = 0;
	uint32_t middle_index= 0;
	uint32_t win_index =0;
	for (i = 0; i < no_of_windows; i++)
	{
		/*get windows index (anchor top-left)*/
		win_index = get_win_index(i);
		/*get the middle block id - to access in image_o_blocks*/
		middle_index = get_middle_block_index(win_index);
		if (middle_index == 0)
		{ // wrong index not found 
			_ASSERT(false);
		}

		/*loop on blocks in the window - blocks move 1 pixel*/
		uint32_t j = 0;
		float_t Error =0;
		float_t min_Error = 0xFFFFFFFF;
		motion_vector.block_motion_vector_array[i].original_block_index = middle_index;
		motion_vector.block_motion_vector_array[i].windows_index = win_index;
		motion_vector.block_motion_vector_array[i].distance = 0xFFFF;
		for (j = 0; j < no_of_pixels_win;j++)
		{
			/*get blocks given pixel index of top left array*/
			get_block(win_index,middle_block,m_image_o);
			get_block(win_index,other_block,m_next_image);

			/*compare the two arrays using MAD_Calc or MSE_Calc or PSNR_Calc*/
			switch (win_conf->matching_type)
			{
			case MAD:
				Error = MAD_Calc(middle_block,other_block);
				break;
			case PSNR:
				Error = PSNR_Calc(middle_block,other_block);
				break;

			default:
				_ASSERT(false);
				break;
			}
			/*update motion vector for this block depending on output */
			if(Error<min_Error) {
				/* calculate x, y components from j and window size */
				motion_vector.block_motion_vector_array[i].block_index = get_block_index(win_index, j);
				motion_vector.block_motion_vector_array[i].distance  = get_distance_between_blocks(j, no_of_pixels_win/2);
			}
			else if(Error=min_Error) {
				uint32_t distance;
				/* calculate distance to middle block */
				distance = get_distance_between_blocks(j, no_of_pixels_win/2);
				if (distance < motion_vector.block_motion_vector_array[i].distance){
					motion_vector.block_motion_vector_array[i].block_index = get_block_index(win_index, j);
					motion_vector.block_motion_vector_array[i].distance  = distance;
				}
			}
		}
	}

};


uint32_t window::get_win_index(uint32_t window_index) {
	uint32_t index = 0;

	index = (window_index % no_of_windows_in_width) * m_win_conf->window_width + 
	((window_index - (window_index % no_of_windows_in_width))/no_of_windows_in_width) *
		m_win_conf->window_hight *m_next_image->pixels_width;

	return index;
}

uint32_t window::get_middle_block_index(uint32_t window_index) {
	uint32_t index = 0;
	uint32_t middle_h_off, middle_w_off;

	middle_w_off = no_of_blocks_win_width/2;
	middle_h_off = no_of_blocks_win_hight/2;
	index = window_index + (middle_w_off*m_win_conf->block_width) + 
	(middle_h_off * m_win_conf->block_hight * m_next_image->pixels_width);
	return index;
}
uint32_t window::get_block_index(uint32_t window_index, uint32_t index_inside_win) {
	uint32_t index = 0;

	index = window_index + (index_inside_win%m_win_conf->window_width) +
		((index_inside_win-(index_inside_win%m_win_conf->window_width)/m_win_conf->window_width) * m_next_image->pixels_width);
	return index;
}

void window::get_block(uint32_t offset, int16_t * block, image* image_input) {
	uint32_t index = 0;
	uint32_t i;
	uint32_t block_col = -1;
	for (i=0; i<block_size; i++)
	{
		if (i%block_size){
			block_col++; 
		}
		index = offset + (i%block_size) + (block_col*image_input->pixels_width);
		block[i] = image_input->YUV_image_in.Y_buff[index];
	}
}

uint32_t window::get_distance_between_blocks(uint32_t index_1,uint32_t index_2){
	int32_t x1, x2, y1, y2;
	uint32_t distance;
	x1 = index_1%m_win_conf->window_width;
	y1 = ((index_1 - index_1%m_win_conf->window_width)/m_win_conf->window_width); 
	x2 = index_2%m_win_conf->window_width;
	y2 = ((index_2 - index_2%m_win_conf->window_width)/m_win_conf->window_width);
	distance = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
	return distance;
}

float_t window::MAD_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_win_conf->block_hight * m_win_conf->block_width;
	uint32_t i, sum=0;
	float_t MAD;
	for (i = 0; i < block_size; i++)
	{
		sum += abs(image_block[i] - next_image_block[i]);
	}
		MAD = (1 / block_size) * sum;
		return MAD;
}

float_t window::MSE_Calc(int16_t* image_block, int16_t* next_image_block) {
	uint32_t block_size = m_win_conf->block_hight * m_win_conf->block_width;
	uint32_t i, sum = 0;
	float_t MSE;
	for (i = 0; i < block_size; i++)
	{
		sum += (image_block[i] - next_image_block[i])* (image_block[i] - next_image_block[i]);
	}
	MSE = (1 / block_size) * sum;
	return MSE;
}

float_t window::PSNR_Calc(int16_t* image_block, int16_t* next_image_block) {
	float_t MSE;
	MSE = MSE_Calc(image_block, next_image_block);
	return (10 * log10((255 * 255) / MSE));
}