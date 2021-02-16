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

const float distanceBetweenCircles = 0.0355f;
const cv::Size arrayOfCirclesSize = cv::Size(4, 11);

std::vector<std::pair<std::string, std::string>> validFramesPaths;

extern "C" void __declspec(dllexport) __stdcall checkCalibrationFrames(int& invalidFramesCount, int& totalFramesCount)
{
	std::string path1 = "../MonitoringCPR/images/Calibration/UnityFirstCam/*.jpg";
	std::string path2 = "../MonitoringCPR/images/Calibration/UnitySecondCam/*.jpg";
	int invalid = 0;

	std::vector<cv::String> fileNames1, fileNames2;
	std::vector<int> invalidIds;
	cv::glob(path1, fileNames1, false);
	cv::glob(path2, fileNames2, false);
	totalFramesCount = fileNames1.size();
	for (size_t i = 0; i < fileNames1.size(); ++i)
	{
		cv::Mat img1 = cv::imread(fileNames1[i]);
		cv::Mat img2 = cv::imread(fileNames2[i]);
		cv::Mat gray1, gray2;
		cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
		cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);
		std::vector<cv::Point2f> centers1, centers2;

		cv::SimpleBlobDetector::Params params;
		params.minArea = 10;
		params.minThreshold = 1;

		params.filterByConvexity = 1;
		params.minConvexity = 0.5;

		cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);

		bool patternFound1 = findCirclesGrid(gray1, arrayOfCirclesSize, centers1, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
		bool patternFound2 = findCirclesGrid(gray2, arrayOfCirclesSize, centers2, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

		if (patternFound1 && patternFound2)
		{			
			invalidIds.push_back(i);
			validFramesPaths.push_back({ fileNames1[i], fileNames2[i] });
		}
		else
		{
			++invalid;
			remove(fileNames1[i].c_str());
			remove(fileNames2[i].c_str());
		}
	}
	invalidFramesCount = invalid;
}
extern "C" bool __declspec(dllexport) __stdcall showInvalidFrame(unsigned char* firstFrameData, unsigned char* secondFrameData/*, int firstMaxValue, int secondMaxValue, int firstBlocksize = 11, int secondBlocksize = 11, int firstC = 12, int secondC = 12*/)
{
		auto firstFrame = cv::imread(validFramesPaths.back().first);
		auto secondFrame = cv::imread(validFramesPaths.back().second);

		cv::Mat gray1, gray2;
		cvtColor(firstFrame, gray1, cv::COLOR_BGR2GRAY);
		cvtColor(secondFrame, gray2, cv::COLOR_BGR2GRAY);
		std::vector<cv::Point2f> centers1, centers2;

		cv::SimpleBlobDetector::Params params;
		params.minArea = 10;
		params.minThreshold = 1;

		params.filterByConvexity = 1;
		params.minConvexity = 0.5;

		cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);

		//cv::Mat firstThresh, secondThresh;
		//cv::adaptiveThreshold(gray1, firstThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 12);
		//cv::adaptiveThreshold(gray2, secondThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 12);

		bool patternFound1 = findCirclesGrid(gray1, arrayOfCirclesSize, centers1, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
		bool patternFound2 = findCirclesGrid(gray2, arrayOfCirclesSize, centers2, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

		drawChessboardCorners(firstFrame, arrayOfCirclesSize, cv::Mat(centers1), patternFound1);
		drawChessboardCorners(secondFrame, arrayOfCirclesSize, cv::Mat(centers2), patternFound2);

		cv::Mat firstArgbImg, secondArgbImg;
		cv::cvtColor(firstFrame, firstArgbImg, CV_BGR2RGBA);
		cv::cvtColor(secondFrame, secondArgbImg, CV_BGR2RGBA);
		std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
		std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall deleteCurrentFrames()
{
	remove(validFramesPaths.back().first.c_str());
	remove(validFramesPaths.back().second.c_str());
}

extern "C" bool __declspec(dllexport) __stdcall moveToNextFrames()
{
	if (validFramesPaths.size() > 0)
	{
		validFramesPaths.pop_back();

		if (validFramesPaths.size() == 1)
		{
			return false;
		}
		return true;
	}
	else return false;
}