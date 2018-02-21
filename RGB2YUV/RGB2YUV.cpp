// RGB2YUV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace cv;
using namespace std;

Mat matrix_rgb2yuv = (Mat_<float>(3, 3) <<
	0.299, 0.587, 0.114,
	-0.14713, -0.28886, 0.436,
	0.615, -0.51499, -0.10001);

Mat matrix_yuv2rgb = (Mat_<float>(3, 3) <<
	1, 0, 1.13983,
	1, -0.39465, -0.5806,
	1, 2.03211, 0);

void rgb2yuv(Mat& input, Mat& output)
{
	for (size_t y = 0; y < input.rows; y++)
	{
		for (size_t x = 0; x < input.cols; x++)
		{
			Vec3f rgb = input.at<Vec3f>(y, x);
			Vec3f yuv = Vec3f((Mat)(matrix_rgb2yuv * Mat(rgb)));
			output.at<Vec3f>(y, x) = yuv;
		}
	}
}

void yuv2rgb(Mat& input, Mat& output)
{
	for (size_t y = 0; y < input.rows; y++)
	{
		for (size_t x = 0; x < input.cols; x++)
		{
			Vec3f yuv = input.at<Vec3f>(y, x);
			Vec3f rgb = Vec3f((Mat)(matrix_yuv2rgb * Mat(yuv)));
			output.at<Vec3f>(y, x) = rgb;
		}
	}
}

int main()
{
	Mat rgb = imread("../data/image_rgb.jpg", 1);
	Mat yuv(rgb.size(), CV_32FC3), rgb2(rgb.size(), CV_32FC3);

	rgb.convertTo(rgb, CV_32FC3, 1.0/255); 
	imshow("RGB", rgb);
	waitKey(10);

	rgb2yuv(rgb, yuv);
	imshow("YUV", yuv);
	waitKey(10);

	yuv2rgb(yuv, rgb2);
	imshow("RGB Reconstruction", rgb2);

	waitKey(0);

    return 0;
}
