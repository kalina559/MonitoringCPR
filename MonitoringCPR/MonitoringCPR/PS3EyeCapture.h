#pragma once
#include "ImgProcUtility.h"

class PS3EyeCapture
{
	ps3eye::PS3EYECam::PS3EYERef camera = NULL /**< Shared pointer to the camera that is being handled*/;
	cv::Ptr<cv::legacy::MultiTracker> multiTracker; /**< Multitracker responsible for tracking of all the markers*/;
	cv::Mat currentFrame; /**< Frame that is currently analysed*/;
	std::vector<cv::Rect> ROIs; /**< Region of interests that are currently tracked or if the monitoring hasn't started yet - can be used to start monitoring*/;
	std::vector<cv::Vec2f> markersCoordinates2D; /**< Coordinates of markers on the current frame*/;
	int threshLevel = 70; /**< Threshold level used in canny edge detection to find markers' centers*/;
	int expectedNumberOfMarkers = 0; /**< Number of markers that have to be detected on the image to start/keep monitoring*/;
public:
	ps3eye::PS3EYECam::PS3EYERef getCamera();
	void setCamera(ps3eye::PS3EYECam::PS3EYERef cam);
	void setMultiTracker(cv::Ptr<cv::legacy::MultiTracker> tracker);
	cv::Ptr<cv::legacy::MultiTracker> getMultiTracker();
	void setCurrentFrame(cv::Mat frame);
	cv::Mat getCurrentFrame();
	void setROIs(std::vector<cv::Vec3f> detectedCircles);
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
	/**
	 * @brief Detects markers on the current frame
	 * @param displayFrame - output frame used to display the position and index of the marker
	 */
	bool detectMarkers(cv::Mat& displayFrame);
	/**
	 * @brief Checks if all the markers are detected in the frame
	 * @param circles - Vector of coordinates of markers in the frame
	 */
	bool checkIfAllMarkersDetected(std::vector<cv::Vec3f> circles);
};