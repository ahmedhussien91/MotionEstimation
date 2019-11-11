#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>

typedef struct image_conf_t {
	const char *file_path;
	const char* ImageType;
};

typedef struct {
	int16_t* Y_buff;
	int16_t* U_buff;
	int16_t* V_buff;
	uint32_t Y_size_bytes;
	uint32_t U_size_bytes;
	uint32_t V_size_bytes;
	uint32_t pixels_hight;
	uint32_t pixels_width;

}YUV_Image_t;

typedef struct
{
	uint8_t* R_buff;
	uint8_t* G_buff;
	uint8_t* B_buff;
	uint32_t pixels_hight;
	uint32_t pixels_width;
	uint8_t  pixel_size;
}RGB_Image_t;

class image
{
public:
	image(RGB_Image_t RGB_image) {
		uint32_t image_size = RGB_image.pixels_hight * RGB_image.pixels_width;
		RGB_image.R_buff = new uint8_t[image_size];
		RGB_image.G_buff = new uint8_t[image_size];
		RGB_image.B_buff = new uint8_t[image_size];
	};
	image(YUV_Image_t YUV_image) {
		uint32_t image_size = YUV_image.pixels_hight * YUV_image.pixels_width;
		YUV_image.Y_buff = new int16_t[image_size];
		YUV_image.U_buff = new int16_t[image_size];
		YUV_image.V_buff = new int16_t[image_size];
	}
	;

	image(image_conf_t image_conf);
	void convertRGBtoYUV();
	void convertYUVtoRGB();	
	
	RGB_Image_t RGB_image_in;
	RGB_Image_t RGB_image_out;

	YUV_Image_t YUV_image_in;
	YUV_Image_t YUV_image_out;

	uint16_t pixels_width;
	uint16_t pixels_hight;
	uint8_t pixel_size;
private:
	char ImR_ImageRead(image_conf_t image_conf);
};

