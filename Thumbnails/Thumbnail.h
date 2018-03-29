#pragma once
class Thumbnail
{
	Size size;
	Size resolution;
	Mat thumb;

	Size tileSize;

	int index = 0;

public:
	Thumbnail(Size size, Size resolution);
	~Thumbnail();

	void insert(Mat mat);
	void show();
};

