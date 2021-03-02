#include"ImgProcUtility.h"
std::string ImgProcUtility::readFile(std::string name)
{
	std::string content;
	std::ifstream inputFile;
	inputFile.open(name);
	inputFile >> content;	
	inputFile.close();
	return content;	
}
std::pair<cv::Mat, cv::Mat> ImgProcUtility::readFrames(cv::VideoCapture firstSequence, cv::VideoCapture secondSequence)
{
	std::pair<cv::Mat, cv::Mat> frames;
	firstSequence >> frames.first;
	secondSequence >> frames.second;
	return frames;
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::resizeFrames(std::pair<cv::Mat, cv::Mat> frames, double scale)
{
	std::pair<cv::Mat, cv::Mat> resizedFrames;
	resize(frames.first, resizedFrames.first, cv::Size(), 0.5, 0.5);
	resize(frames.second, resizedFrames.second, cv::Size(), 0.5, 0.5);
	return resizedFrames;
}

StereoROISets ImgProcUtility::selectMarkers(std::pair<cv::Mat, cv::Mat> frames, std::pair< cv::Ptr<cv::legacy::MultiTracker>, cv::Ptr<cv::legacy::MultiTracker>> multitrackers)
{
	StereoROISets ROISets;
	selectROIs("firstMultiTracker", frames.first, ROISets.first);
	for (int i = 0; i < ROISets.first.size(); i++)
	{
		multitrackers.first->add(cv::legacy::TrackerMOSSE::create(), frames.first, cv::Rect2d(ROISets.first[i]));
	}
	selectROIs("secondMultiTracker", frames.second, ROISets.second);
	for (int i = 0; i < ROISets.second.size(); i++)
	{
		multitrackers.second->add(cv::legacy::TrackerMOSSE::create(), frames.second, cv::Rect2d(ROISets.second[i]));
	}
	return ROISets;
}

void ImgProcUtility::updateTrackers(std::pair<cv::Ptr<cv::legacy::MultiTracker>, cv::Ptr<cv::legacy::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames)
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

cv::Mat ImgProcUtility::erodeImage(cv::Mat frame, int erosionSize = 1, int erosionType = cv::MORPH_RECT)
{
	cv::Mat erodedFrame;

	cv::Mat element = getStructuringElement(erosionType,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	erode(frame, erodedFrame, element);
	return erodedFrame;
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
	for (int i = 0; i < circleCoordinates.first.size() ; ++i)
	{
		cv::Point firstCenter(circleCoordinates.first[i](0), circleCoordinates.first[i](1));
		cv::Point secondCenter(circleCoordinates.second[i](0), circleCoordinates.second[i](1));
		circle(frames.first, firstCenter, radiuses[i].first, cv::Scalar(0, 0, 255));
		circle(frames.second, secondCenter, radiuses[i].second, cv::Scalar(0, 0, 255));
	}
}

void ImgProcUtility::drawCirclesAroundMarkers(cv::Mat frame, std::vector<cv::Vec2f> circleCoordinates, std::vector<int> radiuses)
{
	for (int i = 0; i < circleCoordinates.size(); ++i)
	{
		cv::Point firstCenter(circleCoordinates[i](0), circleCoordinates[i](1));
		circle(frame, firstCenter, radiuses[i], cv::Scalar(0, 0, 255));
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

bool ImgProcUtility::getBiggestContours(cv::Mat frame, std::vector<cv::Point>& biggestContour)
{
	std::vector<std::vector<cv::Point> > contours;
	findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if (contours.size() == 0)
	{
		return false;
	}
	double maxContour = 0;
	int biggestContourId = 0;
	for (size_t i = 0; i < contours.size(); i++) {

		if (contourArea(contours[i]) > maxContour)
		{
			maxContour = contourArea(contours[i]);
			biggestContourId = i;
		}
	}
	biggestContour = contours[biggestContourId];	
	return true;
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

bool ImgProcUtility::findCircleInROI(cv::Mat frame, cv::Vec3f& ROI, int threshLevel)
{
	cv::Mat cannyFrame;
	std::vector<cv::Point> circleContour;
	cv::Canny(frame, cannyFrame, threshLevel, 255);
	if (!getBiggestContours(cannyFrame, circleContour))
		return false;
		
	ROI = getContoursCenterOfMass(circleContour);
	return true;

}
cv::Vec3f ImgProcUtility::findCircleInROI(cv::Mat frame)
{
	cv::Mat cannyFrame;
	std::vector<cv::Point> circleContour;
	cv::Canny(frame, cannyFrame, 70, 255);
	getBiggestContours(cannyFrame, circleContour);
	return getContoursCenterOfMass(circleContour);
}

StereoCoordinates2D ImgProcUtility::getMarkersCoordinates2D(std::pair<cv::Mat, cv::Mat> grayFrames, std::pair<cv::Ptr<cv::legacy::MultiTracker>, cv::Ptr<cv::legacy::MultiTracker>> multitrackers, std::pair<cv::Mat, cv::Mat> frames)
{
	std::vector<std::pair<int, int>> radiuses;// = std::vector<std::pair<int, int>>(expectedNumberOfMarkers);
	StereoCoordinates2D coordinates2D;
	int detectedMarkers = 0;

	for (unsigned int i = 0; i < multitrackers.first->getObjects().size(); ++i)
	{
		std::pair<cv::Rect, cv::Rect> ROI(multitrackers.first->getObjects()[i], multitrackers.second->getObjects()[i]);
		std::pair<cv::Mat, cv::Mat> croppedFrames = ImgProcUtility::cutROIsFromFrames(grayFrames, ROI);
		std::pair<cv::Mat, cv::Mat> threshFrames = ImgProcUtility::thresholdImages(croppedFrames, 150);
		auto erodeFrames = erodeImages(threshFrames, 1);

		//stuff above has the biggest effect on fps
		std::pair<cv::Vec3f, cv::Vec3f> v3fCircles = findCirclesInROIs(erodeFrames);

		//coordinates2D.first[i](0) = v3fCircles.first(0) + multitrackers.first->getObjects()[i].x;
		//coordinates2D.first[i](1) = v3fCircles.first(1) + multitrackers.first->getObjects()[i].y;
		//coordinates2D.second[i](0) = v3fCircles.second(0) + multitrackers.second->getObjects()[i].x;
		//coordinates2D.second[i](1) = v3fCircles.second(1) + multitrackers.second->getObjects()[i].y;
		//radiuses[i].first = v3fCircles.first(2);
		//radiuses[i].second = v3fCircles.second(2);

		cv::Vec2f firstCoordinates, secondCoordinates;
		firstCoordinates(0) = v3fCircles.first(0) + multitrackers.first->getObjects()[i].x;
		firstCoordinates(1) = v3fCircles.first(1) + multitrackers.first->getObjects()[i].y;
		secondCoordinates(0) = v3fCircles.second(0) + multitrackers.second->getObjects()[i].x;
		secondCoordinates(1) = v3fCircles.second(1) + multitrackers.second->getObjects()[i].y;

		coordinates2D.first.push_back(firstCoordinates);
		coordinates2D.second.push_back(secondCoordinates);

		radiuses.push_back({ v3fCircles.first(2), v3fCircles.second(2) });

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

void ImgProcUtility::getMarkersCoordinates3D(cv::Mat triangCoords, std::vector<Coordinates>& outBalls, int& outDetectedBallsCount)
{
	outBalls.clear();
	for (int i = 0; i < triangCoords.cols; ++i) // for different balls
	{
		/*outBalls[i].X = triangCoords.at<double>(0, i) / triangCoords.at<double>(3, i);
		outBalls[i].Y = triangCoords.at<double>(1, i) / triangCoords.at<double>(3, i);
		outBalls[i].Z = triangCoords.at<double>(2, i) / triangCoords.at<double>(3, i);*/

		Coordinates coords;
		coords.X = triangCoords.at<double>(0, i) / triangCoords.at<double>(3, i);
		coords.Y = triangCoords.at<double>(1, i) / triangCoords.at<double>(3, i);
		coords.Z = triangCoords.at<double>(2, i) / triangCoords.at<double>(3, i);

		outBalls.push_back(coords);
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

double ImgProcUtility::calculateDistanceBetweenMarkers(std::vector<Coordinates> outBalls, int firstMarkerId, int secondMarkerId)
{
	return sqrt(pow(outBalls[firstMarkerId].X - outBalls[secondMarkerId].X, 2) + pow(outBalls[firstMarkerId].Y - outBalls[secondMarkerId].Y, 2)
		+ pow(outBalls[firstMarkerId].Z - outBalls[secondMarkerId].Z, 2));
}

void ImgProcUtility::displayDistanceBetweenMarkers(cv::Mat& displayMatrix, Coordinates* outBalls, int firstMarkerId, int secondMarkerId)
{
	cv::putText(displayMatrix, ("Distance between markers " + std::to_string(firstMarkerId) + " and " + std::to_string(secondMarkerId) + ": " + std::to_string(calculateDistanceBetweenMarkers(outBalls, firstMarkerId, secondMarkerId))),
		cv::Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
}
void ImgProcUtility::displayDistanceBetweenMarkers(cv::Mat& displayMatrix, std::vector<Coordinates> outBalls, int firstMarkerId, int secondMarkerId)
{
	cv::putText(displayMatrix, ("Distance between markers " + std::to_string(firstMarkerId) + " and " + std::to_string(secondMarkerId) + ": " + std::to_string(calculateDistanceBetweenMarkers(outBalls, firstMarkerId, secondMarkerId))),
		cv::Point(200, 200), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
}
std::string ImgProcUtility::getId()
{
	std::string firstPath = "..\\MonitoringCPR\\images\\Calibration\\UnityFirstCam\\*.jpg";
	std::string secondPath = "..\\MonitoringCPR\\images\\Calibration\\UnitySecondCam\\*.jpg";
	std::vector<cv::String> fileNames1, fileNames2;

	cv::glob(firstPath, fileNames1, false); //todo wrzucic to w imgprocutility 
	cv::glob(secondPath, fileNames2, false);

	std::string outDigitString;
	if (fileNames1.size() == fileNames2.size() && fileNames1.size() != 0)
	{
		for (int i = 0; i < fileNames1.size(); ++i)
		{
			auto firstFileName = std::filesystem::path(fileNames1[i]).filename();
			auto secondFileName = std::filesystem::path(fileNames2[i]).filename();
			std::string firstFileNameStr{ firstFileName.string() };
			std::string secondFileNameStr{ secondFileName.string() };

			outDigitString += firstFileNameStr += secondFileNameStr;
		}
	}
	return outDigitString;
}

struct contour_sorter // 'less' for contours
{
	bool operator ()(cv::Vec3f a, cv::Vec3f b)
	{
		// scale factor for y should be larger than img.width
		return ((a[0] + 100 * a[1]) < (b[0] + 100 * b[1]));
	}
};

void ImgProcUtility::detectMarkers(cv::Mat& frame, std::vector<cv::Vec3f>& circles)
{
	cv::Mat gray;
	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
	HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, gray.rows / 30, 255, 10, 1, 30);


	std::sort(circles.begin(), circles.end(), contour_sorter());


	for (int i = 0; i < circles.size(); ++i)
	{
		cv::putText(frame, std::to_string(i),
			cv::Point(circles[i](0) - 10, circles[i](1)), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
		circle(frame, cv::Point(circles[i](0), circles[i](1)), circles[i](2), cv::Scalar(0, 0, 255));
	}
}
bool ImgProcUtility::isROIinFrame(cv::Rect ROI, cv::Mat frame)
{
	bool isInsideFrame = (ROI.x > 0 && ROI.x < (frame.cols + ROI.width) && ROI.y > 0 && ROI.y < (frame.rows + ROI.height));
	return isInsideFrame;
}



//funkcja tworzaca ROIs na podtawie wektora vec3f z wspolrzednymi markerow