#pragma once
#include<PS3EyeCapture.h>
#include<CameraCalibration.h>
class StereoCapture
{
	bool isTracking = false;
	Matrices matrices;
	PS3EyeCapture first;
	PS3EyeCapture second;
	cv::Mat triangulatedCoordinates;
	int expectedNumberOfMarkerPairs = 0;
	float fps = 0;
	int64 lastTimeStamp = 0;
	int frameCount = 0;
	
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
	void updateFrames(int width, int height, int64& delay);
	void calculateFPS();
	float getFPS();
	PS3EyeCapture& getFirstCapture();
	PS3EyeCapture& getSecondCapture();
	void freeCameras();
	void initializeMatrices();
	Matrices& getMatrices();
	bool getTrackingState();
	void setTrackingState(bool value);
	void triangulateCameras();
	cv::Mat getTriangCoordinates();
	void setTreshLevel(int level);
	void setExpectedNumberOfMarkerPairs(int number);
	bool checkIfAllMarkersDetected(std::vector<cv::Vec3f> firstCircles, std::vector<cv::Vec3f> secondCircles);
	bool realTimeMonitoring(unsigned char* firstFrameData, unsigned char* secondFrameData, bool& performTracking);
};
