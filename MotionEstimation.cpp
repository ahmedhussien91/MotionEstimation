// MotionEstimation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "image.h"
#include "util.h"
#include "window.h"


int main()
{
	image_conf_t conf_1 = { "color_pencils_big_frame_on_dark/0017/0017.bmp", "bmp" };
	image image_1 = image(conf_1);
	image_conf_t conf_2 = { "color_pencils_big_frame_on_dark/0017/0018.bmp", "bmp" };
	image image_2 = image(conf_2);

	image_1.convertRGBtoYUV();
	image_2.convertRGBtoYUV();

	image_1.convertYUVtoRGB();
	image_2.convertYUVtoRGB();

	util util_1;
	util_1.calculateSNR(image_1.RGB_image_in, image_1.RGB_image_out, "color_pencils_big_frame_on_dark0006"); 
	//util_1.calculateSNR(image_1.YUV_image_in, image_1.YUV_image_out, "barbara_gray YUV");
	util_1.calculateSNR(image_2.RGB_image_in, image_2.RGB_image_out, "color_pencils_big_frame_on_dark0007");
	//util_1.calculateSNR(image_2.YUV_image_in, image_2.YUV_image_out, "lena_color YUV");
	util_1.write_raw_image(image_2.RGB_image_out, "color_pencils_big_frame_on_dark0007.raw");
	util_1.write_raw_image(image_1.RGB_image_out, "color_pencils_big_frame_on_dark0006.raw");
	      
	window_conf_t win_conf; 
	win_conf.block_width = 9;
	win_conf.block_hight = 8;
	win_conf.window_width = 5*9; // multiple of block width
	win_conf.window_hight = 8*8; // multiple of block hight 
	win_conf.matching_type = MAD;

	window window1 = window(&image_1, &image_2, &win_conf);
	//reconstruct from motion vector only
	YUV_Image_t OutputImage;
	OutputImage.pixels_hight = image_1.pixels_hight;
	OutputImage.pixels_width = image_1.pixels_width;
	image motion_vector_image = image(OutputImage);
	uint32_t idx;
	for (idx = 0; idx < window1.motion_vector.number_of_vectors; idx++)
	{
		window1.write_block(window1.motion_vector.block_motion_vector_array[idx].block_index, window1.motion_vector.block_motion_vector_array[idx].original_block_index, &motion_vector_image, &image_2);
	}
	motion_vector_image.convertYUVtoRGB();
	util_1.write_raw_image(motion_vector_image.RGB_image_out, "motion_vector_image.raw");
	

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
