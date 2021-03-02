#pragma once
#include "CameraCalibration.h"
#include<cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/tracking.hpp>
#include"ImgProcUtility.h"
#include <filesystem>

Matrices matrices;
cv::VideoCapture firstSequence("../MonitoringCPR/images/movement/seria2/camera1/video1.avi");
cv::VideoCapture secondSequence("../MonitoringCPR/images/movement/seria2/camera2/video2.avi");

std::pair<cv::Ptr<cv::legacy::MultiTracker>, cv::Ptr<cv::legacy::MultiTracker>> multiTrackers;   //do schowania w stereoCapture
StereoROISets ROIs;    //do schowania w stereoCapture

bool firstFrame;
int _scale = 1;

// tracking on a previously recorded video
extern "C" int __declspec(dllexport) __stdcall Init(int& outCameraWidth, int& outCameraHeight)
{
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamMatrix", 3, 3, matrices.firstCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamMatrix", 3, 3, matrices.secondCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamCoeffs", 5, 1, matrices.firstCamCoeffs);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamCoeffs", 5, 1, matrices.secondCamCoeffs);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P1", 3, 4, matrices.P1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P2", 3, 4, matrices.P2);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R1", 3, 3, matrices.R1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R2", 3, 3, matrices.R2);

	multiTrackers.first = cv::legacy::MultiTracker::create();
	multiTrackers.second = cv::legacy::MultiTracker::create();

	firstSequence.open("C:/Users/Kalin/Desktop/studia/in¿ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera1/video1.avi");
	secondSequence.open("C:/Users/Kalin/Desktop/studia/in¿ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera2/video2.avi");

	if (!firstSequence.isOpened() || !secondSequence.isOpened())
	{
		return -2;
	}
	firstFrame = true;
	outCameraWidth = firstSequence.get(cv::CAP_PROP_FRAME_WIDTH);
	outCameraHeight = firstSequence.get(cv::CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}

extern "C" void __declspec(dllexport) __stdcall GetCurrentFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	std::pair<cv::Mat, cv::Mat> frames = ImgProcUtility::readFrames(firstSequence, secondSequence);

	if (frames.first.empty() || frames.second.empty())       //wrzucic to w jakas funkcje
	{
		firstSequence.release();
		secondSequence.release();
		return;
	}

	cv::Mat firstResizedMat(height, width, frames.first.type());
	cv::Mat secondResizedMat(height, width, frames.first.type());
	cv::resize(frames.first, firstResizedMat, firstResizedMat.size(), cv::INTER_CUBIC);
	cv::resize(frames.second, secondResizedMat, secondResizedMat.size(), cv::INTER_CUBIC);
	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(firstResizedMat, firstArgbImg, cv::COLOR_BGR2RGBA);
	cv::cvtColor(secondResizedMat, secondArgbImg, cv::COLOR_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" bool __declspec(dllexport) __stdcall recordedDetectMarkers(int width, int height)
{
	std::vector<cv::Vec3f> circles1, circles2;

	std::pair<cv::Mat, cv::Mat> frames = ImgProcUtility::readFrames(firstSequence, secondSequence);
	std::pair<cv::Mat, cv::Mat> resizedFrames = ImgProcUtility::resizeFrames(frames, 0.5);


	ImgProcUtility::detectMarkers(resizedFrames.first, circles1);
	ImgProcUtility::detectMarkers(resizedFrames.second, circles2);

	imshow("frame camera 1", resizedFrames.first);
	imshow("frame camera 2", resizedFrames.second);
	cv::waitKey(1);
	if (circles1.size() == circles2.size())
	{
		ROIs.first.clear();
		ROIs.second.clear();
		if (circles1.size() == 10)
		{
			for (int i = 0; i < 10; i++)
			{
				cv::Rect trackedArea1(circles1[i](0) - 4 * circles1[i](2), circles1[i](1) - 4 * circles1[i](2), 8 * circles1[i](2), 8 * circles1[i](2));
				cv::Rect trackedArea2(circles2[i](0) - 4 * circles2[i](2), circles2[i](1) - 4 * circles2[i](2), 8 * circles2[i](2), 8 * circles2[i](2));
				ROIs.first.push_back(trackedArea1);
				ROIs.second.push_back(trackedArea2);
			}
			return true;
		}		
	}
	return false;
}


extern "C" void __declspec(dllexport) __stdcall recordedTrackMarkers(ImgProcUtility::Coordinates * outBalls, int maxOutBallsCount, int& outDetectedBallsCount) // w parametrach powinno byc jeszcze frames.first i frames.second, tak zeby mozna bylo wyswietlac je w unity
{
	cv::Mat firstCameraFrame, secondCameraFrame, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2;

	std::vector<cv::Vec3f> v3fCircles1, v3fCircles2;

	std::pair<cv::Mat, cv::Mat> frames = ImgProcUtility::readFrames(firstSequence, secondSequence);

	if (frames.first.empty() || frames.second.empty())
	{
		multiTrackers.first.release();
		multiTrackers.second.release();
		firstSequence.release();
		secondSequence.release();
		cv::destroyAllWindows();
		return;
	}
	std::pair<cv::Mat, cv::Mat> resizedFrames = ImgProcUtility::resizeFrames(frames, 0.5);

	if (firstFrame == true)
	{
		//ROIs = ImgProcUtility::selectMarkers(resizedFrames, multiTrackers);
		for (int i = 0; i < ROIs.first.size(); i++)
		{
			multiTrackers.first->add(cv::legacy::TrackerMOSSE::create(), resizedFrames.first, ROIs.first[i]);
			multiTrackers.second->add(cv::legacy::TrackerMOSSE::create(), resizedFrames.second, ROIs.second[i]);
		}
		firstFrame = false;
	}
	ImgProcUtility::updateTrackers(multiTrackers, resizedFrames);
	std::pair<cv::Mat, cv::Mat> grayFrames = ImgProcUtility::convertFramesToGray(resizedFrames);

	StereoCoordinates2D coordinates2D = ImgProcUtility::getMarkersCoordinates2D(grayFrames, multiTrackers, resizedFrames);
	cv::Mat triangCoords = ImgProcUtility::process2DCoordinates(coordinates2D, matrices);
	getMarkersCoordinates3D(triangCoords, outBalls, outDetectedBallsCount);
	displayDistanceBetweenMarkers(resizedFrames.first, outBalls, 0, 1);

	imshow("frame camera 1", resizedFrames.first);
	imshow("frame camera 2", resizedFrames.second);
	cv::waitKey(1);
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	firstSequence.release();
	secondSequence.release();
}