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
#include"ImgProcUtility.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL.h>
#include "ps3eye.h"
//in case cuda is needed
//#include<opencv2/cudaarithm.hpp>
//#include<opencv2/cuda.hpp>
//#include<opencv2/cudabgsegm.hpp>
//#include<opencv2/cudacodec.hpp>
//#include<opencv2/cudafeatures2d.hpp>
//#include<opencv2/cudafilters.hpp>
//#include<opencv2/cudaimgproc.hpp>
//#include<opencv2/cudaobjdetect.hpp>
//#include<opencv2/cudaoptflow.hpp>
//#include<opencv2/cudastereo.hpp>
//#include<opencv2/cudawarping.hpp>

Matrices matrices;
cv::VideoCapture firstSequence("../MonitoringCPR/images/movement/seria2/camera1/video1.avi");
cv::VideoCapture secondSequence("../MonitoringCPR/images/movement/seria2/camera2/video2.avi");

std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multiTrackers;
StereoROISets ROIs;

bool firstFrame;
int _scale = 1;

ps3eye::PS3EYECam::PS3EYERef cam1 = NULL;
ps3eye::PS3EYECam::PS3EYERef cam2 = NULL;

std::pair<cv::Mat, cv::Mat> currentFrames;

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

	multiTrackers.first = cv::MultiTracker::create();
	multiTrackers.second = cv::MultiTracker::create();

	// Open the stream.

	firstSequence.open("C:/Users/Kalin/Desktop/studia/in�ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera1/video1.avi");
	secondSequence.open("C:/Users/Kalin/Desktop/studia/in�ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera2/video2.avi");

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
using namespace ImgProcUtility;
extern "C" void __declspec(dllexport) __stdcall GetCurrentFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	std::pair<cv::Mat, cv::Mat> frames = readFrames(firstSequence, secondSequence);

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
	cv::cvtColor(firstResizedMat, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(secondResizedMat, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall Detect(Coordinates * outBalls, int maxOutBallsCount, int& outDetectedBallsCount) // w parametrach powinno byc jeszcze frames.first i frames.second, tak zeby mozna bylo wyswietlac je w unity
{
	cv::Mat firstCameraFrame, secondCameraFrame, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2;

	std::vector<cv::Vec3f> v3fCircles1, v3fCircles2;

	std::pair<cv::Mat, cv::Mat> frames = readFrames(firstSequence, secondSequence);

	if (frames.first.empty() || frames.second.empty())
	{
		multiTrackers.first.release();
		multiTrackers.second.release();
		firstSequence.release();
		secondSequence.release();
		cv::destroyAllWindows();
		return;
	}
	std::pair<cv::Mat, cv::Mat> resizedFrames = resizeFrames(frames, 0.5);
	if (firstFrame == true)
	{
		ROIs = selectMarkers(resizedFrames, multiTrackers);
		firstFrame = false;
	}
	updateTrackers(multiTrackers, resizedFrames);
	std::pair<cv::Mat, cv::Mat> grayFrames = convertFramesToGray(resizedFrames);

	StereoCoordinates2D coordinates2D = getMarkersCoordinates2D(grayFrames, multiTrackers, resizedFrames);
	cv::Mat triangCoords = process2DCoordinates(coordinates2D, matrices);
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

extern "C" int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
{
	auto& devices = ps3eye::PS3EYECam::getDevices(true);
	if (devices.empty())
	{
		return -1;
	}
	cam1 = devices[0];
	cam2 = devices[1];
	bool success1 = cam1->init(640, 480, 60);
	bool success2 = cam2->init(640, 480, 60);
	if (!success1 || !success2)
	{
		return -2;
	}
	cam1->start();
	cam2->start();
	return 0;
}

extern "C" void __declspec(dllexport) __stdcall GetSDLCurrentFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
{
	cv::Mat frame1 = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
	cv::Mat frame2 = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

	cam1->getFrame(frame1.data);
	cam2->getFrame(frame2.data);

	cv::Mat firstResizedMat(height, width, frame1.type());
	cv::Mat secondResizedMat(height, width, frame2.type());

	cv::resize(frame1, firstResizedMat, firstResizedMat.size(), cv::INTER_CUBIC);
	cv::resize(frame2, secondResizedMat, secondResizedMat.size(), cv::INTER_CUBIC);

	currentFrames.first = firstResizedMat;
	currentFrames.second = secondResizedMat;

	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(firstResizedMat, firstArgbImg, CV_BGR2RGBA);
	cv::cvtColor(secondResizedMat, secondArgbImg, CV_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}

extern "C" void __declspec(dllexport) __stdcall showCurrentFrames()
{
	cv::imshow("e", currentFrames.first);
	cv::imshow("edd", currentFrames.second);
	cv::waitKey(1);
}
extern "C" void __declspec(dllexport) __stdcall  CloseSDLCameras()
{
	cam1->stop();
	cam2->stop();
	cam1 = NULL;
	cam2 = NULL;
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
	cv::imwrite("../MonitoringCPR/images/UnityTest/CAM1_" + timestamp + ".jpg", currentFrames.first);
	cv::imwrite("../MonitoringCPR/images/UnityTest/CAM2_" + timestamp + ".jpg", currentFrames.second);
}

