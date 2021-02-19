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




// build into .dll file used in a unity project
using namespace std;
using namespace cv;

struct Coordinates
{
	Coordinates(float x, float y, float z) : X(x), Y(y), Z(z) {}
	float X, Y, Z;
};

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

VideoCapture sequence1("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera1/video1.avi");
VideoCapture sequence2("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera2/video2.avi");

cv::Ptr<cv::MultiTracker> multiTracker1;
cv::Ptr<cv::MultiTracker> multiTracker2;

std::vector<cv::Rect> bboxes1, bboxes2;


int firstFrame;
int _scale = 1;

extern "C" int __declspec(dllexport) __stdcall  InteropClass::Init(int& outCameraWidth, int& outCameraHeight)
{
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamMatrix", 3, 3, firstCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamMatrix", 3, 3, secondCamMatrix);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/firstCamCoeffs", 5, 1, firstCamCoeffs);
	CameraCalibration::loadMatrix("matrices/singleCamCalibration/secondCamCoeffs", 5, 1, secondCamCoeffs);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P1", 3, 4, P1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/P2", 3, 4, P2);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R1", 3, 3, R1);
	CameraCalibration::loadMatrix("matrices/stereoRectifyResults/R2", 3, 3, R2);

	multiTracker1 = cv::MultiTracker::create();
	multiTracker2 = cv::MultiTracker::create();

	// Open the stream.

	sequence1.open("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera1/video1.avi");
	sequence2.open("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/zdjeciaMarkerow/seria2/camera2/video2.avi");

	if (!sequence1.isOpened() || !sequence2.isOpened())
	{
		return -2;
	}
	firstFrame = 1;
	outCameraWidth = sequence1.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = sequence1.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	sequence1.release();
	sequence2.release();
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}

