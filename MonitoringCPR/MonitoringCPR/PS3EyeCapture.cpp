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
void PS3EyeCapture::setROIs(std::vector<cv::Vec3f> detectedMarkers)
{
	if (detectedMarkers.size() == expectedNumberOfMarkers)
	{
		clearROIs();		
		for (int i = 0; i < detectedMarkers.size(); i++)
		{
			cv::Rect trackedArea(detectedMarkers[i](0) - 3 * detectedMarkers[i](2), detectedMarkers[i](1) - 3 * detectedMarkers[i](2), 6 * detectedMarkers[i](2), 6 * detectedMarkers[i](2));
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
	int detectedMarkers = 0;
	cv::Mat grayFrame;
	cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);

	markersCoordinates2D.clear();
	for (unsigned int i = 0; i < ROIs.size(); ++i)
	{
		if(!ImgProcUtility::isROIinFrame(ROIs[i], grayFrame))
		{
			return false;
		}
		//cv::Rect ROI(multiTracker->getObjects()[i]);
		auto croppedFrame = grayFrame(ROIs[i]);
		cv::Mat threshFrame;
		cv::threshold(croppedFrame, threshFrame, threshLevel, 255, cv::THRESH_BINARY);
		auto erodedFrame = ImgProcUtility::erodeImage(threshFrame, 1, cv::MORPH_RECT);
		cv::Vec3f v3fCircles;
		if (!ImgProcUtility::findCircleInROI(erodedFrame, v3fCircles, threshLevel))
		{
			return false;
		}		
		//markersCoordinates2D[i](0) = v3fCircles(0) + ROIs[i].x;
		//markersCoordinates2D[i](1) = v3fCircles(1) + ROIs[i].y;

		markersCoordinates2D.push_back({ v3fCircles(0) + ROIs[i].x , v3fCircles(1) + ROIs[i].y });
		++detectedMarkers;
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

void PS3EyeCapture::detectMarkers()
{
}

