#include "util.h"

void util::write_raw_image(RGB_Image_t imageinfo, const char* filename)
{
	FILE* file;
	file = fopen(filename, "wb");
	uint32_t image_size = imageinfo.pixels_hight * imageinfo.pixels_width * 3;
	uint8_t* buff = new uint8_t[image_size];
	uint32_t idx = 0;
	for (uint32_t i = 0; i < image_size; i = i + 3)
	{
		buff[i] = imageinfo.R_buff[idx];
		buff[i + 1] = imageinfo.G_buff[idx];
		buff[i + 2] = imageinfo.B_buff[idx];
		idx++;
	}
	fwrite(buff, image_size, 1, file);
	fclose(file);
}

void util::write_raw_image(YUV_Image_t imageinfo, const char* filename)
{
	FILE* file;
	file = fopen(filename, "wb");
	uint32_t image_size = imageinfo.pixels_hight * imageinfo.pixels_width * 3;
	uint8_t* buff = new uint8_t[image_size];
	uint32_t idx = 0;
	for (uint32_t i = 0; i < image_size; i = i + 3)
	{
		buff[i] = imageinfo.Y_buff[idx];
		buff[i + 1] = imageinfo.U_buff[idx];
		buff[i + 2] = imageinfo.V_buff[idx];
		idx++;
	}
	fwrite(buff, image_size, 1, file);
	fclose(file);
}

/*void util::Display_raw(ImageInfo_t* imageinfo, const char* info)
{
	// read an image
	cv::Mat image;
	if ((imageinfo.pixel_size / 8) == 3)
		image = cv::Mat(imageinfo.hight, imageinfo.width, CV_8UC3, (uint8_t*)imageinfo.buff);
	else if ((imageinfo.pixel_size / 8) == 1)
		image = cv::Mat(imageinfo.hight, imageinfo.width, CV_8UC1, (uint8_t*)imageinfo.buff);
	// create image window named "My Image"
	cv::namedWindow(info, 0);
	// show the image on window
	cv::imshow("My Image", image);
	// wait key for 5000 ms
	cv::waitKey(5000);

}*/

void util::calculateSNR(RGB_Image_t Original_image, RGB_Image_t Processed_Image, const char* info)
{
	uint32_t sumOfSquareError = 0, MeanSquareError = 0;
	int32_t tmp = 0, tmp1 = 0, tmp2 = 0;
	uint32_t Component_size = Original_image.pixels_hight * Original_image.pixels_width;
	for (uint32_t i = 0; i < Component_size; i++)
	{
		tmp = (int32_t)Original_image.R_buff[i] - (uint32_t)Processed_Image.R_buff[i];
		if (tmp >= 0) {
			tmp = tmp;
		}
		else {
			tmp = -1 * tmp;
		}
		tmp1 = (int32_t)Original_image.G_buff[i] - (uint32_t)Processed_Image.G_buff[i];
		if (tmp1 >= 0) {
			tmp1 = tmp1;
		}
		else {
			tmp1 = -1 * tmp1;
		}
		tmp2 = (int32_t)Original_image.B_buff[i] - (uint32_t)Processed_Image.B_buff[i];
		if (tmp2 >= 0) {
			tmp2 = tmp2;
		}
		else {
			tmp2 = -1 * tmp2;
		}
		sumOfSquareError = (tmp * tmp) + (tmp1 * tmp1) + (tmp2 * tmp2) + sumOfSquareError;
	}
	MeanSquareError = 10 * log10((255 * 255) / (sumOfSquareError / ((double)Original_image.pixels_hight * Original_image.pixels_width * 3)));
	std::cout << "MeanSquareError of " << info << ": " << MeanSquareError << "\n";
}

void util::calculateSNR(YUV_Image_t Original_image, YUV_Image_t Processed_Image, const char* info)
{
	uint32_t sumOfSquareError = 0, MeanSquareError = 0;
	int32_t tmp = 0, tmp1 = 0, tmp2 = 0;
	uint32_t Component_size = Original_image.pixels_hight * Original_image.pixels_width;
	for (uint32_t i = 0; i < Component_size; i++)
	{
		tmp = (int32_t)Original_image.Y_buff[i] - (uint32_t)Processed_Image.Y_buff[i];
		if (tmp >= 0) {
			tmp = tmp;
		}
		else {
			tmp = -1 * tmp;
		}
		tmp1 = (int32_t)Original_image.U_buff[i] - (uint32_t)Processed_Image.U_buff[i];
		if (tmp1 >= 0) {
			tmp1 = tmp1;
		}
		else {
			tmp1 = -1 * tmp1;
		}
		tmp2 = (int32_t)Original_image.V_buff[i] - (uint32_t)Processed_Image.V_buff[i];
		if (tmp2 >= 0) {
			tmp2 = tmp2;
		}
		else {
			tmp2 = -1 * tmp2;
		}
		sumOfSquareError = (tmp * tmp) + (tmp1 * tmp1) + (tmp2 * tmp2) + sumOfSquareError;
	}
	MeanSquareError = 10 * log10((255 * 255) / (sumOfSquareError / ((double)Original_image.pixels_hight * Original_image.pixels_width * 3)));
	std::cout << "MeanSquareError of " << info << ": " << MeanSquareError << "\n";
}