#pragma once
#include "image.h"

class util
{
public:
	util() {};
	void write_raw_image(RGB_Image_t imageinfo, const char* filename);
	void write_raw_image(YUV_Image_t imageinfo, const char* filename);
	//void Display_raw(ImageInfo_t* imageinfo, const char* info);
	void calculateSNR(RGB_Image_t Original_image, RGB_Image_t Processed_Image, const char* info);
	void calculateSNR(YUV_Image_t Original_image, YUV_Image_t Processed_Image, const char* info);
};

