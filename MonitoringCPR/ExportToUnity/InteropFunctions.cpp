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

//loading matrices from files


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
	Mat firstCameraFrame, secondCameraFrame, firstCamFrameResized, secondCamFrameResized, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2, gray1, gray2;

	vector<Vec3f> v3fCircles1, v3fCircles2;

	firstSequence >> firstCameraFrame;
	secondSequence >> secondCameraFrame;
	readFrames(firstSequence, secondSequence, firstCameraFrame, secondCameraFrame);

	if (firstCameraFrame.empty() || secondCameraFrame.empty())
	{
		firstMultiTracker.release();
		secondMultiTracker.release();
		firstSequence.release();
		secondSequence.release();
		cv::destroyAllWindows();
		return;
	}

	resizeFrames(firstCameraFrame, secondCameraFrame, firstCamFrameResized, secondCamFrameResized, 0.5);
	selectMarkers(firstFrame, firstCamFrameResized, secondCamFrameResized, firstROIs, secondROIs, firstMultiTracker, secondMultiTracker);

	std::vector<cv::Vec2f> firstCamCoordinates2D(firstROIs.size()), secondCamCoordinates2D(secondROIs.size());

	updateTrackers(firstMultiTracker, secondMultiTracker, firstCamFrameResized, secondCamFrameResized);
	convertFramesToGray(firstCamFrameResized, secondCamFrameResized, gray1, gray2);

	getMarkersCoordinates2D(gray1, gray2, firstROIs, secondROIs, firstMultiTracker, secondMultiTracker, firstCamFrameResized, secondCamFrameResized, firstCamCoordinates2D, secondCamCoordinates2D);

	if (firstCamCoordinates2D.size() == secondCamCoordinates2D.size())
	{
		//declaring distorded coordinates
		Mat distCoords1 = Mat(Size(2, firstCamCoordinates2D.size()), CV_64FC1);;
		Mat distCoords2 = Mat(Size(2, secondCamCoordinates2D.size()), CV_64FC1);;

		for (int i = 0; i < firstCamCoordinates2D.size(); ++i)
		{
			distCoords1.at<double>(i, 0) = firstCamCoordinates2D[i](0);
			distCoords1.at<double>(i, 1) = firstCamCoordinates2D[i](1);

			distCoords2.at<double>(i, 0) = secondCamCoordinates2D[i](0);
			distCoords2.at<double>(i, 1) = secondCamCoordinates2D[i](1);
		}
		//undistorting the coordinates
		vector<Vec2d> undistCoords1, undistCoords2;
		Mat triangCoords = Mat(Size(firstCamCoordinates2D.size(), 4), CV_64FC1);

		undistortPoints(distCoords1, undistCoords1, firstCamMatrix, firstCamCoeffs, R1, P1);
		undistortPoints(distCoords2, undistCoords2, secondCamMatrix, secondCamCoeffs, R2, P2);

		//triangulation (not that much effect on fps (probably no effect)
		triangulatePoints(P1, P2, undistCoords1, undistCoords2, triangCoords);
		vector<Vec3d> coords3D(triangCoords.cols);
		getMarkersCoordinates3D(triangCoords, outBalls, coords3D, outDetectedBallsCount);

		//---sprawdzanie odleglosci miedzy wybranymi markerami i wyswietlanie
		double distance;
		distance = sqrt(pow(coords3D[0](0) - coords3D[1](0), 2) + pow(coords3D[0](1) - coords3D[1](1), 2)
			+ pow(coords3D[0](2) - coords3D[1](2), 2));

		cv::putText(firstCamFrameResized, "dist: " + to_string(distance), Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
		//-------
	}
	else
	{
		cout << "Not enough balls have been detected" << endl;
		cout << firstCamCoordinates2D.size() << " " << secondCamCoordinates2D.size() << " " << firstROIs.size();
	}
	imshow("frame camera 1", firstCamFrameResized);
	imshow("frame camera 2", secondCamFrameResized);
	waitKey(1);


}