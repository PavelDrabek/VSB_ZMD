// Thumbnails.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

vector<string> getFilesInDir(const std::string& dir)
{
	std::vector<cv::String> filenames; // notice here that we are using the Opencv's embedded "String" class
	glob(dir, filenames); // new function that does the job ;-)

	std::vector<std::string> objects;
	for (size_t i = 0; i < filenames.size(); ++i) {
		objects.push_back(filenames[i]);
		//std::cout << filenames[i] << std::endl;
	}
	return objects;
}

vector<Mat> getMats(vector<string> filenames, int count) {
	vector<Mat> mats;
	for (size_t i = 0; i < filenames.size() || i < count; i++)
	{
		Mat mat = imread(filenames[i], 1);
		mats.push_back(mat);
	}
	return mats;
}

vector<Mat> getMats(string videoPath, int count) {
	vector<Mat> mats;
	VideoCapture cap(videoPath);
	
	for (size_t i = 0; i < count; i++)
	{
		Mat mat;
		cap >> mat;
		mats.push_back(mat);
		for (size_t j = 0; j < 10; j++) {
			cap >> mat;
		}
	}
	return mats;
}

int main(int argc, const char * argv[]) {

	for (size_t i = 0; i < argc; i++)
	{
		cout << argv[i] << endl;
	}

	if (argc != 6) {
		cout << "folder sizex, sizey, resx, resy";
		return -1;
	}

	string folder = argv[1];
	int sizex = strtol(argv[2], NULL, 10);
	int sizey = strtol(argv[3], NULL, 10);
	int resx = strtol(argv[4], NULL, 10);
	int resy = strtol(argv[5], NULL, 10);

	Thumbnail t(Size(sizex, sizey), Size(resx, resy));

	string ext = folder.substr(folder.find_last_of(".") + 1);
	cout << "extension: " << ext << endl;

	vector<Mat> mats;
	if (ext == "mp4" || ext == "avi") {
		mats = getMats(folder, sizex * sizey);
	}
	else {
		vector<string> filenames = getFilesInDir(folder);
		mats = getMats(filenames, sizex * sizey);
	}

	for (size_t i = 0; i < mats.size(); i++)
	{
		t.insert(mats[i]);
	}

	t.show();

	return 0;
}
