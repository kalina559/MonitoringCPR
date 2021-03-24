#pragma once
#define NOMINMAX
#include <cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include<opencv2/calib3d.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include<comutil.h>
#include <comdef.h>
#include <SDL.h>
#include "ps3eye.h"
#include <opencv2/tracking.hpp>
#include<opencv2/tracking/tracking_legacy.hpp>
#include<filesystem>
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

const float distanceBetweenCircles = 0.068f;
const cv::Size arrayOfCirclesSize = cv::Size(4, 11);

struct StereoROISets
{
	std::vector<cv::Rect> first;
	std::vector<cv::Rect> second;
};
struct StereoCoordinates2D
{
	std::vector<cv::Vec2f> first;
	std::vector<cv::Vec2f> second;
};

enum singleCameraId
{
	firstCamera,
	secondCamera
};

struct contourSorter
{
	bool operator ()(cv::Vec3f first, cv::Vec3f second)
	{
		return ((first[0] + 100 * first[1]) < (second[0] + 100 * second[1]));
	}
};
struct Matrices
{
	cv::Mat firstCamMatrix = cv::Mat(cv::Size(3, 3), CV_64FC1);
	cv::Mat secondCamMatrix = cv::Mat(cv::Size(3, 3), CV_64FC1);
	cv::Mat firstCamCoeffs = cv::Mat(cv::Size(1, 5), CV_64FC1);
	cv::Mat secondCamCoeffs = cv::Mat(cv::Size(1, 5), CV_64FC1);
	cv::Mat P1 = cv::Mat(cv::Size(4, 3), CV_64FC1);
	cv::Mat P2 = cv::Mat(cv::Size(4, 3), CV_64FC1);
	cv::Mat R1 = cv::Mat(cv::Size(3, 3), CV_64FC1);
	cv::Mat R2 = cv::Mat(cv::Size(3, 3), CV_64FC1);
};
namespace ImgProcUtility
{
	struct Coordinates
	{
		Coordinates() {}
		Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
		float X, Y, Z;
	};
	std::string readFile(std::string name);
	std::pair<cv::Mat, cv::Mat> thresholdImages(std::pair<cv::Mat, cv::Mat> frames, int thresh);	
	std::pair<cv::Mat, cv::Mat> performCanny(std::pair<cv::Mat, cv::Mat> frames, int threshold);	
	bool getBiggestContours(cv::Mat frame, std::vector<cv::Point>& biggestContour);
	bool findCircleInROI(cv::Mat frame, cv::Vec3f& ROI, int threshLevel);
	cv::Vec3f getContoursMinEnclosingCircle(std::vector<cv::Point> contour);
	cv::Mat process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices);
	void getMarkersCoordinates3D(cv::Mat triangCoords, ImgProcUtility::Coordinates* outBalls);
	std::pair<cv::Mat, cv::Mat> populateMatricesFromVectors(StereoCoordinates2D coordinates2D);
	BSTR getFrameSetId(std::string path);
	void detectMarkers(cv::Mat& frame, cv::Mat& displayFrame, std::vector<cv::Vec3f>& circles);
	bool isROIinFrame(cv::Rect ROI, cv::Mat frame);
	std::vector<std::string> getFileNames(std::string path);
	std::string getCurrentDateStr();
	std::string getCurrentTimeStr();
	void drawDetectedCirclesGrid(cv::Mat& frame, cv::Ptr<cv::FeatureDetector> blobDetector);
	cv::Ptr<cv::FeatureDetector> initBlobDetector();
}