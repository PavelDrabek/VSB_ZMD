// RGB2YUV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace cv;
using namespace std;

vector<Point2i> plus_positions({ Point2i(0,1), Point2i(0,-1), Point2i(-1,0), Point2i(1,0) });
vector<Point2i> cross_positions({ Point2i(-1,1), Point2i(1,1), Point2i(-1,-1), Point2i(1,-1) });
vector<Point2i> col_positions({ Point2i(0,1), Point2i(0,-1) });
vector<Point2i> row_positions({ Point2i(-1,0), Point2i(1,0) });

uchar getMean(Mat& mat, vector<Point2i>& shifts, Point2i pos)
{
	int count = 0;
	int px = 0;

	for (size_t i = 0; i < shifts.size(); i++)
	{
		Point2i p = pos + shifts[i];
		if (p.x >= 0 && p.x < mat.cols && p.y >= 0 && p.y < mat.rows) {
			px += mat.at<uchar>(p);
			count++;
		}
	}

	return px / count;
}

void bayer2rgb(Mat& input, Mat& output)
{
	/*
	Bayer pattern
	R G R G ...
	G B G B ...
	R G R G ...
	...
	*/

	output = Mat(input.size(), CV_8UC3);

	for (size_t y = 0; y < input.rows; y++)
	{
		for (size_t x = 0; x < input.cols; x++)
		{
			uchar r, g, b;
			uchar px = input.at<uchar>(y, x);
			Point2i p(x, y);
			if (y % 2 == 0) {
				if (x % 2 == 0) { // R
					r = px;
					g = getMean(input, plus_positions, p);
					b = getMean(input, cross_positions, p);
				}
				else { // G
					r = getMean(input, col_positions, p);
					g = px;
					b = getMean(input, row_positions, p);
				}
			}
			else {
				if (x % 2 == 0) { // G
					r = getMean(input, col_positions, p);
					g = px;
					b = getMean(input, row_positions, p);
				}
				else { // B
					r = getMean(input, cross_positions, p);
					g = getMean(input, plus_positions, p);
					b = px;
				}
			}

			output.at<Vec3b>(p) = Vec3b(b, g, r);
		}
	}
}

int main()
{
	Mat bayer = imread("../data/bayer.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	cout << bayer.type() << endl;

	imshow("Bayer", bayer);
	Mat rgb;
	bayer2rgb(bayer, rgb);
	imshow("RGB", rgb);

	waitKey(0);

	return 0;
}
