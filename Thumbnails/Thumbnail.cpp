#include "stdafx.h"
#include "Thumbnail.h"


Thumbnail::Thumbnail(Size size, Size resolution)
{
	this->size = size;
	this->resolution = resolution;
	tileSize = Size(resolution.width / size.width, resolution.height / size.height);

	cout << size << endl;

	thumb = Mat(resolution, CV_8UC3);
}


Thumbnail::~Thumbnail()
{
}

void Thumbnail::insert(Mat mat)
{
	if (index >= size.width * size.height) {
		return;
	}

	int x = (index % size.width) * tileSize.width;
	int y = (index / size.width) * tileSize.height;

	Mat resized;
	resize(mat, resized, tileSize);

	resized.copyTo(thumb(Rect(x, y, tileSize.width, tileSize.height)));

	index++;
}

void Thumbnail::show()
{
	imshow("Thumbnail", thumb);
	waitKey(0);
}
