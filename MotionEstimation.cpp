// MotionEstimation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "image.h"
#include "util.h"


int main()
{
	image_conf_t conf_1 = { "barbara_gray.bmp", "bmp" };
	image image_1 = image(conf_1);
	image_conf_t conf_2 = { "lena_color.bmp", "bmp" };
	image image_2 = image(conf_2);

	image_1.convertRGBtoYUV();
	image_2.convertRGBtoYUV();

	image_1.convertYUVtoRGB();
	image_2.convertYUVtoRGB();


	util util_1;

	util_1.calculateSNR(image_1.RGB_image_in, image_1.RGB_image_out, "barbara_gray RGB"); 
	//util_1.calculateSNR(image_1.YUV_image_in, image_1.YUV_image_out, "barbara_gray YUV");
	util_1.calculateSNR(image_2.RGB_image_in, image_2.RGB_image_out, "lena_color RGB");
	//util_1.calculateSNR(image_2.YUV_image_in, image_2.YUV_image_out, "lena_color YUV");
	

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
