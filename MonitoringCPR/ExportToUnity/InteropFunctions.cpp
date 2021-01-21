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


using namespace std;
using namespace cv;

//declaring matrices that'll be loaded from text files

Mat firstCamMatrix = Mat(Size(3, 3), CV_64FC1);
Mat secondCamMatrix = Mat(Size(3, 3), CV_64FC1);
Mat firstCamCoeffs = Mat(Size(1, 5), CV_64FC1);
Mat secondCamCoeffs = Mat(Size(1, 5), CV_64FC1);
Mat P1 = Mat(Size(4, 3), CV_64FC1);
Mat P2 = Mat(Size(4, 3), CV_64FC1);
Mat R1 = Mat(Size(3, 3), CV_64FC1);
Mat R2 = Mat(Size(3, 3), CV_64FC1);

//paths to two series of images

VideoCapture firstSequence("../MonitoringCPR/images/movement/seria2/camera1/video1.avi");
VideoCapture secondSequence("../MonitoringCPR/images/movement/seria2/camera2/video2.avi");

cv::Ptr<cv::MultiTracker> firstMultiTracker;
cv::Ptr<cv::MultiTracker> secondMultiTracker;

std::vector<cv::Rect> firstROIs, secondROIs;


bool firstFrame;
int _scale = 1;
extern "C" int __declspec(dllexport) __stdcall Init(int& outCameraWidth, int& outCameraHeight)
{
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamMatrix", 3, 3, firstCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamMatrix", 3, 3, secondCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamCoeffs", 5, 1, firstCamCoeffs);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamCoeffs", 5, 1, secondCamCoeffs);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P1", 3, 4, P1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P2", 3, 4, P2);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R1", 3, 3, R1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R2", 3, 3, R2);

	firstMultiTracker = cv::MultiTracker::create();
	secondMultiTracker = cv::MultiTracker::create();

	// Open the stream.

	firstSequence.open("C:/Users/Kalin/Desktop/studia/in¿ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera1/video1.avi");
	secondSequence.open("C:/Users/Kalin/Desktop/studia/in¿ynierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera2/video2.avi");

	if (!firstSequence.isOpened() || !secondSequence.isOpened())
	{
		return -2;
	}
	firstFrame = true;
	outCameraWidth = firstSequence.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = firstSequence.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	firstSequence.release();
	secondSequence.release();
	
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}


//TODO clean this shit
using namespace ImgProcUtility;
extern "C" void __declspec(dllexport) __stdcall Detect(Coordinates* outBalls, int maxOutBallsCount, int& outDetectedBallsCount)
{
	Mat firstCameraFrame, secondCameraFrame,  croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2, gray1, gray2;

	vector<Vec3f> v3fCircles1, v3fCircles2;

	firstSequence >> firstCameraFrame;
	secondSequence >> secondCameraFrame;
	StereoFrames frames = readFrames(firstSequence, secondSequence);

	if (frames.first.empty() || frames.second.empty())
	{
		firstMultiTracker.release();
		secondMultiTracker.release();
		firstSequence.release();
		secondSequence.release();
		cv::destroyAllWindows();
		return;
	}
	StereoFrames resizedFrames = resizeFrames(frames, 0.5);
	selectMarkers(firstFrame, resizedFrames, firstROIs, secondROIs, firstMultiTracker, secondMultiTracker);

	std::vector<cv::Vec2f> firstCamCoordinates2D(firstROIs.size()), secondCamCoordinates2D(secondROIs.size());

	updateTrackers(firstMultiTracker, secondMultiTracker, resizedFrames.first, resizedFrames.second);
	convertFramesToGray(resizedFrames.first, resizedFrames.second, gray1, gray2);

	if (getMarkersCoordinates2D(gray1, gray2, firstROIs, secondROIs, firstMultiTracker, secondMultiTracker,
		resizedFrames.first, resizedFrames.second, firstCamCoordinates2D, secondCamCoordinates2D) == firstROIs.size()) //jeœli znaleziona jest oczekiwana liczba wspó³rzêdnych
	{		
		Mat triangCoords = process2DCoordinates(firstCamCoordinates2D, secondCamCoordinates2D, firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs, R1, R2, P1, P2);
		getMarkersCoordinates3D(triangCoords, outBalls, outDetectedBallsCount);
		displayDistanceBetweenMarkers(resizedFrames.first, outBalls, 0, 1);
	}
	imshow("frame camera 1", resizedFrames.first);
	imshow("frame camera 2", resizedFrames.second);
	waitKey(1);
}