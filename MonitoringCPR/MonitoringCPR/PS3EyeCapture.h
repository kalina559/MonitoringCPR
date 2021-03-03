#pragma once
#include "ImgProcUtility.h"

class PS3EyeCapture
{
	ps3eye::PS3EYECam::PS3EYERef camera = NULL;
	cv::Ptr<cv::legacy::MultiTracker> multiTracker;
	cv::Mat currentFrame;
	std::vector<cv::Rect> ROIs;
	std::vector<cv::Vec2f> markersCoordinates2D;// = std::vector<cv::Vec2f>(expectedNumberOfMarkers);
	int threshLevel = 70;
	int expectedNumberOfMarkers;
public:
	~PS3EyeCapture()
	{
		camera->stop();
		camera = NULL;
		cv::imshow("zamnkieto ps3eye", cv::imread("C/Users/Kalin/Desktop/pierdoly/memes/outstanding.png"));
		cv::waitKey(0);
	}
	ps3eye::PS3EYECam::PS3EYERef getCamera();
	void setCamera(ps3eye::PS3EYECam::PS3EYERef cam);
	void setMultiTracker(cv::Ptr<cv::legacy::MultiTracker> tracker);
	cv::Ptr<cv::legacy::MultiTracker> getMultiTracker();
	void setCurrentFrame(cv::Mat frame);
	cv::Mat getCurrentFrame();
	void setROIs(std::vector<cv::Vec3f> detectedMarkers);
	void startMultiTracker();
	void stopMultiTracker();
	void clearROIs();
	std::vector<cv::Rect> getROIs();
	void updateTracker();
	bool calculateMarkersCoordinates();
	std::vector<cv::Vec2f> getMarkersCoordinates();
	void setThreshLevel(int level);
	int getThreshLevel();
	void setExpectedNumberOfMarkers(int number);
	void detectMarkers();
};