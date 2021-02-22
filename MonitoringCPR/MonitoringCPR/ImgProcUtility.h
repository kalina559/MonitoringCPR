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
namespace ImgProcUtility
{
	struct Coordinates
	{
		Coordinates() {}
		Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
		float X, Y, Z;
	};
	//int initializeCameras(realTimeCapturePair* stereoCapture);
	std::string readFile(std::string name);
	std::pair<cv::Mat, cv::Mat> readFrames(cv::VideoCapture firstSequence, cv::VideoCapture secondSequence);
	//void readRealTimeFrames(realTimeCapturePair* stereoCapture, int width, int height);
	std::pair<cv::Mat, cv::Mat> resizeFrames(std::pair<cv::Mat, cv::Mat> frames, double scale);
	StereoROISets selectMarkers(std::pair<cv::Mat, cv::Mat> frames, std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers);
	void updateTrackers(std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames);
	std::pair<cv::Mat, cv::Mat> convertFramesToGray(std::pair<cv::Mat, cv::Mat> colorFrames);
	std::pair<cv::Mat, cv::Mat> cutROIsFromFrames(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Rect, cv::Rect> ROI);
	std::pair<cv::Mat, cv::Mat> thresholdImages(std::pair<cv::Mat, cv::Mat> frames, int thresh);
	std::pair<cv::Mat, cv::Mat> erodeImages(std::pair<cv::Mat, cv::Mat> frames, int erosionSize, int erosionType);
	cv::Mat erodeImage(cv::Mat frame, int erosionSize, int erosionType);
	std::pair<cv::Mat, cv::Mat> performCanny(std::pair<cv::Mat, cv::Mat> frames, int threshold);
	StereoCoordinates2D getMarkersCoordinates2D(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames);
	void drawCirclesAroundMarkers(std::pair<cv::Mat, cv::Mat> frames, StereoCoordinates2D circleCoordinates, std::vector<std::pair<int, int>> radiuses);
	void drawCirclesAroundMarkers(cv::Mat frame, std::vector<cv::Vec2f> circleCoordinates, std::vector<int> radiuses);
	void drawRectAroundROI(std::pair<cv::Mat, cv::Mat> frames, std::pair<cv::Rect, cv::Rect> trackedAreas);
	std::pair<std::vector<cv::Point>, std::vector<cv::Point>> getBiggestContours(std::pair<cv::Mat, cv::Mat> frames);
	bool getBiggestContours(cv::Mat frame, std::vector<cv::Point>& biggestContour);
	cv::Vec3f getContoursCenterOfMass(std::vector<cv::Point> contour);
	std::pair<cv::Vec3f, cv::Vec3f> findCirclesInROIs(std::pair<cv::Mat, cv::Mat> frames);
	bool findCircleInROI(cv::Mat frame, cv::Vec3f& ROI, int threshLevel);
	cv::Vec3f findCircleInROI(cv::Mat frame);
	cv::Mat process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices);
	void getMarkersCoordinates3D(cv::Mat triangCoords, ImgProcUtility::Coordinates* outBalls, int& outDetectedBallsCount);
	void getMarkersCoordinates3D(cv::Mat triangCoords, std::vector<Coordinates>& outBalls, int& outDetectedBallsCount);
	std::pair<cv::Mat, cv::Mat> populateMatricesFromVectors(StereoCoordinates2D coordinates2D);
	double calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
	double calculateDistanceBetweenMarkers(std::vector<Coordinates> outBalls, int firstMarkerId, int secondMarkerId);
	void displayDistanceBetweenMarkers(cv::Mat& displayMatrix, std::vector<Coordinates> outBalls, int firstMarkerId, int secondMarkerId);
	void displayDistanceBetweenMarkers(cv::Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
	std::string getId();
	void detectMarkers(cv::Mat& frame, std::vector<cv::Vec3f>& circles);
}