#include"ImgProcUtility.h"

StereoFrames ImgProcUtility::readFrames(VideoCapture firstSequence, VideoCapture secondSequence)
{
	StereoFrames frames;
	firstSequence >> frames.first;
	secondSequence >> frames.second;
	return frames;
}

StereoFrames ImgProcUtility::resizeFrames(StereoFrames frames, double scale)
{
	StereoFrames resizedFrames;
	resize(frames.first, resizedFrames.first, Size(), 0.5, 0.5);
	resize(frames.second, resizedFrames.second, Size(), 0.5, 0.5);
	return resizedFrames;
}

void ImgProcUtility::selectMarkers(bool& condition, StereoFrames frames, std::vector<cv::Rect>& firstROIs, std::vector<cv::Rect>& secondROIs, cv::Ptr<cv::MultiTracker> firstMultiTracker, cv::Ptr<cv::MultiTracker> secondMultiTracker)
{	
	if (condition == true)
	{
		condition = false;

		// ponizsze wrzucic do funkcji choose markers w utility
		selectROIs("firstMultiTracker", frames.first, firstROIs);

		for (int i = 0; i < firstROIs.size(); i++)
		{
			firstMultiTracker->add(cv::TrackerCSRT::create(), frames.first, cv::Rect2d(firstROIs[i]));
		}

		selectROIs("secondMultiTracker", frames.second, secondROIs);

		for (int i = 0; i < secondROIs.size(); i++)
		{
			secondMultiTracker->add(cv::TrackerCSRT::create(), frames.second, cv::Rect2d(secondROIs[i]));
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

int ImgProcUtility::getMarkersCoordinates2D(Mat gray1, Mat gray2, std::vector<cv::Rect> firstROIs, std::vector<cv::Rect> secondROIs, cv::Ptr<cv::MultiTracker> firstMultiTracker, 
	cv::Ptr<cv::MultiTracker> secondMultiTracker, Mat& firstCamFrame, Mat& secondCamFrame, std::vector<cv::Vec2f>& firstCamCoordinates2D, std::vector<cv::Vec2f>& secondCamCoordinates2D)
{
	Mat croppedImg1, croppedImg2, threshImg1, threshImg2;
	vector<Vec3f> v3fCircles1, v3fCircles2;
	int detectedMarkers = 0;

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
			++detectedMarkers;
		}
		ImgProcUtility::drawRectAroundROI(firstCamFrame, secondCamFrame, firstMultiTracker->getObjects()[i], secondMultiTracker->getObjects()[i], cv::Scalar(255, 0, 0), 2, 1);
	}
	return detectedMarkers;
}

Mat ImgProcUtility::process2DCoordinates(std::vector<cv::Vec2f> firstCamCoordinates2D, std::vector<cv::Vec2f> secondCamCoordinates2D, Mat firstCamMatrix, Mat secondCamMatrix,
		Mat firstCamCoeffs, Mat secondCamCoeffs, Mat R1, Mat R2, Mat P1, Mat P2)
{
	Mat distCoords1 = Mat(Size(2, firstCamCoordinates2D.size()), CV_64FC1);      		//declaring distorded coordinates
	Mat distCoords2 = Mat(Size(2, secondCamCoordinates2D.size()), CV_64FC1);
	ImgProcUtility::populateMatriceFromVector(firstCamCoordinates2D, secondCamCoordinates2D, distCoords1, distCoords2);

	vector<Vec2d> undistCoords1, undistCoords2;
	undistortPoints(distCoords1, undistCoords1, firstCamMatrix, firstCamCoeffs, R1, P1);         //undistorting the coordinates
	undistortPoints(distCoords2, undistCoords2, secondCamMatrix, secondCamCoeffs, R2, P2);

	Mat triangCoords;/* = Mat(Size(firstCamCoordinates2D.size(), 4), CV_64FC1);*/
	triangulatePoints(P1, P2, undistCoords1, undistCoords2, triangCoords);     //triangulation (not that much effect on fps (probably no effect)
	return triangCoords;
}

void ImgProcUtility::getMarkersCoordinates3D(Mat triangCoords, Coordinates* outBalls, int& outDetectedBallsCount)
{
	for (int i = 0; i < triangCoords.cols; ++i) // for different balls
	{
		outBalls[i].X = triangCoords.at<double>(0, i) / triangCoords.at<double>(3, i);
		outBalls[i].Y = triangCoords.at<double>(1, i) / triangCoords.at<double>(3, i);
		outBalls[i].Z = triangCoords.at<double>(2, i) / triangCoords.at<double>(3, i);

		++outDetectedBallsCount;
	}
}

void ImgProcUtility::populateMatriceFromVector(std::vector<cv::Vec2f> firstCamCoordinates2D, std::vector<cv::Vec2f> secondCamCoordinates2D, Mat& firstCamDistCoords, Mat& secondCamDistCoords)
{
	for (int i = 0; i < firstCamCoordinates2D.size(); ++i)
	{
		firstCamDistCoords.at<double>(i, 0) = firstCamCoordinates2D[i](0);
		firstCamDistCoords.at<double>(i, 1) = firstCamCoordinates2D[i](1);

		secondCamDistCoords.at<double>(i, 0) = secondCamCoordinates2D[i](0);
		secondCamDistCoords.at<double>(i, 1) = secondCamCoordinates2D[i](1);
	}
}

double ImgProcUtility::calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{
	return sqrt(pow(outBalls[firstMarkerId].X - outBalls[secondMarkerId].X, 2) + pow(outBalls[firstMarkerId].Y - outBalls[secondMarkerId].Y, 2) 
		+ pow(outBalls[firstMarkerId].Z - outBalls[secondMarkerId].Z, 2));
}

void ImgProcUtility::displayDistanceBetweenMarkers(Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{	
	cv::putText(displayMatrix, ("Distance between markers " + to_string(firstMarkerId) + " and " + to_string(secondMarkerId) +": "+ to_string(calculateDistanceBetweenMarkers(outBalls, firstMarkerId, secondMarkerId))),
		Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
}