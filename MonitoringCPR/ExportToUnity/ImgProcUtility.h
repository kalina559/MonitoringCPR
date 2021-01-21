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

class StereoFrames
{	
public:
	Mat first, second;
	//StereoFrames(Mat a, Mat b) : first(a), second(b) {}
};
class StereoROIs
{
public:
	std::vector<cv::Rect> first, second;
};
namespace ImgProcUtility
{
	struct Coordinates
	{
		Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
		float X, Y, Z;
	};
	StereoFrames readFrames(VideoCapture firstSequence, VideoCapture secondSequence);
	StereoFrames resizeFrames(StereoFrames frames, double scale);
	void selectMarkers(bool& condition, StereoFrames frames, std::vector<cv::Rect>& firstROIs, std::vector<cv::Rect>& secondROIs, 
		cv::Ptr<cv::MultiTracker> firstMultiTracker, 	cv::Ptr<cv::MultiTracker> secondMultiTracker);
	void updateTrackers(cv::Ptr<cv::MultiTracker> firstMultiTracker, cv::Ptr<cv::MultiTracker> secondMultiTracker, Mat firstCamFrame, Mat secondCamFrame);
	void convertFramesToGray(Mat firstCamFrame, Mat secondCamFrame, Mat& firstCamGray, Mat& secondCamGray);
	void cutROIsFromFrames(Mat& croppedImg1, Mat& croppedImg2, Mat gray1, Mat gray2, Rect2d firstROI, Rect2d secondROI);
	void thresholdImages(Mat& threshImg1, Mat& threshImg2, Mat croppedImg1, Mat croppedImg2, double thresh, double maxval, int type);
	int getMarkersCoordinates2D(Mat gray1, Mat gray2, std::vector<cv::Rect> firstROIs, std::vector<cv::Rect> secondROIs, cv::Ptr<cv::MultiTracker> firstMultiTracker,
		cv::Ptr<cv::MultiTracker> secondMultiTracker, Mat& firstCamFrame, Mat& secondCamFrame, std::vector<cv::Vec2f>& firstCamCoordinates2D, std::vector<cv::Vec2f>& secondCamCoordinates2D);
	void drawCirclesAroundMarkers(Mat& firstCamFrame, Mat& secondCamFrame, Point firstCenter, Point secondCenter, int firstRadius, int secondRadius, Scalar color);
	void drawRectAroundROI(Mat& firstCamFrame, Mat& secondCamFrame, Rect firstCamTrackedArea, Rect secondCamTrackedArea, Scalar color, int thickness, int linetype);
	void findCirclesInROIs(vector<Vec3f>& firstCamCircles, vector<Vec3f>& secondCamCircles, Mat threshImg1, Mat threshImg2);
	Mat process2DCoordinates(std::vector<cv::Vec2f> firstCamCoordinates2D, std::vector<cv::Vec2f> secondCamCoordinates2D, Mat firstCamMatrix, Mat secondCamMatrix,
		Mat firstCamCoeffs, Mat secondCamCoeffs, Mat R1, Mat R2, Mat P1, Mat P2);
	void getMarkersCoordinates3D(Mat triangCoords, ImgProcUtility::Coordinates* outBalls, int& outDetectedBallsCount);
	void populateMatriceFromVector(std::vector<cv::Vec2f> firstCamCoordinates2D, std::vector<cv::Vec2f> secondCamCoordinates2D, Mat& firstCamDistCoords, Mat& secondCamDistCoords);
	double calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
	void displayDistanceBetweenMarkers(Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId);
}