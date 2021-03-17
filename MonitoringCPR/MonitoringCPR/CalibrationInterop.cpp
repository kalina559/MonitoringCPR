#pragma once
#include "CameraCalibration.h"
#include <cstdlib>
#include "opencv2/objdetect.hpp"
#include <stdio.h>
#include <opencv2/tracking.hpp>
#include <math.h>
#include <SDL.h>
#include"StereoCapture.h"
#include<filesystem>

const float distanceBetweenCircles = 0.036f;
const cv::Size arrayOfCirclesSize = cv::Size(4, 11);

std::vector<std::pair<std::string, std::string>> validFramesPaths;

extern "C" bool __declspec(dllexport) __stdcall checkCalibrationFrames(int& invalidFramesCount, int& framesWithoutPair, int& totalFramesCount)
{
	validFramesPaths.clear();
	std::string path1 = "../MonitoringCPR/images/Calibration/UnityFirstCam/";
	std::string path2 = "../MonitoringCPR/images/Calibration/UnitySecondCam/";
	std::string imgFormat = "*.jpg";

	auto singleFrames = CameraCalibration::checkFramesPairs(path1, path2, imgFormat);
	framesWithoutPair = singleFrames.size();
	while (singleFrames.size() > 0)
	{
		remove((singleFrames.back()).c_str());
		singleFrames.pop_back();
	}
	int invalid = 0;

	std::vector<cv::String> fileNames1, fileNames2;
	std::vector<int> invalidIds;
	cv::glob(path1 + imgFormat, fileNames1, false);
	cv::glob(path2 + imgFormat, fileNames2, false);
	if (fileNames1.size() == fileNames2.size())
	{
		totalFramesCount = fileNames1.size();
	}
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
	if (totalFramesCount == invalidFramesCount)
	{
		return false;  //brak poprawnych zdjec
	}

	return true;     //wszystko git

}
extern "C" void __declspec(dllexport) __stdcall showValidFrame(unsigned char* firstFrameData, unsigned char* secondFrameData)
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
	cv::cvtColor(firstFrame, firstArgbImg, cv::COLOR_BGR2RGBA);
	cv::cvtColor(secondFrame, secondArgbImg, cv::COLOR_BGR2RGBA);
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
	if (validFramesPaths.size() > 1)
	{
		validFramesPaths.pop_back();
	}
	else
	{
		return false;
	}
}

extern "C" void __declspec(dllexport) __stdcall clearCalibrationFramesFolder()
{
	std::string command = "del /Q ";
	std::string path = "..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg";
	system(command.append(path).c_str());

	command = "del /Q ";
	path = "..\\MonitoringCPR\\images\\Calibration\\UnitySecondCam\\*.jpg";
	system(command.append(path).c_str());
}

std::string test = "halo";
extern "C" __declspec(dllexport) BSTR __stdcall  getFramesSetId()
{
	BSTR bs;
	std::string firstPath = "..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg";
	std::string secondPath = "..\\MonitoringCPR\\images\\Calibration\\UnitySecondCam\\*.jpg";
	std::vector<cv::String> fileNames1, fileNames2;

	cv::glob(firstPath, fileNames1, false); //todo wrzucic to w imgprocutility 
	cv::glob(secondPath, fileNames2, false);

	std::wstring outDigitString;
	if (fileNames1.size() == fileNames2.size() && fileNames1.size() != 0)
	{
		for (int i = 0; i < fileNames1.size(); ++i)
		{
			auto firstFileName = std::filesystem::path(fileNames1[i]).filename();
			auto secondFileName = std::filesystem::path(fileNames2[i]).filename();
			std::wstring firstFileNameStr{ firstFileName.wstring() };
			std::wstring secondFileNameStr{ secondFileName.wstring() };

			outDigitString += firstFileNameStr += secondFileNameStr;
		}
		bs = SysAllocStringLen(outDigitString.data(), outDigitString.size());
		return SysAllocString(bs);
	}
	bs = SysAllocString(L"");
	return SysAllocString(bs);
}

//extern "C" void __declspec(dllexport) __stdcall  saveId(std::string path)
//{
//	auto currentFrameSetId = ImgProcUtility::getId();
//	std::ofstream outputFile;
//	outputFile.open(path);
//	outputFile << currentFrameSetId;
//	outputFile.close();
//
//}
//extern "C" bool __declspec(dllexport) __stdcall  checkId()
//{
//	std::string validatedFrameSetId;
//	std::ifstream inputFile;
//	inputFile.open("validatedFrameSetId.txt");
//	inputFile >> validatedFrameSetId;
//	inputFile.close();
//
//	return validatedFrameSetId == ImgProcUtility::getId();
//}

extern "C" void __declspec(dllexport) __stdcall  stereoCalibrate(int& pairCount, int& time, bool& isFinished)
{
	Uint32 start_ticks = SDL_GetTicks();
	std::vector<cv::Mat> images1, images2;
	CameraCalibration::loadPhotos("..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg", images1);
	CameraCalibration::loadPhotos("..\\MonitoringCPR\\images\\Calibration\\UnitySecondCam\\*.jpg", images2);

	cv::Mat firstCamCoeffs, secondCamCoeffs, firstCamMatrix, secondCamMatrix;

	CameraCalibration::getSingleCamerasCoeffs(images1, images2, arrayOfCirclesSize, distanceBetweenCircles, firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs);

	cv::Mat R, T, E, F;

	double rms = CameraCalibration::stereoCalibration(images1, images2,
		firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs, arrayOfCirclesSize, R, T, E, F, distanceBetweenCircles);

	std::ofstream outStream("RMS.txt", std::ofstream::app);
	if (outStream)
	{
		outStream << "stereo RMS: " << rms << "\n";
	}
	outStream.close();

	cv::Mat R1, R2, P1, P2, Q;
	stereoRectify(firstCamMatrix, firstCamCoeffs, secondCamMatrix, secondCamCoeffs, cv::Size(1224, 1024), R, T, R1, R2, P1, P2, Q);
	CameraCalibration::saveRectifiedMatrices(R1, R2, P1, P2, Q);
	Uint32 end_ticks = SDL_GetTicks();
	pairCount = images1.size();
	time = (end_ticks - start_ticks) / 1000;
	isFinished = true;
	StereoCapture::getInstance()->initializeMatrices();
}

extern "C" int __declspec(dllexport) __stdcall  getEstimatedCalibrationTime()
{
	std::string firstPath = "..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg";
	std::vector<cv::String> fileNames1;
	cv::glob(firstPath, fileNames1, false); //todo wrzucic to w imgprocutility 

	int x = fileNames1.size();            //number of pairs of frames

	/*double estimatedTime = 0.0008 * pow(x, 3) - 0.0138 * pow(x, 2) + 0.393 * x - 1.2881;*/
	double estimatedTime = 0.0004 * pow(x, 3) + 0.0216 * pow(x, 2) - 0.5134 * x + 2.8644;

	return ceil(estimatedTime);
}
