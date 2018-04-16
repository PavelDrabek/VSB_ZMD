#include "stdafx.h"

#define MARKER_BINS 7
#define BIN_SIZE 8
#define MARKER_SIZE (MARKER_BINS*BIN_SIZE)
#define T_AREA 1000

const std::vector<cv::Point2f> MBAR::corners_ = { cv::Point( 0, 0 ),
	cv::Point( MARKER_SIZE - 1, 0 ), cv::Point( MARKER_SIZE - 1, MARKER_SIZE - 1 ),
	cv::Point( 0, MARKER_SIZE - 1 ) };

const std::vector<cv::Point3f> MBAR::corners_ws_ = { cv::Point3f( -3.0f, 3.0f, 0.0f ),
cv::Point3f( 3.0f, 3.0f, 0.0f ), cv::Point3f( 3.0f, -3.0f, 0.0f ), cv::Point3f( -3.0f, -3.0f, 0.0f ) };

const std::vector<cv::Point3f> MBAR::axis_ws_ = { cv::Point3f( 0.0f, 0.0f, 0.0f ),
cv::Point3f( 1.0f, 0.0f, 0.0f ), cv::Point3f( 0.0f, 1.0f, 0.0f ), cv::Point3f( 0.0f, 0.0f, 1.0f ) };

//http://www.mathaddict.net/hamming.htm
//http://homepages.bw.edu/~rmolmen/multimedia/hammingcode.swf
unsigned char encode_2bits( const unsigned char data2 )
{
	return 0;
}

inline unsigned char get_bit( const unsigned char byte, const char bit )
{
	assert( bit >= 0 && bit < 8 );

	return ( ( byte >> bit ) & 1 )/* == 1*/;
}

unsigned char decode_2bits( const unsigned char codeword5, bool & error )
{
	return 0;
}

MBAR::MBAR( const std::string & file_name )
{
	image_ = cv::imread( file_name );
}

void MBAR::start( const std::string & file_name )
{
	image_ = cv::imread( file_name );
	start();
}

template<typename type>
std::string to_binary( const type & value )
{
	std::bitset<sizeof( type ) * 8> bs( value );

	return bs.to_string();
}

void MBAR::start()
{	
	contours_.clear();
	markers_.clear();

	preprocess_image();
	find_contours();
	crop_markers();
	read_markers();
	refine_corners();
	solve_pnp();

	cv::waitKey(0);
}

void MBAR::preprocess_image()
{	
	cv::cvtColor(image_, image_gray_, cv::COLOR_BGR2GRAY);

	cv::imshow("orig", image_);
}

void MBAR::find_contours()
{
	cv::adaptiveThreshold(image_gray_, image_bw_, 255.0, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 3.0);
	cv::imshow("bw", image_bw_);

	std::vector<std::vector<cv::Point>> contours_candidates;
	cv::findContours(image_bw_.clone(), contours_candidates, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < contours_candidates.size(); i++)
	{
		if (contours_candidates[i].size() < 4) {
			continue;
		}

		std::vector<cv::Point2f> approximated_contour;
		cv::approxPolyDP(contours_candidates[i], approximated_contour, 0.02 * arcLength(contours_candidates[i], true), true);

		if (cv::contourArea(approximated_contour) < T_AREA) {
			continue;
		}

		if (!cv::isContourConvex(approximated_contour)) {
			continue;
		}

		std::cout << i << " - " << contours_candidates[i].size() << " -> " << approximated_contour.size() << std::endl;
		
		std::vector<cv::Point2f> contour_ccw;
		cv::convexHull(approximated_contour, contour_ccw, false);

		cv::Mat img = image_.clone();
		draw_contours(img, contours_candidates[i], cv::Scalar(256, 0, 0));
		draw_contours(img, contour_ccw, cv::Scalar(0, 256, 0));

		contours_.push_back(contour_ccw);
	}
	
}

void MBAR::draw_contours(cv::Mat& img, std::vector<cv::Point2f>& contours, cv::Scalar color)
{
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::circle(img, contours[i], 3, color, -1);
	}

	cv::imshow("contours", img);
}

void MBAR::draw_contours(cv::Mat& img, std::vector<cv::Point>& contours, cv::Scalar color)
{
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::circle(img, contours[i], 3, color, -1);
	}

	cv::imshow("contours", img);
}

void MBAR::crop_markers()
{
	for (size_t i = 0; i < contours_.size(); i++)
	{
		cv::Mat marker;
		cv::Mat is2ms = cv::getPerspectiveTransform(contours_[i], corners_);
		cv::warpPerspective(image_gray_, marker, is2ms, cv::Size(MARKER_SIZE, MARKER_SIZE));
		cv::threshold(marker, marker, 127.0, 255.0, cv::THRESH_OTSU);
		markers_.push_back(Marker(marker));
	}
}

