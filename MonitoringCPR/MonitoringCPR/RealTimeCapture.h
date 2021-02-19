#pragma once
#include <opencv2/tracker.hpp>
#include <iostream>
#include <opencv2/tracking.hpp>
#include "ps3eye.h"
class RealTimeCapture
{
	ps3eye::PS3EYECam::PS3EYERef camera = NULL;
	cv::Ptr<cv::MultiTracker> multiTracker;
	cv::Mat currentFrame;
	// bool isReady czy mozna przejsc do trackowania
	// ROIs
	//number of detected balls

public:
	~RealTimeCapture()
	{
		camera->stop();
		camera = NULL;
		cv::imshow("~realtimecapture", cv::imread("C/Users/Kalin/Desktop/pierdoly/memes/outstanding.png"));
		cv::waitKey(0);
	}
	ps3eye::PS3EYECam::PS3EYERef getCamera()
	{
		return camera;
	}
	void setCamera(ps3eye::PS3EYECam::PS3EYERef cam)
	{
		camera = cam;
	}
	void setMultiTracker(cv::Ptr<cv::MultiTracker> tracker)
	{
		multiTracker = tracker;
	}
	cv::Ptr<cv::MultiTracker> getMultiTracker()
	{
		return multiTracker;
	}
	void setCurrentFrame(cv::Mat frame)
	{
		currentFrame = frame;
	}
	cv::Mat getCurrentFrame()
	{
		return currentFrame;
	}
};

class realTimeCapturePair
{
	bool isReady = false;
	RealTimeCapture first;
	RealTimeCapture second;
	realTimeCapturePair(realTimeCapturePair const& copy);
	void operator=(realTimeCapturePair const& copy);
public:
	static realTimeCapturePair* getInstance()
	{
		static realTimeCapturePair instance;
		return &instance;
	}
	realTimeCapturePair()
	{
	}
	~realTimeCapturePair()
	{
	}
	RealTimeCapture& getFirstCapture()
	{
		return first;
	}
	RealTimeCapture& getSecondCapture()
	{
		return second;
	}
	void freeCameras()
	{
		getInstance()->getFirstCapture().getCamera()->stop();
		getInstance()->getFirstCapture().setCamera(NULL);
		getInstance()->getSecondCapture().getCamera()->stop();
		getInstance()->getSecondCapture().setCamera(NULL);
	}
	void setIsReady(bool value)
	{
		isReady = value;
	}
	bool getIsReady()
	{
		return isReady;
	}
};
