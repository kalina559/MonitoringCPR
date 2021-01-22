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

Matrices matrices;
//paths to two series of images

VideoCapture firstSequence("../MonitoringCPR/images/movement/seria2/camera1/video1.avi");
VideoCapture secondSequence("../MonitoringCPR/images/movement/seria2/camera2/video2.avi");

pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multiTrackers;
StereoROISets ROIs;

bool firstFrame;
int _scale = 1;
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
	Mat firstCameraFrame, secondCameraFrame, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2;

	vector<Vec3f> v3fCircles1, v3fCircles2;

	firstSequence >> firstCameraFrame;
	secondSequence >> secondCameraFrame;
	pair<Mat, Mat> frames = readFrames(firstSequence, secondSequence);

	if (frames.first.empty() || frames.second.empty())
	{
		multiTrackers.first.release();
		multiTrackers.second.release();
		firstSequence.release();
		secondSequence.release();
		cv::destroyAllWindows();
		return;
	}
	pair<Mat, Mat> resizedFrames = resizeFrames(frames, 0.5);
	if (firstFrame == true)
	{
		ROIs = selectMarkers(resizedFrames, multiTrackers);
		firstFrame = false;
	}
	updateTrackers(multiTrackers, resizedFrames);
	pair<Mat, Mat> grayFrames = convertFramesToGray(resizedFrames);

	StereoCoordinates2D coordinates2D = getMarkersCoordinates2D(grayFrames, multiTrackers, resizedFrames);	
	Mat triangCoords = process2DCoordinates(coordinates2D, matrices);
	getMarkersCoordinates3D(triangCoords, outBalls, outDetectedBallsCount);
	displayDistanceBetweenMarkers(resizedFrames.first, outBalls, 0, 1);

	imshow("frame camera 1", resizedFrames.first);
	imshow("frame camera 2", resizedFrames.second);
	waitKey(1);
}