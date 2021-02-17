#pragma once
#include "CameraCalibration.h"
#include <opencv2/tracker.hpp>
#include <opencv2/cuda.hpp>
#include <cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/tracking.hpp>
#include <math.h>
#include <SDL.h>
#include "ps3eye.h"
#include"RealTimeCapture.h"

const int expectedNumberOfMarkers = 2;

struct StereoROISets
{
	std::vector<cv::Rect> first = std::vector<cv::Rect>(expectedNumberOfMarkers);
	std::vector<cv::Rect> second = std::vector<cv::Rect>(expectedNumberOfMarkers);
};

struct StereoCoordinates2D
{
	std::vector<cv::Vec2f> first = std::vector<cv::Vec2f>(expectedNumberOfMarkers);
	std::vector<cv::Vec2f> second = std::vector<cv::Vec2f>(expectedNumberOfMarkers);
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

class realTimeCapturePair
{
	RealTimeCapture first;
	RealTimeCapture second;
	bool isInitialized = false;
public:
	RealTimeCapture& getFirstCapture()
	{
		return first;
	}
	RealTimeCapture& getSecondCapture()
	{
		return second;
	}
	void setIsInitialized(bool value)
	{
		isInitialized = value;
	}
	bool getIsInitialized()
	{
		return isInitialized;
	}
};


// stworzyc na tej podstawie klase StereoFrames: pola Mat first, second, colorFrames, grayFrames itd.
// potem klase pochodn¹ StereoCapture, dodaæ multitrackery, ROI itd. i w zast¹piæ niektóre parametry funkcji za pomoc¹ pól klasy

namespace ImgProcUtility
{
	struct Coordinates
	{
		Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
		float X, Y, Z;
	};
	int initializeCameras(realTimeCapturePair& stereoCapture);
	std::pair<cv::Mat, cv::Mat> readFrames(cv::VideoCapture firstSequence, cv::VideoCapture secondSequence);
	void readRealTimeFrames(realTimeCapturePair& stereoCapture, int width, int height);
	std::pair<cv::Mat, cv::Mat> resizeFrames(std::pair<cv::Mat, cv::Mat> frames, double scale);
	StereoROISets selectMarkers(std::pair<cv::Mat, cv::Mat> frames, std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers);
	void updateTrackers(std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames);
	std::pair<cv::Mat, cv::Mat> convertFramesToGray(std::pair<cv::Mat, cv::Mat> colorFrames);
	std::pair<cv::Mat, cv::Mat> cutROIsFromFrames(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Rect, cv::Rect> ROI);
	std::pair<cv::Mat, cv::Mat> thresholdImages(std::pair<cv::Mat, cv::Mat> frames, int thresh);
	std::pair<cv::Mat, cv::Mat> erodeImages(std::pair<cv::Mat, cv::Mat> frames, int erosionSize, int erosionType);
	std::pair<cv::Mat, cv::Mat> performCanny(std::pair<cv::Mat, cv::Mat> frames, int threshold);
	StereoCoordinates2D getMarkersCoordinates2D(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames);
	void drawCirclesAroundMarkers(std::pair<cv::Mat, cv::Mat> frames, StereoCoordinates2D circleCoordinates, std::vector<std::pair<int, int>> radiuses);
	void drawRectAroundROI(std::pair<cv::Mat, cv::Mat> frames, std::pair<cv::Rect, cv::Rect> trackedAreas);
	std::pair<std::vector<cv::Point>, std::vector<cv::Point>> getBiggestContours(std::pair<cv::Mat, cv::Mat> frames);
	cv::Vec3f getContoursCenterOfMass(std::vector<cv::Point> contour);
	std::pair<cv::Vec3f, cv::Vec3f> findCirclesInROIs(std::pair<cv::Mat, cv::Mat> frames);
	cv::Mat process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices);
	void getMarkersCoordinates3D(cv::Mat triangCoords, ImgProcUtility::Coordinates* outBalls, int& outDetectedBallsCount);
	std::pair<cv::Mat, cv::Mat> populateMatricesFromVectors(StereoCoordinates2D coordinates2D);
	double calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
	void displayDistanceBetweenMarkers(cv::Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
}