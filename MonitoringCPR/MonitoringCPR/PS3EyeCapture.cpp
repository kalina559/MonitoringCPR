#include "PS3EyeCapture.h"
ps3eye::PS3EYECam::PS3EYERef PS3EyeCapture::getCamera()
{
	return camera;
}
void PS3EyeCapture::setCamera(ps3eye::PS3EYECam::PS3EYERef cam)
{
	camera = cam;
}
void PS3EyeCapture::setMultiTracker(cv::Ptr<cv::legacy::MultiTracker> tracker)
{
	multiTracker = tracker;
}
cv::Ptr<cv::legacy::MultiTracker> PS3EyeCapture::getMultiTracker()
{
	return multiTracker;
}
void PS3EyeCapture::setCurrentFrame(cv::Mat frame)
{
	currentFrame = frame;
}
cv::Mat PS3EyeCapture::getCurrentFrame()
{
	return currentFrame;
}
void PS3EyeCapture::setROIs(std::vector<cv::Vec3f> detectedCircles)
{
	if (detectedCircles.size() == expectedNumberOfMarkers)
	{
		clearROIs();
		for (int i = 0; i < detectedCircles.size(); i++)
		{
			cv::Rect trackedArea(detectedCircles[i](0) - 3 * detectedCircles[i](2), detectedCircles[i](1) - 3 * detectedCircles[i](2), 6 * detectedCircles[i](2), 6 * detectedCircles[i](2));
			ROIs.push_back(trackedArea);
		}

	}
}
void PS3EyeCapture::startMultiTracker()
{
	multiTracker = cv::legacy::MultiTracker::create();
	for (int i = 0; i < ROIs.size(); i++)
	{
		multiTracker->add(cv::legacy::TrackerMOSSE::create(), currentFrame, ROIs[i]);
	}
}
void PS3EyeCapture::stopMultiTracker()
{
	ROIs.clear();
	multiTracker.release();
}

void PS3EyeCapture::clearROIs()
{
	ROIs.clear();
}

std::vector<cv::Rect> PS3EyeCapture::getROIs()
{
	return ROIs;
}
void PS3EyeCapture::updateTracker()
{
	multiTracker->update(currentFrame);
	for (int i = 0; i < ROIs.size(); ++i)
	{
		ROIs[i] = multiTracker->getObjects()[i];
	}
}

bool PS3EyeCapture::calculateMarkersCoordinates()
{
	cv::Mat grayFrame;
	cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);
	if (markersCoordinates2D.size() > 0)
	{
		markersCoordinates2D.clear();
	}
	for (unsigned int i = 0; i < ROIs.size(); ++i)
	{
		if (!ImgProcUtility::isROIinFrame(ROIs[i], grayFrame))
		{
			return false;
		}
		auto croppedFrame = grayFrame(ROIs[i]);
		cv::Vec3f v3fCircles;
		if (!ImgProcUtility::findCircleInROI(croppedFrame, v3fCircles, threshLevel))
		{
			return false;
		}
		markersCoordinates2D.push_back({ v3fCircles(0) + ROIs[i].x , v3fCircles(1) + ROIs[i].y });
		circle(currentFrame, cv::Point(v3fCircles[0] + ROIs[i].x, v3fCircles[1] + ROIs[i].y), v3fCircles[2], cv::Scalar(0, 0, 255));
		rectangle(currentFrame, ROIs[i], cv::Scalar(255, 0, 0), 2, 1);
	}
	return true;
}
std::vector<cv::Vec2f> PS3EyeCapture::getMarkersCoordinates()
{
	return markersCoordinates2D;
}

void PS3EyeCapture::setThreshLevel(int level)
{
	threshLevel = level;
}
int PS3EyeCapture::getThreshLevel()
{
	return threshLevel;
}

void PS3EyeCapture::setExpectedNumberOfMarkers(int number)
{
	expectedNumberOfMarkers = number;
}

bool PS3EyeCapture::detectMarkers(cv::Mat& displayFrame)
{
	cv::Mat gray, threshFrame, cannyFrame;
	currentFrame.copyTo(displayFrame);
	cv::cvtColor(displayFrame, gray, cv::COLOR_BGR2GRAY);
	cv::threshold(gray, threshFrame, threshLevel, 255, cv::THRESH_BINARY);
	cv::Canny(threshFrame, cannyFrame, 200, 255);

	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(cannyFrame, circles, cv::HOUGH_GRADIENT, 1, displayFrame.rows / 30, 255, 10, 1, 30);

	std::sort(circles.begin(), circles.end(), ImgProcUtility::contourSorter());
	setROIs(circles);
	for (int i = 0; i < circles.size(); ++i)
	{
		cv::putText(displayFrame, std::to_string(i),
			cv::Point(circles[i](0) - 10, circles[i](1)), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
		circle(displayFrame, cv::Point(circles[i](0), circles[i](1)), circles[i](2), cv::Scalar(0, 0, 255));
	}
	return checkIfAllMarkersDetected(circles);
}
bool PS3EyeCapture::checkIfAllMarkersDetected(std::vector<cv::Vec3f> circles)
{
	return (circles.size() == expectedNumberOfMarkers);
}