extern "C" void __declspec(dllexport) __stdcall Detect(Coordinates* outBalls, int maxOutBallsCount, int& outDetectedBallsCount)
{
	Mat frame1, frame2, resized1, resized2, croppedImg1, croppedImg2,
		threshImg1, threshImg2, croppedColor1, croppedColor2, gray1, gray2;

	vector<Vec3f> v3fCircles1, v3fCircles2;

	//read the frames
	sequence1 >> frame1;
	sequence2 >> frame2;

	if (frame1.empty() || frame2.empty())
	{
		multiTracker1.release();
		multiTracker2.release();
		sequence1.release();
		sequence2.release();
		cv::destroyAllWindows();
		return;
	}

	resize(frame1, resized1, Size(), 0.5, 0.5);
	resize(frame2, resized2, Size(), 0.5, 0.5);

	if (firstFrame == 1)
	{
		firstFrame = 0;

		selectROIs("MultiTracker1", resized1, bboxes1);

		for (int i = 0; i < bboxes1.size(); i++)
		{
			multiTracker1->add(cv::TrackerCSRT::create(), resized1, cv::Rect2d(bboxes1[i]));
		}

		selectROIs("MultiTracker2", resized2, bboxes2);

		for (int i = 0; i < bboxes2.size(); i++)
		{
			multiTracker2->add(cv::TrackerCSRT::create(), resized2, cv::Rect2d(bboxes2[i]));
		}


	}

	std::vector<cv::Vec2f> balls1(bboxes1.size()), balls2(bboxes2.size());

	multiTracker1->update(resized1);
	multiTracker2->update(resized2);



	//converting into BGR to show the detected circles in color
	cvtColor(resized1, gray1, COLOR_BGR2GRAY);
	cvtColor(resized2, gray2, COLOR_BGR2GRAY);

	for (unsigned int i = 0; i < bboxes1.size(); ++i)
	{

		croppedImg1 = gray1(multiTracker1->getObjects()[i]);          // cropping ROIs to see the analysed part of the image	
		croppedImg2 = gray2(multiTracker2->getObjects()[i]);

		threshold(croppedImg1, threshImg1, 150, 255, THRESH_BINARY);
		threshold(croppedImg2, threshImg2, 150, 255, THRESH_BINARY);

		//stuff above has the biggest effect on fps

		cv::HoughCircles(threshImg1, v3fCircles1, cv::HOUGH_GRADIENT, 2, threshImg1.rows, 150, 10);
		cv::HoughCircles(threshImg2, v3fCircles2, cv::HOUGH_GRADIENT, 2, threshImg2.rows, 150, 10);

		if (v3fCircles1.size() == 1 && v3fCircles2.size() == 1)       //only one circle should be found in a single ROI, no effect on fps
		{

			//drawing circles in the whole frame
			circle(resized1, Point(v3fCircles1[0](0) + multiTracker1->getObjects()[i].x, v3fCircles1[0](1) + multiTracker1->getObjects()[i].y),
				v3fCircles1[0](2), cv::Scalar(0, 0, 255), 1);
			circle(resized2, Point(v3fCircles2[0](0) + multiTracker2->getObjects()[i].x, v3fCircles2[0](1) + multiTracker2->getObjects()[i].y),
				v3fCircles2[0](2), cv::Scalar(0, 0, 255), 1);

			balls1[i](0) = v3fCircles1[0](0) + multiTracker1->getObjects()[i].x;
			balls1[i](1) = v3fCircles1[0](1) + multiTracker1->getObjects()[i].y;
			balls2[i](0) = v3fCircles2[0](0) + multiTracker2->getObjects()[i].x;
			balls2[i](1) = v3fCircles2[0](1) + multiTracker2->getObjects()[i].y;
			// saves all coordinates of all the balls in chosen ROIs
		}
		//draw the tracked ROIs
		rectangle(resized1, multiTracker1->getObjects()[i], cv::Scalar(255, 0, 0), 2, 1);
		rectangle(resized2, multiTracker2->getObjects()[i], cv::Scalar(255, 0, 0), 2, 1);
	}

	if (balls1.size() == balls2.size() && balls2.size() == bboxes1.size())
	{
		//declaring distorded coordinates
		Mat distCoords1 = Mat(Size(2, balls1.size()), CV_64FC1);;
		Mat distCoords2 = Mat(Size(2, balls2.size()), CV_64FC1);;

		for (int i = 0; i < balls1.size(); ++i)
		{
			distCoords1.at<double>(i, 0) = balls1[i](0);
			distCoords1.at<double>(i, 1) = balls1[i](1);

			distCoords2.at<double>(i, 0) = balls2[i](0);
			distCoords2.at<double>(i, 1) = balls2[i](1);
		}
		//undistorting the coordinates
		vector<Vec2d> undistCoords1, undistCoords2;
		Mat triangCoords = Mat(Size(balls1.size(), 4), CV_64FC1);

		undistortPoints(distCoords1, undistCoords1, firstCamMatrix, firstCamCoeffs, R1, P1);
		undistortPoints(distCoords2, undistCoords2, secondCamMatrix, secondCamCoeffs, R2, P2);


		//triangulation (not that much effect on fps (probably no effect)
		triangulatePoints(P1, P2, undistCoords1, undistCoords2, triangCoords);

		//tu dziala

		vector<Vec3d> coords3D(triangCoords.cols);

		for (int i = 0; i < triangCoords.cols; ++i) // for different balls
		{
			for (int j = 0; j < 3; ++j) // moving through cols
			{
				coords3D[i](j) = triangCoords.at<double>(j, i) / triangCoords.at<double>(3, i);
			}
			outBalls[i].X = coords3D[i](0);
			outBalls[i].Y = coords3D[i](1);
			outBalls[i].Z = coords3D[i](2);

			++outDetectedBallsCount;

		}

		double distance;
		distance = sqrt(pow(coords3D[0](0) - coords3D[1](0), 2) + pow(coords3D[0](1) - coords3D[1](1), 2)
			+ pow(coords3D[0](2) - coords3D[1](2), 2));

		cv::putText(resized1, "dist: " + to_string(distance), Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
	}
	else
	{
		cout << "Not enough balls have been detected" << endl;
		cout << balls1.size() << " " << balls2.size() << " " << bboxes1.size();
	}
	imshow("frame camera 1", resized1);
	imshow("frame camera 2", resized2);
	waitKey(1);


}