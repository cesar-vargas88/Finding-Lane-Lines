// Finding Lane Lines.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define imageWidht  640
#define imageHeight 480

Mat getGray(Mat source);
Mat getBlur(Mat source, int kernelSize, int borderType);
Mat getEdge(Mat source, double minThreshold, double maxThreshold, int apertureSize);
Mat getInterestRegion(Mat source, vector<Point> vertices);
Mat getLines(Mat source, double rho, double theta, int threshold, double minLineLength, double maxLineGap);
Mat drawLines(Mat source, vector<Vec4i> vlines, cv::Scalar color, int thickness);
Mat getWeighted(Mat source1, Mat source2, double α, double β, double γ);

int main()
{
	Mat windowColor(Size(imageWidht * 3, imageHeight), CV_8UC3);
	Mat windowGray(Size(imageWidht * 3, imageHeight), CV_8UC1);

	string sourcePath = "Images/";
	vector<String> imagesNames;

	// Get all images in the source folder
	glob(sourcePath, imagesNames);
	Mat source = imread(imagesNames[1]);

	// Verify that image opened correctly
	if (!source.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Image processing
	resize(source, source, Size(imageWidht, imageHeight));			// Resize the image						resize(InputArray, OutputArray, KernelSize)

	vector<Point> vertices;
	vertices.push_back(Point(source.cols * 0.00 , source.rows * 1.00));
	vertices.push_back(Point(source.cols * 0.46 , source.rows * 0.60));
	vertices.push_back(Point(source.cols * 0.54 , source.rows * 0.60));
	vertices.push_back(Point(source.cols * 1.00 , source.rows * 1.00));
	
	Mat gray = getGray(source);
	Mat blur = getBlur(gray, 7, 0);
	Mat edge = getEdge(blur, 80, 160, 3);
	Mat interestRegion = getInterestRegion(edge, vertices);
	Mat lines = getLines(interestRegion, 10, CV_PI / 180, 10, 10, 50);
	Mat weighted = getWeighted(lines, source, 1, 1, 0);

	// Show the images 
	source.copyTo(windowColor(Rect(0, 0, imageWidht, imageHeight)));
	lines.copyTo(windowColor(Rect(640, 0, imageWidht, imageHeight)));
	weighted.copyTo(windowColor(Rect(1280, 0, imageWidht, imageHeight)));

	gray.copyTo(windowGray(Rect(0, 0, imageWidht, imageHeight)));
	interestRegion.copyTo(windowGray(Rect(640, 0, imageWidht, imageHeight)));
	interestRegion.copyTo(windowGray(Rect(1280, 0, imageWidht, imageHeight)));
	
	namedWindow("Color", WINDOW_NORMAL);
	namedWindow("Gray", WINDOW_NORMAL);
	imshow("Color", windowColor);
	imshow("Gray", windowGray);
	
	waitKey(0);
	return 0;
}


Mat getGray(Mat source)
{
	Mat gray;

	cvtColor(source, gray, COLOR_BGR2GRAY);

	return gray;
}

Mat getBlur(Mat source, int kernelSize, int borderType)
{
	Mat blur;

	GaussianBlur(source, blur, Size(kernelSize, kernelSize), borderType);

	return blur;
}

Mat getEdge(Mat source, double minThreshold, double maxThreshold, int apertureSize)
{ 
	Mat edge;

	Canny(source, edge, minThreshold, maxThreshold, apertureSize);	

	return edge;
}

Mat getInterestRegion(Mat source, vector<Point> vertices)
{
	Mat interestRegion = source.clone();
	// Create interest area image
	Mat mask(Size(source.cols, source.rows), CV_8UC1, Scalar(0, 0, 0));
	fillConvexPoly(mask, vertices, Scalar(255, 255, 255));
	bitwise_and(mask, source, interestRegion);

	return interestRegion;
}

Mat getLines(Mat source, double rho, double theta, int threshold, double minLineLength, double maxLineGap)
{
	vector<Vec4i> vlines;						
	HoughLinesP(source, vlines, rho, theta, threshold, minLineLength, maxLineGap);
	Mat lines = drawLines(source, vlines, Scalar(0, 0, 255), 4);

	return lines;
}

Mat drawLines(Mat source, vector<Vec4i> vlines, cv::Scalar color, int thickness)
{
	Mat lines(Size(source.cols, source.rows), CV_8UC3, Scalar(0, 0, 0));

	for (size_t i = 0; i < vlines.size(); i++)
		line(lines, Point(vlines[i][0], vlines[i][1]), Point(vlines[i][2], vlines[i][3]), color, thickness, 8);

	return lines;
}

Mat getWeighted(Mat source1, Mat source2, double α, double β, double γ)
{
	Mat weighted;
	addWeighted(source1, α, source2, β, 0.0, weighted);

	return weighted;
}

