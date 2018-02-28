// HDR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

vector<Mat> imagesc3;
vector<Mat> imagesc1;
vector<Mat> weights;
Mat weightSum;

const float mi = 0.5f;
const float ro = 0.2f;

#define SQR(x) ((x)*(x))

int main()
{
	imagesc3.push_back(imread("../data/hdr/s1_0.png", 1));
	imagesc3.push_back(imread("../data/hdr/s1_1.png", 1));
	imagesc3.push_back(imread("../data/hdr/s1_2.png", 1));
	imagesc3.push_back(imread("../data/hdr/s1_3.png", 1));
	imagesc3.push_back(imread("../data/hdr/s1_4.png", 1));

	for (size_t i = 0; i < imagesc3.size(); i++)
	{
		Mat gray;
		cvtColor(imagesc3[i], gray, cv::COLOR_BGR2GRAY);
		imagesc1.push_back(gray);

		cout << gray.type() << endl;

		weights.push_back(Mat(gray.size(), CV_32F));
	}

	size_t rows = imagesc1[0].rows;
	size_t cols = imagesc1[0].cols;
	int n = imagesc1.size();

	// calc weights
	weightSum = Mat(rows, cols, CV_32FC1);
	for (size_t r = 0; r < rows; r++)
	{
		for (size_t c = 0; c < cols; c++)
		{
			float wsum = 0;
			for (size_t i = 0; i < imagesc1.size(); i++)
			{
				uchar v = imagesc1[i].at<uchar>(r, c);
				float top = v - mi * 255;
				float bottom = ro * 255;
				float x = exp( SQR(top) / (2 * SQR(bottom)) );

				weights[i].at<float>(r, c) = x;
				wsum += x;
			}
			weightSum.at<float>(r, c) = wsum;
			
			// normalize
			for (size_t i = 0; i < weights.size(); i++)
			{
				weights[i].at<float>(r, c) /= weightSum.at<float>(r, c);
			}
		}
	}

	Mat outputc1(rows, cols, CV_8UC1);
	Mat outputc3(rows, cols, CV_8UC3);
	for (size_t r = 0; r < rows; r++)
	{
		for (size_t c = 0; c < cols; c++)
		{
			outputc1.at<uchar>(r, c) = 0;
			outputc3.at<Vec3b>(r, c) = Vec3b(0,0,0);

			for (size_t i = 0; i < imagesc1.size(); i++)
			{
				uchar c1 = imagesc1[i].at<uchar>(r, c);
				Vec3b c3 = imagesc3[i].at<Vec3b>(r, c);

				float w = weights[i].at<float>(r, c);
				c1 *= w;
				c3 *= w;

				outputc1.at<uchar>(r, c) += c1;
				outputc3.at<Vec3b>(r, c) += c3;
			}
		}
	}

	imshow("C3", outputc3);
	imshow("C1", outputc1);

	Mat o1 = imread("../data/hdr/hdr_c1.png", 0);
	Mat o3 = imread("../data/hdr/hdr_c3.png", 1);

	imshow("C3 - orig", o3);
	imshow("C1 - orig", o1);


	waitKey(0);

    return 0;
}

