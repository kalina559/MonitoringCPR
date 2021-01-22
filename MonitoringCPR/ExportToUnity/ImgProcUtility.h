#pragma once
#include "CameraCalibration.h"
#include<opencv2/tracker.hpp>
#include<opencv2/cuda.hpp>
#include<cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/tracking.hpp>

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
	Mat firstCamMatrix = Mat(Size(3, 3), CV_64FC1);
	Mat secondCamMatrix = Mat(Size(3, 3), CV_64FC1);
	Mat firstCamCoeffs = Mat(Size(1, 5), CV_64FC1);
	Mat secondCamCoeffs = Mat(Size(1, 5), CV_64FC1);
	Mat P1 = Mat(Size(4, 3), CV_64FC1);
	Mat P2 = Mat(Size(4, 3), CV_64FC1);
	Mat R1 = Mat(Size(3, 3), CV_64FC1);
	Mat R2 = Mat(Size(3, 3), CV_64FC1);
};

namespace ImgProcUtility
{
	struct Coordinates
	{
		Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
		float X, Y, Z;
	};
	pair<Mat, Mat> readFrames(VideoCapture firstSequence, VideoCapture secondSequence);
	pair<Mat, Mat> resizeFrames(pair<Mat, Mat> frames, double scale);
	StereoROISets selectMarkers(pair<Mat, Mat> frames, pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers);
	void updateTrackers(pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers, pair<Mat, Mat> frames);
	pair<Mat, Mat> convertFramesToGray(pair<Mat, Mat> colorFrames);
	pair<Mat, Mat> cutROIsFromFrames(pair<Mat, Mat> grayFrames, pair<Rect, Rect> ROI);
	pair<Mat, Mat> thresholdImages(pair<Mat, Mat> frames);
	StereoCoordinates2D getMarkersCoordinates2D(pair<Mat, Mat> grayFrames, pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers, pair<Mat, Mat> frames);
	void drawCirclesAroundMarkers(pair<Mat, Mat> frames, StereoCoordinates2D circleCoordinates, vector<std::pair<int, int>> radiuses);
	void drawRectAroundROI(pair<Mat, Mat> frames, pair<Rect, Rect> trackedAreas);
	void findCirclesInROIs(pair<vector<Vec3f>, vector<Vec3f>>& circles, pair<Mat, Mat> frames);
	Mat process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices);
	void getMarkersCoordinates3D(Mat triangCoords, ImgProcUtility::Coordinates* outBalls, int& outDetectedBallsCount);
	pair<Mat, Mat> populateMatricesFromVectors(StereoCoordinates2D coordinates2D);
	double calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
	void displayDistanceBetweenMarkers(Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
}