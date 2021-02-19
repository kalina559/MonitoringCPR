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
#include "ImgProcUtility.h"

const cv::Size arrayOfCirclesSize = cv::Size(4, 11);

extern "C" int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
{
	return ImgProcUtility::initializeCameras(realTimeCapturePair::getInstance());
}

extern "C" void __declspec(dllexport) __stdcall GetCalibrationFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	ImgProcUtility::readRealTimeFrames(realTimeCapturePair::getInstance(), width, height);
	cv::Mat firstFrameCopy, secondFrameCopy;
	realTimeCapturePair::getInstance()->getFirstCapture().getCurrentFrame().copyTo(firstFrameCopy);
	realTimeCapturePair::getInstance()->getSecondCapture().getCurrentFrame().copyTo(secondFrameCopy);

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

extern "C" void __declspec(dllexport) __stdcall GetCurrentGrayscaleFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	ImgProcUtility::readRealTimeFrames(realTimeCapturePair::getInstance(), width, height);
	//cv::Mat firstArgbImg, secondArgbImg;
	//cv::cvtColor(realTimeCapturePair::getInstance()->getFirstCapture().getCurrentFrame(), firstArgbImg, CV_BGR2RGBA);
	//cv::cvtColor(realTimeCapturePair::getInstance()->getSecondCapture().getCurrentFrame(), secondArgbImg, CV_BGR2RGBA);
	cv::Mat gray1, gray2;
	cvtColor(realTimeCapturePair::getInstance()->getFirstCapture().getCurrentFrame(), gray1, cv::COLOR_BGR2GRAY);
	cvtColor(realTimeCapturePair::getInstance()->getSecondCapture().getCurrentFrame(), gray2, cv::COLOR_BGR2GRAY);
	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(gray1, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(gray2, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall detectMarkers(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	std::vector<cv::Vec3f> circles1, circles2;

	ImgProcUtility::readRealTimeFrames(realTimeCapturePair::getInstance(), width, height);
	auto frame1 = realTimeCapturePair::getInstance()->getFirstCapture().getCurrentFrame();
	auto frame2 = realTimeCapturePair::getInstance()->getSecondCapture().getCurrentFrame();
	ImgProcUtility::detectMarkers(frame1, circles1);
	ImgProcUtility::detectMarkers(frame2, circles2);

	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(frame1, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(frame2, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}



extern "C" void __declspec(dllexport) __stdcall  CloseSDLCameras()
{
	realTimeCapturePair::getInstance()->freeCameras();
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
	cv::imwrite("../MonitoringCPR/images/Calibration/UnityFirstCam/" + timestamp, realTimeCapturePair::getInstance()->getFirstCapture().getCurrentFrame());
	cv::imwrite("../MonitoringCPR/images/Calibration/UnitySecondCam/" + timestamp, realTimeCapturePair::getInstance()->getSecondCapture().getCurrentFrame());
}

//funkcja startTracking() ktora zmienia bool isReady na true, tworzy zestaw ROIs i odpala multitrackery ( po wcisnieciu odpowiedniego przycisku)
extern "C" bool __declspec(dllexport) __stdcall startTrackingMarkers()
{
	realTimeCapturePair::getInstance()->setIsReady(true);
	return realTimeCapturePair::getInstance()->getIsReady();
}