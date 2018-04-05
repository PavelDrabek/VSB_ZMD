// StereoMatching.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void disparity(Mat& left, Mat& right) {

}

int hammingDist(Mat& left, Point2i lp, Mat& right, Point2i rp, vector<Point2i>& mask) 
{
	int dist = 0;
	for (size_t i = 0; i < mask.size(); i++)
	{
		if (left.at<uchar>(lp) < left.at<uchar>(lp + mask[i]) != right.at<uchar>(rp) < right.at<uchar>(rp + mask[i])) {
			dist++;
		}
	}
	return dist;
}

double nnc_val(Mat& left, Point2i lp, Mat& right, Point2i rp, vector<Point2i>& mask)
{
	double top = 0;
	double ldown = 0;
	double rdown = 0;

	for (size_t i = 0; i < mask.size(); i++)
	{
		int T = left.at<uchar>(lp + mask[i]);
		int I = right.at<uchar>(rp + mask[i]);

		top += (T * I);
		ldown += (T * T);
		rdown += (I * I);
	}
	return top / sqrt(ldown * rdown);
}

int main(int argc, const char * argv[]) 
{
	
	string pathLeft = "../data/stereomatching/cones-im6.png";
	string pathRight = "../data/stereomatching/cones-im2.png";

	Mat left = imread(pathLeft, 0);
	Mat right = imread(pathRight, 0);
	Mat res = imread("../data/stereomatching/cones-disp2.png", 0);
	Mat disp = Mat(left.size(), left.type());
	Mat ncc = Mat(left.size(), left.type());
	Mat census = Mat(left.size(), left.type());

	imshow("left", left);
	imshow("right", right);
	imshow("res", res);
	waitKey(30);

	int maskExt = 2;
	int maskSize = 2 * maskExt + 1;
	vector<Point2i> shiftMask;
	for (int x = -maskExt; x <= maskExt; x++) {
		for (int y = -maskExt; y <= maskExt; y++) {
			shiftMask.push_back(Point2i(x, y));
		}
	}

	Point2i o, os;


	//cv::Mat output;
	//cv::matchTemplate(right, left, output, CV_TM_CCORR_NORMED);
	//float nccDiff = output.at<float>(0, 0);
	//imshow("output", output);
	//waitKey(30);

	//float nahore = cv::sum(right.mul(left))[0];
	//float dole = cv::sqrt(squareSumToFind * (cv::sum(right.mul(right))[0]));
	//float cnnDiff = nahore / dole;

	for (size_t r = maskExt; r < left.rows - maskExt; r++) {
		for (size_t c = maskExt; c < left.cols - maskExt; c++) {
			o = Point2i(c, r);
			int minSum = 9999999;
			int minC = c;

			for (size_t s = c; s < left.cols - maskExt && s < c + 64; s++)
			{
				float sum = 0;
				os = Point2i(s, r);
				for (size_t i = 0; i < shiftMask.size(); i++)
				{
					sum += abs(left.at<uchar>(o + shiftMask[i]) - right.at<uchar>(os + shiftMask[i]));
				}
				if (sum < minSum) {
					minSum = sum;
					minC = s;
				}
			}

			uchar aa = (uchar)(minC - c);
			disp.at<uchar>(o) = aa * 4;
		}

		if (r % 10 == 0) {
			imshow("disparity", disp);
			waitKey(10);
		}
	}

	// NCC
	for (size_t r = maskExt; r < left.rows - maskExt; r++) {
		for (size_t c = maskExt; c < left.cols - maskExt; c++) {
			o = Point2i(c, r);
			double minSum = 0;
			int minC = c;

			for (size_t s = c; s < left.cols - maskExt && s < c + 64; s++)
			{
				os = Point2i(s, r);
				double val = nnc_val(left, o, right, os, shiftMask);

				if (val > minSum) {
					minSum = val;
					minC = s;
				}
			}

			uchar aa = (uchar)(minC - c);
			ncc.at<uchar>(o) = aa * 4;
		}

		if (r % 10 == 0) {
			imshow("ncc", ncc);
			waitKey(10);
		}
	}

	// Census
	for (size_t r = maskExt; r < left.rows - maskExt; r++) {
		for (size_t c = maskExt; c < left.cols - maskExt; c++) {
			o = Point2i(c, r);
			int minSum = 9999999;
			int minC = c;

			for (size_t s = c; s < left.cols - maskExt && s < c + 64; s++)
			{
				os = Point2i(s, r);
				int H_dist = hammingDist(left, o, right, os, shiftMask);

				if (H_dist < minSum) {
					minSum = H_dist;
					minC = s;
				}
			}

			uchar aa = (uchar)(minC - c);
			census.at<uchar>(o) = aa * 4;
		}

		if (r % 10 == 0) {
			imshow("census", census);
			waitKey(10);
		}
	}

	imshow("disparity", disp);
	imshow("census", census);
	imshow("ncc", ncc);
	waitKey(0);

}

