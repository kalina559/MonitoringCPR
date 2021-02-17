#include"ImgProcUtility.h"
int ImgProcUtility::initializeCameras(realTimeCapturePair& stereoCapture)
{
	auto& devices = ps3eye::PS3EYECam::getDevices(true);
	if (devices.size() != 2)
	{
		return -1;
	}
	stereoCapture.getFirstCapture().setCamera(devices[0]);
	stereoCapture.getSecondCapture().setCamera(devices[1]);
	bool success1 = stereoCapture.getFirstCapture().getCamera()->init(640, 480, 60);
	bool success2 = stereoCapture.getSecondCapture().getCamera()->init(640, 480, 60);
	if (!success1 || !success2)
	{
		return -2;
	}
	stereoCapture.getFirstCapture().getCamera()->start();
	stereoCapture.getSecondCapture().getCamera()->start();
	return 0;
}
std::pair<cv::Mat, cv::Mat> ImgProcUtility::readFrames(cv::VideoCapture firstSequence, cv::VideoCapture secondSequence)
{
	std::pair<cv::Mat, cv::Mat> frames;
	firstSequence >> frames.first;
	secondSequence >> frames.second;
	return frames;
}

void ImgProcUtility::readRealTimeFrames(realTimeCapturePair& stereoCapture, int width, int height)
{
	cv::Mat firstFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
	cv::Mat secondFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

	stereoCapture.getFirstCapture().getCamera()->getFrame(firstFrame.data);
	stereoCapture.getSecondCapture().getCamera()->getFrame(secondFrame.data);

	cv::Mat firstResizedMat(height, width, firstFrame.type());
	cv::Mat secondResizedMat(height, width, secondFrame.type());

	cv::resize(firstFrame, firstResizedMat, firstResizedMat.size(), cv::INTER_CUBIC);
	cv::resize(secondFrame, secondResizedMat, secondResizedMat.size(), cv::INTER_CUBIC);

	stereoCapture.getFirstCapture().setCurrentFrame(firstResizedMat);
	stereoCapture.getSecondCapture().setCurrentFrame(secondResizedMat);
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::resizeFrames(std::pair<cv::Mat, cv::Mat> frames, double scale)
{
	std::pair<cv::Mat, cv::Mat> resizedFrames;
	resize(frames.first, resizedFrames.first, cv::Size(), 0.5, 0.5);
	resize(frames.second, resizedFrames.second, cv::Size(), 0.5, 0.5);
	return resizedFrames;
}

StereoROISets ImgProcUtility::selectMarkers(std::pair<cv::Mat, cv::Mat> frames, std::pair< cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers)
{
	StereoROISets ROISets;
	selectROIs("firstMultiTracker", frames.first, ROISets.first);
	for (int i = 0; i < ROISets.first.size(); i++)
	{
		multitrackers.first->add(cv::TrackerCSRT::create(), frames.first, cv::Rect2d(ROISets.first[i]));
	}
	selectROIs("secondMultiTracker", frames.second, ROISets.second);
	for (int i = 0; i < ROISets.second.size(); i++)
	{
		multitrackers.second->add(cv::TrackerCSRT::create(), frames.second, cv::Rect2d(ROISets.second[i]));
	}
	return ROISets;
}

void ImgProcUtility::updateTrackers(std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames)
{
	multitrackers.first->update(frames.first);
	multitrackers.second->update(frames.second);
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::convertFramesToGray(std::pair<cv::Mat, cv::Mat> colorFrames)
{
	std::pair<cv::Mat, cv::Mat> grayFrames;
	cvtColor(colorFrames.first, grayFrames.first, cv::COLOR_BGR2GRAY);
	cvtColor(colorFrames.second, grayFrames.second, cv::COLOR_BGR2GRAY);
	return grayFrames;
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::cutROIsFromFrames(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Rect, cv::Rect> ROI)
{
	std::pair<cv::Mat, cv::Mat> croppedFrames;
	croppedFrames.first = grayFrames.first(ROI.first);          // cropping ROIs to see the analysed part of the image	
	croppedFrames.second = grayFrames.second(ROI.second);
	return croppedFrames;
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::thresholdImages(std::pair<cv::Mat, cv::Mat> frames, int thresh)
{
	std::pair<cv::Mat, cv::Mat> threshFrames;
	cv::threshold(frames.first, threshFrames.first, thresh, 255, cv::THRESH_BINARY);
	cv::threshold(frames.second, threshFrames.second, thresh, 255, cv::THRESH_BINARY);
	return threshFrames;
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::erodeImages(std::pair<cv::Mat, cv::Mat> frames, int erosionSize = 1, int erosionType = cv::MORPH_RECT)
{
	std::pair<cv::Mat, cv::Mat> erodedFrames;

	cv::Mat element = getStructuringElement(erosionType,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	erode(frames.first, erodedFrames.first, element);
	erode(frames.second, erodedFrames.second, element);

	return erodedFrames;
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::performCanny(std::pair<cv::Mat, cv::Mat> frames, int threshold)
{
	std::pair<cv::Mat, cv::Mat> cannyFrames;
	cv::Canny(frames.first, cannyFrames.first, threshold, 255);
	cv::Canny(frames.second, cannyFrames.second, threshold, 255);

	return cannyFrames;
}

void ImgProcUtility::drawCirclesAroundMarkers(std::pair<cv::Mat, cv::Mat> frames, StereoCoordinates2D circleCoordinates, std::vector<std::pair<int, int>> radiuses)
{
	for (int i = 0; i < expectedNumberOfMarkers; ++i)
	{
		cv::Point firstCenter(circleCoordinates.first[i](0), circleCoordinates.first[i](1));
		cv::Point secondCenter(circleCoordinates.second[i](0), circleCoordinates.second[i](1));
		circle(frames.first, firstCenter, radiuses[i].first, cv::Scalar(0, 0, 255));
		circle(frames.second, secondCenter, radiuses[i].second, cv::Scalar(0, 0, 255));
	}
}

void ImgProcUtility::drawRectAroundROI(std::pair<cv::Mat, cv::Mat> frames, std::pair<cv::Rect, cv::Rect> trackedAreas)
{
	rectangle(frames.first, trackedAreas.first, cv::Scalar(255, 0, 0), 2, 1);
	rectangle(frames.second, trackedAreas.second, cv::Scalar(255, 0, 0), 2, 1);
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> ImgProcUtility::getBiggestContours(std::pair<cv::Mat, cv::Mat> frames)
{
	std::vector<std::vector<cv::Point> > firstContours, secondContours;
	findContours(frames.first, firstContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	findContours(frames.second, secondContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	double maxContour1 = 0, maxContour2 = 0;
	int firstBiggestContour = 0, secondBiggestContour = 0;
	for (size_t i = 0; i < max(firstContours.size(), secondContours.size()); i++) {

		if (i < firstContours.size() && contourArea(firstContours[i]) > maxContour1)
		{
			maxContour1 = contourArea(firstContours[i]);
			firstBiggestContour = i;
		}
		if (i < secondContours.size() && contourArea(secondContours[i]) > maxContour2)
		{
			maxContour2 = contourArea(secondContours[i]);
			secondBiggestContour = i;
		}
	}
	return{ firstContours[firstBiggestContour], secondContours[secondBiggestContour] };
}

cv::Vec3f ImgProcUtility::getContoursCenterOfMass(std::vector<cv::Point> contour)
{
	auto M = moments(contour);
	float CX = M.m10 / M.m00;
	float CY = M.m01 / M.m00;
	float R = sqrt(contourArea(contour) / M_PI);

	return { CX, CY, R };
}


std::pair<cv::Vec3f, cv::Vec3f> ImgProcUtility::findCirclesInROIs(std::pair<cv::Mat, cv::Mat> frames)
{
	auto cannyFrames = performCanny(frames, 43);
	auto circleCenters = getBiggestContours(cannyFrames);
	auto firstCenter = getContoursCenterOfMass(circleCenters.first);
	auto secondCenter = getContoursCenterOfMass(circleCenters.second);

	return { firstCenter, secondCenter };
}

StereoCoordinates2D ImgProcUtility::getMarkersCoordinates2D(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Ptr<cv::MultiTracker>, cv::Ptr<cv::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames)
{
	std::vector<std::pair<int, int>> radiuses = std::vector<std::pair<int, int>>(expectedNumberOfMarkers);
	StereoCoordinates2D coordinates2D;
	int detectedMarkers = 0;

	for (unsigned int i = 0; i < expectedNumberOfMarkers; ++i)
	{
		std::pair<cv::Rect, cv::Rect> ROI(multitrackers.first->getObjects()[i], multitrackers.second->getObjects()[i]);
		std::pair<cv::Mat, cv::Mat> croppedFrames = ImgProcUtility::cutROIsFromFrames(grayFrames, ROI);
		std::pair<cv::Mat, cv::Mat> threshFrames = ImgProcUtility::thresholdImages(croppedFrames, 150);
		auto erodeFrames = erodeImages(threshFrames, 1);

		//stuff above has the biggest effect on fps
		std::pair<cv::Vec3f, cv::Vec3f> v3fCircles = findCirclesInROIs(erodeFrames);

		coordinates2D.first[i](0) = v3fCircles.first(0) + multitrackers.first->getObjects()[i].x;
		coordinates2D.first[i](1) = v3fCircles.first(1) + multitrackers.first->getObjects()[i].y;
		coordinates2D.second[i](0) = v3fCircles.second(0) + multitrackers.second->getObjects()[i].x;
		coordinates2D.second[i](1) = v3fCircles.second(1) + multitrackers.second->getObjects()[i].y;
		radiuses[i].first = v3fCircles.first(2);
		radiuses[i].second = v3fCircles.second(2);

		++detectedMarkers;

		ImgProcUtility::drawCirclesAroundMarkers(frames, coordinates2D, radiuses);
		ImgProcUtility::drawRectAroundROI(frames, std::pair<cv::Rect, cv::Rect>(multitrackers.first->getObjects()[i], multitrackers.second->getObjects()[i]));
	}
	return coordinates2D;
}

cv::Mat ImgProcUtility::process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices)
{
	std::pair<cv::Mat, cv::Mat> distCoords = ImgProcUtility::populateMatricesFromVectors(coordinates2D);

	std::vector<cv::Vec2d> undistCoords1, undistCoords2;
	undistortPoints(distCoords.first, undistCoords1, matrices.firstCamMatrix, matrices.firstCamCoeffs, matrices.R1, matrices.P1);
	undistortPoints(distCoords.second, undistCoords2, matrices.secondCamMatrix, matrices.secondCamCoeffs, matrices.R2, matrices.P2);

	cv::Mat triangCoords;
	triangulatePoints(matrices.P1, matrices.P2, undistCoords1, undistCoords2, triangCoords);     //triangulation (not that much effect on fps (probably no effect)
	return triangCoords;
}

void ImgProcUtility::getMarkersCoordinates3D(cv::Mat triangCoords, Coordinates* outBalls, int& outDetectedBallsCount)
{
	for (int i = 0; i < triangCoords.cols; ++i) // for different balls
	{
		outBalls[i].X = triangCoords.at<double>(0, i) / triangCoords.at<double>(3, i);
		outBalls[i].Y = triangCoords.at<double>(1, i) / triangCoords.at<double>(3, i);
		outBalls[i].Z = triangCoords.at<double>(2, i) / triangCoords.at<double>(3, i);

		++outDetectedBallsCount;
	}
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::populateMatricesFromVectors(StereoCoordinates2D coordinates2D)
{
	cv::Mat firstCamCoordinates = cv::Mat(cv::Size(2, coordinates2D.first.size()), CV_64FC1);
	cv::Mat secondCamCoordinates = cv::Mat(cv::Size(2, coordinates2D.second.size()), CV_64FC1);
	std::pair<cv::Mat, cv::Mat> coordinates2DMat(firstCamCoordinates, secondCamCoordinates);

	for (int i = 0; i < coordinates2D.first.size(); ++i)
	{
		coordinates2DMat.first.at<double>(i, 0) = coordinates2D.first[i](0);
		coordinates2DMat.first.at<double>(i, 1) = coordinates2D.first[i](1);

		coordinates2DMat.second.at<double>(i, 0) = coordinates2D.second[i](0);
		coordinates2DMat.second.at<double>(i, 1) = coordinates2D.second[i](1);
	}
	return coordinates2DMat;
}

double ImgProcUtility::calculateDistanceBetweenMarkers(Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{
	return sqrt(pow(outBalls[firstMarkerId].X - outBalls[secondMarkerId].X, 2) + pow(outBalls[firstMarkerId].Y - outBalls[secondMarkerId].Y, 2)
		+ pow(outBalls[firstMarkerId].Z - outBalls[secondMarkerId].Z, 2));
}

void ImgProcUtility::displayDistanceBetweenMarkers(cv::Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{
	cv::putText(displayMatrix, ("Distance between markers " + std::to_string(firstMarkerId) + " and " + std::to_string(secondMarkerId) + ": " + std::to_string(calculateDistanceBetweenMarkers(outBalls, firstMarkerId, secondMarkerId))),
		cv::Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
}