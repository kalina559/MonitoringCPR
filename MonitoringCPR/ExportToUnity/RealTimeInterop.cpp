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

realTimeCapturePair stereoCapture;

// real-time tracking functions
extern "C" int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
{
	return ImgProcUtility::initializeCameras(stereoCapture);
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

	stereoCapture.getFirstCapture().getCamera()->stop();
	stereoCapture.getSecondCapture().getCamera()->stop();
	stereoCapture.getFirstCapture().setCamera(NULL);
	stereoCapture.getSecondCapture().setCamera(NULL);

}

extern "C" void __declspec(dllexport) __stdcall clearCalibrationFramesFolder()
{
	std::string command = "del /Q ";
	std::string firstPath = "..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg";
	std::string secondPath = "..\\MonitoringCPR\\images\\Calibration\\UnitySecondCam\\*.jpg";
	system(command.append(firstPath).c_str());
	system(command.append(secondPath).c_str());
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
