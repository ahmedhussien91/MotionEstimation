#include "image.h"


#  define BF_TYPE 0x4D42             /* "MB" */

#pragma pack(2)
typedef struct                       /**** BMP file header structure ****/
{
	uint16_t	bfType;           /* Magic number for file */
	uint32_t	bfSize;           /* Size of file */
	uint16_t	bfReserved1;      /* Reserved */
	uint16_t	bfReserved2;      /* ... */
	uint32_t	bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;



typedef struct                       /**** BMP file info structure ****/
{
	uint32_t	biSize;           /* Size of info header */
	int32_t		biWidth;          /* Width of image */
	int32_t		biHeight;         /* Height of image */
	uint16_t	biPlanes;         /* Number of color planes */
	uint16_t	biBitCount;       /* Number of bits per pixel */
	uint32_t	biCompression;    /* Type of compression to use */
	uint32_t	biSizeImage;      /* Size of image data */
	int32_t		biXPelsPerMeter;  /* X pixels per meter */
	int32_t		biYPelsPerMeter;  /* Y pixels per meter */
	uint32_t	biClrUsed;        /* Number of colors used */
	uint32_t	biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;
#pragma pack()
/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

uint8_t* buff;
BITMAPFILEHEADER fh;
BITMAPINFOHEADER ih;
uint32_t image_hight_pixels;
uint32_t image_width_pixels;
uint16_t image_pixel_size;
uint32_t image_size_bytes;




image::image(image_conf_t image_conf)
{
	ImR_ImageRead(image_conf);
	RGB_image_in.pixels_hight = pixels_hight;
	RGB_image_in.pixels_width = pixels_width;
	RGB_image_in.pixel_size = pixel_size;

	RGB_image_out.pixels_hight = pixels_hight;
	RGB_image_out.pixels_width = pixels_width;
	RGB_image_out.pixel_size = pixel_size;

	YUV_image_in.pixels_hight = pixels_hight;
	YUV_image_in.pixels_width = pixels_width;

	YUV_image_out.pixels_hight = pixels_hight;
	YUV_image_out.pixels_width = pixels_width;
}

char image::ImR_ImageRead(image_conf_t image_conf)
{
	FILE* file;
	file = fopen(image_conf.file_path, "rb");

	if (file != NULL) { // file opened
		size_t x = fread(&fh, sizeof(BITMAPFILEHEADER), 1, file); //reading the FILEHEADER
		x = fread(&ih, sizeof(BITMAPINFOHEADER), 1, file);
		// create Buffer
		image_size_bytes = (ih.biHeight * ih.biWidth * ih.biBitCount + ((ih.biHeight * ih.biWidth * ih.biBitCount) % 32)) / 8;
		buff = new uint8_t[image_size_bytes];
		image_hight_pixels = ih.biHeight;
		image_width_pixels = ih.biWidth;
		image_pixel_size = ih.biBitCount;
		x = fread(buff, image_size_bytes, 1, file);

		fclose(file);
	}

	uint32_t idx = 0;
	pixel_size = image_pixel_size;
	if (3 == (pixel_size / 8))
	{
		RGB_image_in.R_buff = new uint8_t[image_size_bytes / 3];
		RGB_image_in.G_buff = new uint8_t[image_size_bytes / 3];
		RGB_image_in.B_buff = new uint8_t[image_size_bytes / 3];

		RGB_image_out.R_buff = new uint8_t[image_size_bytes / 3];
		RGB_image_out.G_buff = new uint8_t[image_size_bytes / 3];
		RGB_image_out.B_buff = new uint8_t[image_size_bytes / 3];

		YUV_image_in.Y_buff = new int16_t[image_size_bytes / 3];
		YUV_image_in.U_buff = new int16_t[image_size_bytes / 3];
		YUV_image_in.V_buff = new int16_t[image_size_bytes / 3];

		YUV_image_out.Y_buff = new int16_t[image_size_bytes / 3];
		YUV_image_out.U_buff = new int16_t[image_size_bytes / 3];
		YUV_image_out.V_buff = new int16_t[image_size_bytes / 3];
		for (uint32_t i = 0; i < image_size_bytes; i = i + 3)
		{
			RGB_image_in.R_buff[idx] = buff[i + 2];
			RGB_image_in.G_buff[idx] = buff[i + 1];
			RGB_image_in.B_buff[idx] = buff[i];
			idx++;
		}
		pixels_hight = image_hight_pixels;
		pixels_width = image_width_pixels;
	}
	else if (1 == (pixel_size / 8))
	{

		RGB_image_in.R_buff = new uint8_t[image_size_bytes];
		RGB_image_in.G_buff = new uint8_t[image_size_bytes];
		RGB_image_in.B_buff = new uint8_t[image_size_bytes];

		RGB_image_out.R_buff = new uint8_t[image_size_bytes];
		RGB_image_out.G_buff = new uint8_t[image_size_bytes];
		RGB_image_out.B_buff = new uint8_t[image_size_bytes];

		YUV_image_in.Y_buff = new int16_t[image_size_bytes];
		YUV_image_in.U_buff = new int16_t[image_size_bytes];
		YUV_image_in.V_buff = new int16_t[image_size_bytes];

		YUV_image_out.Y_buff = new int16_t[image_size_bytes];
		YUV_image_out.U_buff = new int16_t[image_size_bytes];
		YUV_image_out.V_buff = new int16_t[image_size_bytes];
		for (uint32_t i = 0; i < image_size_bytes; i = i + 1)
		{
			RGB_image_in.R_buff[idx] = buff[i];
			RGB_image_in.G_buff[idx] = buff[i];
			RGB_image_in.B_buff[idx] = buff[i];
			idx++;
		}
		pixels_hight = image_hight_pixels;
		pixels_width = image_width_pixels;
	}
	else {// unhandled pixel size 
		_ASSERT(false);
	}

	delete buff;

	return 1;
}

void image::convertRGBtoYUV() {
	uint32_t Image_size = 0;
	Image_size = pixels_hight * pixels_width;

	YUV_image_in.Y_size_bytes = Image_size;
	YUV_image_in.U_size_bytes = Image_size;
	YUV_image_in.V_size_bytes = Image_size;

	for (uint32_t i = 0; i < Image_size; i = i + 1)
	{
		int16_t tmp1, tmp2, tmp3;
		float R = RGB_image_in.R_buff[i];
		float G = RGB_image_in.G_buff[i];
		float B = RGB_image_in.B_buff[i];
		tmp1 = (16 + (int16_t)(0.25679 * R + 0.50413 * G + 0.0979 * B));//(0.257 * R + 0.504 * G + 0.098 * B + 16); //y
		tmp2 = (128 + (int16_t)(-0.14822 * R - 0.29099 * G + 0.43921 * B));//(-0.148 * R - 0.291 * G + 0.439 * B + 128); //u
		tmp3 = (128 + (int16_t)(0.43921 * R - 0.36779 * G - 0.07143 * B));//(0.439 * R - 0.368 * G - 0.071 * B + 128); //v

		YUV_image_in.Y_buff[i] = (int16_t)tmp1;
		YUV_image_in.U_buff[i] = (int16_t)tmp2;
		YUV_image_in.V_buff[i] = (int16_t)tmp3;
	}
}

void image::convertYUVtoRGB()
{
	uint32_t Image_size = 0;
	Image_size = pixels_hight * pixels_width;

	for (uint32_t i = 0; i < Image_size; i = i + 1)
	{
		int16_t tmp1, tmp2, tmp3;
		float Y = YUV_image_in.Y_buff[i];
		float U = YUV_image_in.U_buff[i];
		float V = YUV_image_in.V_buff[i];
		tmp1 = ((int16_t)(1.16438 * Y + 1.596 * V)) - 222.921;//(1.164*Y + 1.596*V);
		tmp2 = ((int16_t)(1.16438 * Y - 0.39176 * U - 0.81296 * V)) + 135.576;//(1.164*Y - 0.392*U - 0.813*V);
		tmp3 = ((int16_t)(1.16438 * Y + 2.0172 * U)) - 276.836;//(1.164*Y + 2.017*U);
		if (tmp1 >= 255)
			RGB_image_out.R_buff[i] = (uint8_t)255;
		else if (tmp1 <= 0)
			RGB_image_out.R_buff[i] = (uint8_t)0;
		else
			RGB_image_out.R_buff[i] = (uint8_t)tmp1;

		if (tmp2 >= 255)
			RGB_image_out.G_buff[i] = (uint8_t)255;
		else if (tmp2 <= 0)
			RGB_image_out.G_buff[i] = (uint8_t)0;
		else
			RGB_image_out.G_buff[i] = (uint8_t)tmp2;

		if (tmp3 >= 255)
			RGB_image_out.B_buff[i] = (uint8_t)255;
		else if (tmp3 <= 0)
			RGB_image_out.B_buff[i] = (uint8_t)0;
		else
			RGB_image_out.B_buff[i] = (uint8_t)tmp3;
	}

}
