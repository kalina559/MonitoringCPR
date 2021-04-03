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
std::pair<cv::Mat, cv::Mat> ImgProcUtility::thresholdImages(std::pair<cv::Mat, cv::Mat> frames, int thresh)
{
	std::pair<cv::Mat, cv::Mat> threshFrames;
	cv::threshold(frames.first, threshFrames.first, thresh, 255, cv::THRESH_BINARY);
	cv::threshold(frames.second, threshFrames.second, thresh, 255, cv::THRESH_BINARY);
	return threshFrames;
}

std::vector<std::string> ImgProcUtility::getFileNames(std::string path)
{
	std::vector<std::string> fileNamesVec;
	cv::glob(path, fileNamesVec, false);
	return fileNamesVec;
}

std::string ImgProcUtility::getCurrentDateStr()
{
	char buffer[30];
	char dateStr[30];
	_strdate_s(dateStr);
	sprintf_s(buffer, dateStr);
	return std::string(buffer);
}

std::string ImgProcUtility::getCurrentTimeStr()
{
	char buffer[30];
	char timeStr[30];
	_strtime_s(timeStr);
	sprintf_s(buffer, timeStr);
	return std::string(buffer);
}

void ImgProcUtility::drawDetectedCirclesGrid(cv::Mat& frame, cv::Ptr<cv::FeatureDetector> blobDetector)
{
	std::vector<cv::Point2f> centers;
	cv::Mat gray;
	cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

	bool patternFound1 = findCirclesGrid(gray, arrayOfCirclesSize, centers, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
	drawChessboardCorners(frame, arrayOfCirclesSize, cv::Mat(centers), 1);
}

cv::Ptr<cv::FeatureDetector> ImgProcUtility::initBlobDetector()
{
	cv::SimpleBlobDetector::Params params;
	params.minThreshold = 1;
	params.filterByConvexity = 1;
	params.minConvexity = 0.5;
	return cv::SimpleBlobDetector::create(params);
}

void ImgProcUtility::passFrameToUnity(cv::Mat frame, unsigned char* frameData)
{
	cv::Mat argbImg;
	cv::cvtColor(frame, argbImg, cv::COLOR_BGR2RGBA);
	std::memcpy(frameData, argbImg.data, argbImg.total() * argbImg.elemSize());
}

std::pair<cv::Mat, cv::Mat> ImgProcUtility::performCanny(std::pair<cv::Mat, cv::Mat> frames, int threshold)
{
	std::pair<cv::Mat, cv::Mat> cannyFrames;
	cv::Canny(frames.first, cannyFrames.first, threshold, 255);
	cv::Canny(frames.second, cannyFrames.second, threshold, 255);

	return cannyFrames;
}

bool ImgProcUtility::getBiggestContour(cv::Mat frame, std::vector<cv::Point>& biggestContour)  //dodac opisywanie okregu na konturze
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

cv::Vec3f ImgProcUtility::getContoursMinEnclosingCircle(std::vector<cv::Point> contour)
{
	cv::Point2f center;
	float radius;
	cv::minEnclosingCircle(contour, center, radius);
	return { center.x, center.y, radius };
}

bool ImgProcUtility::findCircleInROI(cv::Mat frame, cv::Vec3f& circleCoordinates, int threshLevel)
{
	cv::Mat cannyFrame;
	std::vector<cv::Point> circleContour;

	cv::Canny(frame, cannyFrame, threshLevel, (threshLevel + 255) / 2);
	if (!getBiggestContour(cannyFrame, circleContour))
		return false;

	circleCoordinates = getContoursMinEnclosingCircle(circleContour);

	return true;
}

cv::Mat ImgProcUtility::getMarkers3DCoordinates(StereoCoordinates2D coordinates2D, Matrices& matrices)
{
	std::pair<cv::Mat, cv::Mat> distCoords = ImgProcUtility::populateMatricesFromVectors(coordinates2D);

	std::vector<cv::Vec2d> undistCoords1, undistCoords2;
	undistortPoints(distCoords.first, undistCoords1, matrices.firstCamMatrix, matrices.firstCamCoeffs, matrices.R1, matrices.P1);
	undistortPoints(distCoords.second, undistCoords2, matrices.secondCamMatrix, matrices.secondCamCoeffs, matrices.R2, matrices.P2);

	cv::Mat coordinates3D;
	triangulatePoints(matrices.P1, matrices.P2, undistCoords1, undistCoords2, coordinates3D);
	return coordinates3D;
}

void ImgProcUtility::homogenousToCartesianCoordinates(cv::Mat triangCoords, Coordinates* outBalls)
{
	for (int i = 0; i < triangCoords.cols; ++i)
	{
		outBalls[i].X = triangCoords.at<double>(0, i) / triangCoords.at<double>(3, i);
		outBalls[i].Y = triangCoords.at<double>(1, i) / triangCoords.at<double>(3, i);
		outBalls[i].Z = triangCoords.at<double>(2, i) / triangCoords.at<double>(3, i);
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

BSTR ImgProcUtility::getFrameSetId(std::string path)
{
	BSTR bs;
	std::vector<cv::String> fileNames = getFileNames(path);

	std::wstring outDigitString;
	if (fileNames.size() != 0)
	{
		for (int i = 0; i < fileNames.size(); ++i)
		{
			auto fileName = std::filesystem::path(fileNames[i]).filename();
			std::wstring fileNameStr{ fileName.wstring() };

			outDigitString += fileNameStr;
		}
		bs = SysAllocStringLen(outDigitString.data(), outDigitString.size());
		return SysAllocString(bs);
	}
	bs = SysAllocString(L"");
	return SysAllocString(bs);
}

//void ImgProcUtility::detectMarkers(cv::Mat& frame, cv::Mat& displayFrame, std::vector<cv::Vec3f>& circles)
//{
//	cv::Mat cannyFrame;
//	cv::Canny(frame, cannyFrame, 200, 255);
//
//	cv::HoughCircles(cannyFrame, circles, cv::HOUGH_GRADIENT, 1, frame.rows / 30, 255, 10, 1, 30);
//
//	std::sort(circles.begin(), circles.end(), contourSorter());
//
//	for (int i = 0; i < circles.size(); ++i)
//	{
//		cv::putText(displayFrame, std::to_string(i),
//			cv::Point(circles[i](0) - 10, circles[i](1)), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
//		circle(displayFrame, cv::Point(circles[i](0), circles[i](1)), circles[i](2), cv::Scalar(0, 0, 255));
//	}
//}
bool ImgProcUtility::isROIinFrame(cv::Rect ROI, cv::Mat frame)
{
	bool isInsideFrame = (ROI.x > 0 && ROI.x < (frame.cols - ROI.width) && ROI.y > 0 && ROI.y < (frame.rows - ROI.height));
	return isInsideFrame;
}