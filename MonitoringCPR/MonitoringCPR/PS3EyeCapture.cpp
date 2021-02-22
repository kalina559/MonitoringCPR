#include "PS3EyeCapture.h"
ps3eye::PS3EYECam::PS3EYERef PS3EyeCapture::getCamera()
{
	return camera;
}
void PS3EyeCapture::setCamera(ps3eye::PS3EYECam::PS3EYERef cam)
{
	camera = cam;
}
void PS3EyeCapture::setMultiTracker(cv::Ptr<cv::MultiTracker> tracker)
{
	multiTracker = tracker;
}
cv::Ptr<cv::MultiTracker> PS3EyeCapture::getMultiTracker()
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
void PS3EyeCapture::setROIs(std::vector<cv::Vec3f> detectedMarkers)
{
	if (detectedMarkers.size() == 2)
	{
		clearROIs();
		for (int i = 0; i < detectedMarkers.size(); i++)
		{
			cv::Rect trackedArea(detectedMarkers[i](0) - 2 * detectedMarkers[i](2), detectedMarkers[i](1) - 2 * detectedMarkers[i](2), 4 * detectedMarkers[i](2), 4 * detectedMarkers[i](2));
			ROIs.push_back(trackedArea);
		}
	}
}
void PS3EyeCapture::startMultiTracker()
{
	multiTracker = cv::MultiTracker::create();
	for (int i = 0; i < ROIs.size(); i++)
	{
		multiTracker->add(cv::TrackerCSRT::create(), currentFrame, ROIs[i]);
	}
}
void PS3EyeCapture::stopMultiTracker()
{
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
}
bool PS3EyeCapture::calculateMarkersCoordinates()
{
	int detectedMarkers = 0;
	cv::Mat grayFrame;
	cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);
	std::vector<int> radiuses(expectedNumberOfMarkers);

	for (unsigned int i = 0; i < ROIs.size(); ++i)
	{
		cv::Rect ROI(multiTracker->getObjects()[i]);
		auto croppedFrame = grayFrame(ROI);
		cv::Mat threshFrame;
		cv::threshold(croppedFrame, threshFrame, threshLevel, 255, cv::THRESH_BINARY);
		auto erodedFrame = ImgProcUtility::erodeImage(threshFrame, 1, cv::MORPH_RECT);
		cv::Vec3f v3fCircles(1);
		if (!ImgProcUtility::findCircleInROI(erodedFrame, v3fCircles, threshLevel))
		{
			return false;
		}
		markersCoordinates2D[i](0) = v3fCircles(0) + multiTracker->getObjects()[i].x;
		markersCoordinates2D[i](1) = v3fCircles(1) + multiTracker->getObjects()[i].y;
		radiuses[i] = v3fCircles(2);
		++detectedMarkers;
		rectangle(currentFrame, multiTracker->getObjects()[i], cv::Scalar(255, 0, 0), 2, 1);
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
