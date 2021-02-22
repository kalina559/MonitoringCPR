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
#include"StereoCapture.h"
#include "ImgProcUtility.h"


const cv::Size arrayOfCirclesSize = cv::Size(4, 11);

extern "C" int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
{
	StereoCapture::getInstance()->initializeMatrices();
	//return ImgProcUtility::initializeCameras(StereoCapture::getInstance());
	return StereoCapture::getInstance()->initCameras();
}

extern "C" void __declspec(dllexport) __stdcall GetCalibrationFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	//ImgProcUtility::readRealTimeFrames(StereoCapture::getInstance(), width, height);
	StereoCapture::getInstance()->updateFrames(width, height);
	cv::Mat firstFrameCopy, secondFrameCopy;
	StereoCapture::getInstance()->getFirstCapture().getCurrentFrame().copyTo(firstFrameCopy);
	StereoCapture::getInstance()->getSecondCapture().getCurrentFrame().copyTo(secondFrameCopy);

	//wrzucic to w funkcje
	std::vector<cv::Point2f> centers1, centers2;
	cv::Mat gray1, gray2;
	cvtColor(firstFrameCopy, gray1, cv::COLOR_BGR2GRAY);
	cvtColor(secondFrameCopy, gray2, cv::COLOR_BGR2GRAY);
	cv::SimpleBlobDetector::Params params;
	params.minThreshold = 1;
	params.filterByConvexity = 1;
	params.minConvexity = 0.5;
	cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);

	bool patternFound1 = findCirclesGrid(gray1, arrayOfCirclesSize, centers1, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
	bool patternFound2 = findCirclesGrid(gray2, arrayOfCirclesSize, centers2, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
	drawChessboardCorners(firstFrameCopy, arrayOfCirclesSize, cv::Mat(centers1), 1);
	drawChessboardCorners(secondFrameCopy, arrayOfCirclesSize, cv::Mat(centers2), 1);
	//--------
	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(firstFrameCopy, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(secondFrameCopy, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall GetCurrentGrayscaleFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height, int threshLevel)
{
	StereoCapture::getInstance()->updateFrames(width, height);
	cv::Mat gray1, gray2;
	cvtColor(StereoCapture::getInstance()->getFirstCapture().getCurrentFrame(), gray1, cv::COLOR_BGR2GRAY);
	cvtColor(StereoCapture::getInstance()->getSecondCapture().getCurrentFrame(), gray2, cv::COLOR_BGR2GRAY);	
	auto threshImages = ImgProcUtility::thresholdImages({ gray1, gray2 }, threshLevel);
	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(threshImages.first, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(threshImages.second, secondArgbImg, CV_BGR2RGBA);	
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall  CloseSDLCameras()
{
	StereoCapture::getInstance()->freeCameras();
}

extern "C" void __declspec(dllexport) __stdcall saveCurrentFrames()
{
	char buffer[30];
	char dateStr[30];
	char timeStr[30];
	_strdate_s(dateStr);
	_strtime_s(timeStr);

	sprintf_s(buffer, "img%s_%s.jpg", dateStr, timeStr);

	std::string timestamp(buffer);
	std::replace(timestamp.begin(), timestamp.end(), ':', '_');
	std::replace(timestamp.begin(), timestamp.end(), '/', '_');
	cv::imwrite("../MonitoringCPR/images/Calibration/UnityFirstCam/" + timestamp, StereoCapture::getInstance()->getFirstCapture().getCurrentFrame());
	cv::imwrite("../MonitoringCPR/images/Calibration/UnitySecondCam/" + timestamp, StereoCapture::getInstance()->getSecondCapture().getCurrentFrame());
}

extern "C" void __declspec(dllexport) __stdcall detectMarkers(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	std::vector<cv::Vec3f> circles1, circles2;

	//ImgProcUtility::readRealTimeFrames(StereoCapture::getInstance(), width, height);
	StereoCapture::getInstance()->updateFrames(width, height);
	cv::Mat frame1, frame2;
	StereoCapture::getInstance()->getFirstCapture().getCurrentFrame().copyTo(frame1);
	StereoCapture::getInstance()->getSecondCapture().getCurrentFrame().copyTo(frame2);

	ImgProcUtility::detectMarkers(frame1, circles1);
	ImgProcUtility::detectMarkers(frame2, circles2);
	StereoCapture::getInstance()->getFirstCapture().setROIs(circles1);
	StereoCapture::getInstance()->getSecondCapture().setROIs(circles2);

	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(frame1, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(frame2, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}



extern "C" void __declspec(dllexport) __stdcall trackMarkers(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height, double& distance, bool& beginTracking)
{
	//cv::Mat img = cv::imread("C:/Users/Kalin/Desktop/pierdoly/memes/ee.jpg");
	cv::Mat firstCameraFrame, secondCameraFrame, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2;

	std::vector<cv::Vec3f> v3fCircles1, v3fCircles2;
	std::vector<cv::Vec3f> circles1, circles2;

	StereoCapture::getInstance()->updateFrames(width, height);

	if (!StereoCapture::getInstance()->getTrackingState())       //jesli jeszcze nie zaczeto trackowania, inicjalizacja multitrackerow
	{
		StereoCapture::getInstance()->getFirstCapture().startMultiTracker();
		StereoCapture::getInstance()->getSecondCapture().startMultiTracker();
		StereoCapture::getInstance()->setTrackingState(true);
	}

	StereoCapture::getInstance()->getFirstCapture().updateTracker();
	StereoCapture::getInstance()->getSecondCapture().updateTracker();

	std::pair<cv::Mat, cv::Mat> frames = { StereoCapture::getInstance()->getFirstCapture().getCurrentFrame() ,StereoCapture::getInstance()->getSecondCapture().getCurrentFrame() };
	std::pair<cv::Mat, cv::Mat> grayFrames = ImgProcUtility::convertFramesToGray(frames);

	auto outBalls = std::vector<ImgProcUtility::Coordinates>(expectedNumberOfMarkers);
	int outDetectedBallsCount;

	bool result1 = StereoCapture::getInstance()->getFirstCapture().calculateMarkersCoordinates();
	bool result2 = StereoCapture::getInstance()->getSecondCapture().calculateMarkersCoordinates();

	if (!result1 || !result2)
	{
		StereoCapture::getInstance()->setTrackingState(false);
		beginTracking = false;
		StereoCapture::getInstance()->getFirstCapture().stopMultiTracker();
		StereoCapture::getInstance()->getSecondCapture().stopMultiTracker();
		return;
	}

	StereoCapture::getInstance()->triangulateCameras();    //chyba dziala, triangSize = 2

	ImgProcUtility::getMarkersCoordinates3D(StereoCapture::getInstance()->getTriangCoordinates(), outBalls, outDetectedBallsCount);

	distance = ImgProcUtility::calculateDistanceBetweenMarkers(outBalls, 0, 1);

	ImgProcUtility::displayDistanceBetweenMarkers(frames.first, outBalls, 0, 1);

	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(frames.first, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(frames.second, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());

}

extern "C" void __declspec(dllexport) __stdcall saveThreshLevel(int threshLevel)
{
	std::ofstream outputFile;
	outputFile.open("thresholdLevel.txt");
	outputFile << threshLevel;
	outputFile.close();
	StereoCapture::getInstance()->setTreshLevel(threshLevel);
}
extern "C" void __declspec(dllexport) __stdcall updateThreshLevel(int threshLevel)
{
	StereoCapture::getInstance()->setTreshLevel(threshLevel);
}
extern "C" int __declspec(dllexport) __stdcall  getThreshLevel()
{
	return StereoCapture::getInstance()->getFirstCapture().getThreshLevel();
}