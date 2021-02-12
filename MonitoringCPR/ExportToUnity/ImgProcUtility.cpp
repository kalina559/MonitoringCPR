#include"ImgProcUtility.h"

pair<Mat, Mat> ImgProcUtility::readFrames(VideoCapture firstSequence, VideoCapture secondSequence)
{
	pair<Mat, Mat> frames;
	firstSequence >> frames.first;
	secondSequence >> frames.second;
	return frames;
}

pair<Mat, Mat> ImgProcUtility::resizeFrames(pair<Mat, Mat> frames, double scale)
{
	pair<Mat, Mat> resizedFrames;
	resize(frames.first, resizedFrames.first, Size(), 0.5, 0.5);
	resize(frames.second, resizedFrames.second, Size(), 0.5, 0.5);
	return resizedFrames;
}

StereoROISets ImgProcUtility::selectMarkers(pair<Mat, Mat> frames, pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers)
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

void ImgProcUtility::updateTrackers(pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers, pair<Mat, Mat> frames)
{
	multitrackers.first->update(frames.first);
	multitrackers.second->update(frames.second);
}

pair<Mat, Mat> ImgProcUtility::convertFramesToGray(pair<Mat, Mat> colorFrames)
{
	pair<Mat, Mat> grayFrames;
	cvtColor(colorFrames.first, grayFrames.first, COLOR_BGR2GRAY);
	cvtColor(colorFrames.second, grayFrames.second, COLOR_BGR2GRAY);
	return grayFrames;
}

pair<Mat, Mat> ImgProcUtility::cutROIsFromFrames(pair<Mat, Mat> grayFrames, pair<Rect, Rect> ROI)
{
	pair<Mat, Mat> croppedFrames;
	croppedFrames.first = grayFrames.first(ROI.first);          // cropping ROIs to see the analysed part of the image	
	croppedFrames.second = grayFrames.second(ROI.second);
	return croppedFrames;
}

pair<Mat, Mat> ImgProcUtility::thresholdImages(pair<Mat, Mat> frames, int thresh)
{
	pair<Mat, Mat> threshFrames;
	threshold(frames.first, threshFrames.first, thresh, 255, THRESH_BINARY);
	threshold(frames.second, threshFrames.second, thresh, 255, THRESH_BINARY);
	return threshFrames;
}

pair<Mat, Mat> ImgProcUtility::erodeImages(pair<Mat, Mat> frames, int erosionSize = 1, int erosionType = MORPH_RECT)
{
	pair<Mat, Mat> erodedFrames;

	Mat element = getStructuringElement(erosionType,
		Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		Point(erosionSize, erosionSize));

	erode(frames.first, erodedFrames.first, element);
	erode(frames.second, erodedFrames.second, element);

	return erodedFrames;
}

pair<Mat, Mat> ImgProcUtility::performCanny(pair<Mat, Mat> frames, int threshold)
{
	pair<Mat, Mat> cannyFrames;
	cv::Canny(frames.first, cannyFrames.first, threshold, 255);
	cv::Canny(frames.second, cannyFrames.second, threshold, 255);

	return cannyFrames;
}

void ImgProcUtility::drawCirclesAroundMarkers(pair<Mat, Mat> frames, StereoCoordinates2D circleCoordinates, vector<pair<int, int>> radiuses)
{
	for (int i = 0; i < expectedNumberOfMarkers; ++i)
	{
		Point firstCenter(circleCoordinates.first[i](0), circleCoordinates.first[i](1));
		Point secondCenter(circleCoordinates.second[i](0), circleCoordinates.second[i](1));
		circle(frames.first, firstCenter, radiuses[i].first, cv::Scalar(0, 0, 255));
		circle(frames.second, secondCenter, radiuses[i].second, cv::Scalar(0, 0, 255));
	}
}

void ImgProcUtility::drawRectAroundROI(pair<Mat, Mat> frames, pair<Rect, Rect> trackedAreas)
{
	rectangle(frames.first, trackedAreas.first, cv::Scalar(255, 0, 0), 2, 1);
	rectangle(frames.second, trackedAreas.second, cv::Scalar(255, 0, 0), 2, 1);
}

