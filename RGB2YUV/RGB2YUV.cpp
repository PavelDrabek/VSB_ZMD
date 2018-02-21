// RGB2YUV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace cv;
using namespace std;

void rgb2yuv(Mat& input, Mat& output)
{

}

void yuv2rgb(Mat& input, Mat& output)
{

}

int main()
{
	Mat rgb = imread("../data/image_rgb.jpg", 1);
	Mat yuv, rgb2;

	imshow("RGB", rgb);

	rgb2yuv(rgb, rgb);
	imshow("YUV", yuv);
	
	yuv2rgb(rgb, rgb);
	imshow("RGB Reconstruction", rgb2);

	waitKey(0);

    return 0;
}
