#pragma once
#include<PS3EyeCapture.h>
#include<CameraCalibrationUtility.h>
class StereoCapture
{
	bool isTracking = false; /**< Indicates if the system is being used to track markers*/
	ImgProcUtility::Matrices matrices; /**< Set of camera intrinsic/extrinsic parameters, as well as the distortion coefficients vectors*/
	PS3EyeCapture first; /**< First camera capture */
	PS3EyeCapture second; /**< Second camera capture*/
	cv::Mat triangulatedCoordinates; /**< Matrix contatining triangulated coordinates of the markers*/
	int expectedNumberOfMarkerPairs = 0; /**< Number of markers that need to be detected before monitoring can begin*/
	float fps = 0; /**< Frames per second, updates every second*/
	int64 lastTimeStamp = 0; /**< Timestamp of the last fps measurement*/
	int frameCount = 0; /**< Number of frames since the last fps measurement*/

public:
	/**
	 * @brief Returns the only instance of the class. Built using the singleton pattern
	 */
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
	/**
     * @brief Calculates FPS in the last second
	 */
	void calculateFPS();
	float getFPS();
	PS3EyeCapture& getFirstCapture();
	PS3EyeCapture& getSecondCapture();
	/**
	 * @brief Stops the capture of frames
	 */
	void freeCameras();
	/**
	 * @brief Loads matrices from the text files
	 */
	void initializeMatrices();
	ImgProcUtility::Matrices& getMatrices();
	bool getTrackingState();
	void setTrackingState(bool value);
	void triangulateCameras();
	cv::Mat getTriangCoordinates();
	void setTreshLevel(int level);
	void setExpectedNumberOfMarkerPairs(int number);
	/**
	 * @brief Checks if all the markers are detected in both pictures
	 * @param firstCircles - Vector of coordinates of markers in the first image
	 * @param secondCircles - Vector of coordinates of markers in the second image
	 */
	bool checkIfAllMarkersDetected(std::vector<cv::Vec3f> firstCircles, std::vector<cv::Vec3f> secondCircles);
	/**
	 * @brief Monitors the location of markers in both images
	 * @param firstFrameData, secondFrameData - Frame data later passed to Unity
	 * @param performTracking - bool variable, its value depend on whether or not all the markers are detected
	 */
	bool realTimeMonitoring(unsigned char* firstFrameData, unsigned char* secondFrameData, bool& performTracking);
};
