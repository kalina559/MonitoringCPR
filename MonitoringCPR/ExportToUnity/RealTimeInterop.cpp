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

bool initialized = false;
realTimeCapturePair stereoCapture;
const cv::Size arrayOfCirclesSize = cv::Size(4, 11);
// real-time tracking functions
extern "C" int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
{
	return ImgProcUtility::initializeCameras(stereoCapture);
}

extern "C" void __declspec(dllexport) __stdcall GetCalibrationFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	ImgProcUtility::readRealTimeFrames(stereoCapture, width, height);	
	cv::Mat firstFrameCopy, secondFrameCopy;
	stereoCapture.getFirstCapture().getCurrentFrame().copyTo(firstFrameCopy);
	stereoCapture.getSecondCapture().getCurrentFrame().copyTo(secondFrameCopy);

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

extern "C" void __declspec(dllexport) __stdcall GetSDLCurrentFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	ImgProcUtility::readRealTimeFrames(stereoCapture, width, height);	
	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(stereoCapture.getFirstCapture().getCurrentFrame(), firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(stereoCapture.getSecondCapture().getCurrentFrame(), secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}




extern "C" void __declspec(dllexport) __stdcall  CloseSDLCameras()
{
	if (initialized)
	{
		stereoCapture.getFirstCapture().getCamera()->stop();
		stereoCapture.getSecondCapture().getCamera()->stop();
		stereoCapture.getFirstCapture().setCamera(NULL);
		stereoCapture.getSecondCapture().setCamera(NULL);
	}
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
	cv::imwrite("../MonitoringCPR/images/Calibration/UnityFirstCam/CAM1_" + timestamp, stereoCapture.getFirstCapture().getCurrentFrame());
	cv::imwrite("../MonitoringCPR/images/Calibration/UnitySecondCam/CAM2_" + timestamp, stereoCapture.getSecondCapture().getCurrentFrame());
}