pair<vector<Point>, vector<Point>> ImgProcUtility::getBiggestContours(pair<Mat, Mat> frames)
{
	vector<vector<Point> > firstContours, secondContours;
	findContours(frames.first, firstContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	findContours(frames.second, secondContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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

Vec3f ImgProcUtility::getContoursCenterOfMass(vector<Point> contour)
{
	auto M = moments(contour);
	float CX = M.m10 / M.m00;
	float CY = M.m01 / M.m00;
	float R = sqrt(contourArea(contour) / M_PI);

	return { CX, CY, R };
}


pair<Vec3f, Vec3f> ImgProcUtility::findCirclesInROIs(pair<Mat, Mat> frames)
{
	auto cannyFrames = performCanny(frames, 43);
	auto circleCenters = getBiggestContours(cannyFrames);
	auto firstCenter = getContoursCenterOfMass(circleCenters.first);
	auto secondCenter = getContoursCenterOfMass(circleCenters.second);

	return { firstCenter, secondCenter };
}

StereoCoordinates2D ImgProcUtility::getMarkersCoordinates2D(pair<Mat, Mat> grayFrames, pair<Ptr<MultiTracker>, Ptr<MultiTracker>> multitrackers, pair<Mat, Mat> frames)
{
	vector<pair<int, int>> radiuses = vector<pair<int, int>>(expectedNumberOfMarkers);
	StereoCoordinates2D coordinates2D;
	int detectedMarkers = 0;

	for (unsigned int i = 0; i < expectedNumberOfMarkers; ++i)
	{
		pair<Rect, Rect> ROI(multitrackers.first->getObjects()[i], multitrackers.second->getObjects()[i]);
		pair<Mat, Mat> croppedFrames = ImgProcUtility::cutROIsFromFrames(grayFrames, ROI);
		pair<Mat, Mat> threshFrames = ImgProcUtility::thresholdImages(croppedFrames, 150);
		auto erodeFrames = erodeImages(threshFrames, 1);

		//stuff above has the biggest effect on fps
		pair<Vec3f, Vec3f> v3fCircles = findCirclesInROIs(erodeFrames);

		coordinates2D.first[i](0) = v3fCircles.first(0) + multitrackers.first->getObjects()[i].x;
		coordinates2D.first[i](1) = v3fCircles.first(1) + multitrackers.first->getObjects()[i].y;
		coordinates2D.second[i](0) = v3fCircles.second(0) + multitrackers.second->getObjects()[i].x;
		coordinates2D.second[i](1) = v3fCircles.second(1) + multitrackers.second->getObjects()[i].y;
		radiuses[i].first = v3fCircles.first(2);
		radiuses[i].second = v3fCircles.second(2);

		++detectedMarkers;

		ImgProcUtility::drawCirclesAroundMarkers(frames, coordinates2D, radiuses);
		ImgProcUtility::drawRectAroundROI(frames, pair<Rect, Rect>(multitrackers.first->getObjects()[i], multitrackers.second->getObjects()[i]));
	}
	return coordinates2D;
}

Mat ImgProcUtility::process2DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices)
{
	pair<Mat, Mat> distCoords = ImgProcUtility::populateMatricesFromVectors(coordinates2D);

	vector<Vec2d> undistCoords1, undistCoords2;
	undistortPoints(distCoords.first, undistCoords1, matrices.firstCamMatrix, matrices.firstCamCoeffs, matrices.R1, matrices.P1);         //undistorting the coordinates
	undistortPoints(distCoords.second, undistCoords2, matrices.secondCamMatrix, matrices.secondCamCoeffs, matrices.R2, matrices.P2);

	Mat triangCoords;
	triangulatePoints(matrices.P1, matrices.P2, undistCoords1, undistCoords2, triangCoords);     //triangulation (not that much effect on fps (probably no effect)
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

pair<Mat, Mat> ImgProcUtility::populateMatricesFromVectors(StereoCoordinates2D coordinates2D)
{
	Mat firstCamCoordinates = Mat(Size(2, coordinates2D.first.size()), CV_64FC1);      		//declaring distorded coordinates
	Mat secondCamCoordinates = Mat(Size(2, coordinates2D.second.size()), CV_64FC1);
	pair<Mat, Mat> coordinates2DMat(firstCamCoordinates, secondCamCoordinates);

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

void ImgProcUtility::displayDistanceBetweenMarkers(Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{
	cv::putText(displayMatrix, ("Distance between markers " + to_string(firstMarkerId) + " and " + to_string(secondMarkerId) + ": " + to_string(calculateDistanceBetweenMarkers(outBalls, firstMarkerId, secondMarkerId))),
		Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
}