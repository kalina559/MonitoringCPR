#include"ImgProcUtility.h"

void ImgProcUtility::readFrames(VideoCapture firstSequence, VideoCapture secondSequence, Mat& firstCamFrame, Mat& secondCamFrame)
{
	firstSequence >> firstCamFrame;
	secondSequence >> secondCamFrame;
}

void ImgProcUtility::resizeFrames(Mat firstCamFrame, Mat secondCamFrame, Mat& firstResized, Mat& secondResized, double scale)
{
	resize(firstCamFrame, firstResized, Size(), 0.5, 0.5);
	resize(secondCamFrame, secondResized, Size(), 0.5, 0.5);
}

void ImgProcUtility::selectMarkers(bool& condition, Mat firstCamFrame, Mat secondCamFrame, std::vector<cv::Rect>& firstROIs, std::vector<cv::Rect>& secondROIs,
	cv::Ptr<cv::MultiTracker> firstMultiTracker, cv::Ptr<cv::MultiTracker> secondMultiTracker)
{
	if (condition == true)
	{
		condition = false;

		// ponizsze wrzucic do funkcji choose markers w utility
		selectROIs("firstMultiTracker", firstCamFrame, firstROIs);

		for (int i = 0; i < firstROIs.size(); i++)
		{
			firstMultiTracker->add(cv::TrackerCSRT::create(), firstCamFrame, cv::Rect2d(firstROIs[i]));
		}

		selectROIs("secondMultiTracker", secondCamFrame, secondROIs);

		for (int i = 0; i < secondROIs.size(); i++)
		{
			secondMultiTracker->add(cv::TrackerCSRT::create(), secondCamFrame, cv::Rect2d(secondROIs[i]));
		}
	}
}

void ImgProcUtility::updateTrackers(cv::Ptr<cv::MultiTracker> firstMultiTracker, cv::Ptr<cv::MultiTracker> secondMultiTracker, Mat firstCamFrame, Mat secondCamFrame)
{
	firstMultiTracker->update(firstCamFrame);
	secondMultiTracker->update(secondCamFrame);
}

void ImgProcUtility::convertFramesToGray(Mat firstCamFrame, Mat secondCamFrame, Mat& firstCamGray, Mat& secondCamGray)
{
	cvtColor(firstCamFrame, firstCamGray, COLOR_BGR2GRAY);
	cvtColor(secondCamFrame, secondCamGray, COLOR_BGR2GRAY);
}

void ImgProcUtility::cutROIsFromFrames(Mat& croppedImg1, Mat& croppedImg2, Mat gray1, Mat gray2, Rect2d firstROI, Rect2d secondROI)
{
	croppedImg1 = gray1(firstROI);          // cropping ROIs to see the analysed part of the image	
	croppedImg2 = gray2(secondROI);  
}

void ImgProcUtility::thresholdImages(Mat& threshImg1, Mat& threshImg2, Mat croppedImg1, Mat croppedImg2, double thresh, double maxval, int type)
{
	threshold(croppedImg1, threshImg1, thresh, maxval, type);
	threshold(croppedImg2, threshImg2, thresh, maxval, type);
}

void ImgProcUtility::drawCirclesAroundMarkers(Mat& firstCamFrame, Mat& secondCamFrame, Point firstCenter, Point secondCenter, int firstRadius, int secondRadius, Scalar color)
{
	circle(firstCamFrame, firstCenter, firstRadius, color);
	circle(secondCamFrame, secondCenter, secondRadius, color);
}

void ImgProcUtility::drawRectAroundROI(Mat& firstCamFrame, Mat& secondCamFrame, Rect firstCamTrackedArea, Rect secondCamTrackedArea, Scalar color, int thickness, int linetype)
{
	rectangle(firstCamFrame, firstCamTrackedArea, color, thickness, linetype);
	rectangle(secondCamFrame, secondCamTrackedArea, color, thickness, linetype);
}

void ImgProcUtility::findCirclesInROIs(vector<Vec3f>& firstCamCircles, vector<Vec3f>& secondCamCircles, Mat threshImg1, Mat threshImg2)
{
	cv::HoughCircles(threshImg1, firstCamCircles, cv::HOUGH_GRADIENT, 2, threshImg1.rows, 150, 10);
	cv::HoughCircles(threshImg2, secondCamCircles, cv::HOUGH_GRADIENT, 2, threshImg2.rows, 150, 10);          //do utility
}

void ImgProcUtility::getMarkersCoordinates2D(Mat gray1, Mat gray2, std::vector<cv::Rect> firstROIs, std::vector<cv::Rect> secondROIs, cv::Ptr<cv::MultiTracker> firstMultiTracker, 
	cv::Ptr<cv::MultiTracker> secondMultiTracker, Mat& firstCamFrame, Mat& secondCamFrame, std::vector<cv::Vec2f>& firstCamCoordinates2D, std::vector<cv::Vec2f>& secondCamCoordinates2D)
{
	Mat croppedImg1, croppedImg2, threshImg1, threshImg2;
	vector<Vec3f> v3fCircles1, v3fCircles2;

	for (unsigned int i = 0; i < firstROIs.size(); ++i)
	{
		ImgProcUtility::cutROIsFromFrames(croppedImg1, croppedImg2, gray1, gray2, firstMultiTracker->getObjects()[i], secondMultiTracker->getObjects()[i]);
		ImgProcUtility::thresholdImages(threshImg1, threshImg2, croppedImg1, croppedImg2, 150, 255, THRESH_BINARY);
	//stuff above has the biggest effect on fps
		ImgProcUtility::findCirclesInROIs(v3fCircles1, v3fCircles2, threshImg1, threshImg2);

		if (v3fCircles1.size() == 1 && v3fCircles2.size() == 1)       //only one circle should be found in a single ROI, no effect on fps
		{
			ImgProcUtility::drawCirclesAroundMarkers(firstCamFrame, secondCamFrame,
				Point(v3fCircles1[0](0) + firstMultiTracker->getObjects()[i].x, v3fCircles1[0](1) + firstMultiTracker->getObjects()[i].y),
				Point(v3fCircles2[0](0) + secondMultiTracker->getObjects()[i].x, v3fCircles2[0](1) + secondMultiTracker->getObjects()[i].y),
				v3fCircles1[0](2), v3fCircles2[0](2), cv::Scalar(0, 0, 255));

			firstCamCoordinates2D[i](0) = v3fCircles1[0](0) + firstMultiTracker->getObjects()[i].x;
			firstCamCoordinates2D[i](1) = v3fCircles1[0](1) + firstMultiTracker->getObjects()[i].y;
			secondCamCoordinates2D[i](0) = v3fCircles2[0](0) + secondMultiTracker->getObjects()[i].x;
			secondCamCoordinates2D[i](1) = v3fCircles2[0](1) + secondMultiTracker->getObjects()[i].y;
		}
		ImgProcUtility::drawRectAroundROI(firstCamFrame, secondCamFrame, firstMultiTracker->getObjects()[i], secondMultiTracker->getObjects()[i], cv::Scalar(255, 0, 0), 2, 1);
	}
}

void ImgProcUtility::getMarkersCoordinates3D(Mat triangCoords, ImgProcUtility::Coordinates* outBalls, vector<Vec3d>& coords3D, int& outDetectedBallsCount)
{
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
}