void MBAR::read_markers()
{
	cv::namedWindow("marker", cv::WINDOW_KEEPRATIO);
	cv::namedWindow("marker_orig", cv::WINDOW_KEEPRATIO);
	cv::Mat orig_marker = cv::imread("../../data/marker.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::resize(orig_marker, orig_marker, cv::Size(MARKER_BINS, MARKER_BINS));
	cv::Mat m_rotation = cv::getRotationMatrix2D(cv::Point2f((MARKER_BINS - 1)*0.5f, (MARKER_BINS - 1)*0.5f), -90.0, 1.0);
	cv::Mat btwxor(orig_marker.size(), CV_8UC1);

	cv::imshow("marker_orig", orig_marker);
	cv::waitKey(30);

	for (size_t i = 0; i < markers_.size(); i++)
	{
		cv::Mat marker;

		cv::resize(markers_[i].image, marker, cv::Size(MARKER_BINS, MARKER_BINS));
		cv::threshold(marker, marker, 127.0, 255.0, cv::THRESH_BINARY);

		for (size_t r = 0; r < 4; r++)
		{
			cv::warpAffine(marker, marker, m_rotation, marker.size());
			std::rotate(contours_[i].begin(), contours_[i].end() - 1, contours_[i].end());

			cv::bitwise_xor(marker, orig_marker, btwxor);
			if (cv::norm(cv::sum(btwxor)) == 0) {
				cv::imshow("marker", marker);
				break;
			}
		}

		markers_[i].image = marker;
		markers_[i].corners_is = contours_[i];
	}
}

void MBAR::refine_corners()
{
	for (size_t i = 0; i < markers_.size(); i++)
	{
		cv::cornerSubPix(image_gray_, markers_[i].corners_is, cv::Size(5, 5),
		cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));
	}
}

cv::Point2d w2is(cv::Point3d ws, cv::Mat m_M) {
	cv::Mat point_ws(4, 1, CV_64FC1);
	point_ws.at<double>(0, 0) = ws.x;
	point_ws.at<double>(1, 0) = ws.y;
	point_ws.at<double>(2, 0) = ws.z;
	point_ws.at<double>(3, 0) = 1;

	cv::Mat point_is = m_M * point_ws;
	point_is /= point_is.at<double>(2, 0);

	cv::Point2d point = cv::Point2d(point_is.at<double>(0, 0), point_is.at<double>(1, 0));
	return point;
}

void MBAR::solve_pnp()
{
	cv::Mat m_camera = camera_matrix();
	cv::Mat distortion_coefficients = cv::Mat::zeros( 4, 1, CV_64FC1 );
	double & k1 = distortion_coefficients.at<double>( 0, 0 );
	double & k2 = distortion_coefficients.at<double>( 1, 0 );
	double & p1 = distortion_coefficients.at<double>( 2, 0 );
	double & p2 = distortion_coefficients.at<double>( 3, 0 );
	k1 = 0; // radial distortion
	k2 = 0;
	p1 = 0; // tangential distortion
	p2 = 0;

	for (size_t i = 0; i < markers_.size(); i++)
	{
		cv::Mat rvec, tvec;
		cv::solvePnP(corners_ws_, markers_[i].corners_is, m_camera, distortion_coefficients, rvec, tvec);
		cv::Mat m_R(3, 3, CV_64FC1);
		cv::Rodrigues(rvec, m_R);

		cv::Mat m_T;
		hconcat(m_R, tvec, m_T);
		cv::Mat m_M = m_camera * m_T;

		for (size_t j = 0; j < corners_ws_.size(); j++)
		{
			cv::Point2d point = w2is(corners_ws_[j], m_M);
			cv::circle(image_, point, 5, cv::Scalar(255, 0, 64 * j), -1);
		}

		cv::Point2d center = w2is(axis_ws_[0], m_M);
		cv::Point2d x = (w2is(axis_ws_[1] * 2, m_M));
		cv::Point2d y = (w2is(axis_ws_[2] * 2, m_M));
		cv::Point2d z = (w2is(axis_ws_[3] * 2, m_M));

		cv::circle(image_, center, 5, cv::Scalar(0, 255, 0), -1);
		cv::arrowedLine(image_, center, x, cv::Scalar(0, 0, 255), 2);
		cv::arrowedLine(image_, center, y, cv::Scalar(0, 255, 0), 2);
		cv::arrowedLine(image_, center, z, cv::Scalar(255, 0, 0), 2);

		cv::imshow("image_", image_);

	}
}

cv::Mat MBAR::camera_matrix() const
{
	cv::Mat camera_matrix = cv::Mat::zeros( 3, 3, CV_64FC1 );
	double & fx = camera_matrix.ptr<double>( 0 )[0];
	double & fy = camera_matrix.ptr<double>( 1 )[1];
	double & cx = camera_matrix.ptr<double>( 0 )[2];
	double & cy = camera_matrix.ptr<double>( 1 )[2];	
	camera_matrix.ptr<double>( 2 )[2] = 1.0;
	const cv::Size size = image_.size();
	const double fov_y = DEG2RAD( 42.185 );
	fx = ( size.height * 0.5 ) / tan( fov_y * 0.5 );
	fy = fx;
	cx = size.width * 0.5;
	cy = size.height * 0.5;

	return camera_matrix;
}
