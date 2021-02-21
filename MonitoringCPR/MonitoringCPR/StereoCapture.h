#pragma once
#include<PS3EyeCapture.h>
class StereoCapture
{
	bool isTracking = false;
	Matrices matrices;
	PS3EyeCapture first;
	PS3EyeCapture second;
	cv::Mat triangulatedCoordinates;
	
public:
	static StereoCapture* getInstance()
	{
		static StereoCapture instance;
		return &instance;
	}
	StereoCapture()
	{
	}
	~StereoCapture()
	{
	}
	StereoCapture(StereoCapture const&) = delete;
	void operator=(StereoCapture const&) = delete;
	int initCameras();
	void updateFrames(int width, int height);
	PS3EyeCapture& getFirstCapture();
	PS3EyeCapture& getSecondCapture();
	void freeCameras();
	void initializeMatrices();
	Matrices& getMatrices();
	bool getTrackingState();
	void setTrackingState(bool value);
	void triangulateCameras();
	cv::Mat getTriangCoordinates();
};
