#include "StereoCapture.h"
int StereoCapture::initCameras()
{
	auto threshLevelStr = ImgProcUtility::readFile("C:/dev/MonitoringCPR/MonitoringCPRUnityProject/thresholdLevel.txt");
	int threshLevel = std::stoi(threshLevelStr);
	if (!(threshLevel >= 0) && !(threshLevel <= 255))
		threshLevel = 70;
	first.setThreshLevel(threshLevel);
	first.setThreshLevel(threshLevel);

	auto& devices = ps3eye::PS3EYECam::getDevices(true);
	if (devices.empty())
	{
		return -1;
	}
	first.setCamera(devices[0]);
	second.setCamera(devices[1]);
	bool success1 = first.getCamera()->init(640, 480, 60);
	bool success2 = second.getCamera()->init(640, 480, 60);

	if (!success1 || !success2)
	{
		return -2;
	}
	first.getCamera()->start();
	second.getCamera()->start();
	return 0;
}

void StereoCapture::updateFrames(int width, int height)
{
	cv::Mat firstFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
	cv::Mat secondFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

	first.getCamera()->getFrame(firstFrame.data);
	second.getCamera()->getFrame(secondFrame.data);

	cv::Mat firstResizedMat(height, width, firstFrame.type());
	cv::Mat secondResizedMat(height, width, secondFrame.type());

	cv::resize(firstFrame, firstResizedMat, firstResizedMat.size(), cv::INTER_CUBIC);
	cv::resize(secondFrame, secondResizedMat, secondResizedMat.size(), cv::INTER_CUBIC);

	first.setCurrentFrame(firstResizedMat);
	second.setCurrentFrame(secondResizedMat);
	calculateFPS();
}
void StereoCapture::updateFrames(int width, int height, int64& delay)
{
	cv::Mat firstFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
	cv::Mat secondFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

	int64 firstTimeStamp = GetTickCount64();
	first.getCamera()->getFrame(firstFrame.data);
	second.getCamera()->getFrame(secondFrame.data);
	delay = GetTickCount64() - firstTimeStamp;

	cv::Mat firstResizedMat(height, width, firstFrame.type());
	cv::Mat secondResizedMat(height, width, secondFrame.type());

	cv::resize(firstFrame, firstResizedMat, firstResizedMat.size(), cv::INTER_CUBIC);
	cv::resize(secondFrame, secondResizedMat, secondResizedMat.size(), cv::INTER_CUBIC);

	first.setCurrentFrame(firstResizedMat);
	second.setCurrentFrame(secondResizedMat);
	calculateFPS();
}
void StereoCapture::calculateFPS()
{
	++frameCount;
	if (lastTimeStamp == 0)
	{
		lastTimeStamp = SDL_GetTicks();
	}
	auto elapsedTime = SDL_GetTicks() - lastTimeStamp;

	if (elapsedTime > 1000)
	{
		fps = 1000 * float(frameCount) / elapsedTime;
		lastTimeStamp = SDL_GetTicks();
		frameCount = 0;
	}
}
float StereoCapture::getFPS()
{
	return fps;
}
PS3EyeCapture& StereoCapture::getFirstCapture()
{
	return first;
}
PS3EyeCapture& StereoCapture::getSecondCapture()
{
	return second;
}
void StereoCapture::freeCameras()
{
	getInstance()->getFirstCapture().getCamera()->stop();
	getInstance()->getFirstCapture().setCamera(NULL);
	getInstance()->getSecondCapture().getCamera()->stop();
	getInstance()->getSecondCapture().setCamera(NULL);
}
void StereoCapture::initializeMatrices()
{
	CameraCalibrationUtility::loadMatrix("matrices/singleCamCalibration/firstCamMatrix", cv::Size(3, 3), matrices.firstCamMatrix);
	CameraCalibrationUtility::loadMatrix("matrices/singleCamCalibration/secondCamMatrix", cv::Size(3, 3), matrices.secondCamMatrix);
	CameraCalibrationUtility::loadMatrix("matrices/singleCamCalibration/firstCamCoeffs", cv::Size(5, 1), matrices.firstCamCoeffs);
	CameraCalibrationUtility::loadMatrix("matrices/singleCamCalibration/secondCamCoeffs", cv::Size(5, 1), matrices.secondCamCoeffs);
	CameraCalibrationUtility::loadMatrix("matrices/stereoRectifyResults/P1", cv::Size(3, 4), matrices.P1);
	CameraCalibrationUtility::loadMatrix("matrices/stereoRectifyResults/P2", cv::Size(3, 4), matrices.P2);
	CameraCalibrationUtility::loadMatrix("matrices/stereoRectifyResults/R1", cv::Size(3, 3), matrices.R1);
	CameraCalibrationUtility::loadMatrix("matrices/stereoRectifyResults/R2", cv::Size(3, 3), matrices.R2);
}
ImgProcUtility::Matrices& StereoCapture::getMatrices()
{
	return matrices;
}
bool StereoCapture::getTrackingState()
{
	return isTracking;
}
void StereoCapture::setTrackingState(bool value)
{
	isTracking = value;
}

void StereoCapture::triangulateCameras()
{
	ImgProcUtility::StereoCoordinates2D stereoCoordinates2D = { first.getMarkersCoordinates(), second.getMarkersCoordinates() };
	triangulatedCoordinates = ImgProcUtility::getMarkers3DCoordinates(stereoCoordinates2D, matrices);
}
cv::Mat StereoCapture::getTriangCoordinates()
{
	return triangulatedCoordinates;
}

void StereoCapture::setTreshLevel(int level)
{
	first.setThreshLevel(level);
	second.setThreshLevel(level);
}

void StereoCapture::setExpectedNumberOfMarkerPairs(int number)
{
	expectedNumberOfMarkerPairs = number;
	first.setExpectedNumberOfMarkers(number);
	second.setExpectedNumberOfMarkers(number);
}

bool StereoCapture::checkIfAllMarkersDetected(std::vector<cv::Vec3f> firstCircles, std::vector<cv::Vec3f> secondCircles)
{
	return (firstCircles.size() == expectedNumberOfMarkerPairs && secondCircles.size() == expectedNumberOfMarkerPairs);
}

bool StereoCapture::realTimeMonitoring(unsigned char* firstFrameData, unsigned char* secondFrameData, bool& performTracking)
{
	if (!isTracking)       //jesli jeszcze nie zaczeto trackowania, inicjalizacja multitrackerow
	{
		first.startMultiTracker();
		second.startMultiTracker();
		isTracking = true;
	}

	first.updateTracker();
	second.updateTracker();

	bool result1 = first.calculateMarkersCoordinates();
	bool result2 = second.calculateMarkersCoordinates();

	if (!result1 || !result2)
	{
		isTracking = false;
		performTracking = false;
		first.stopMultiTracker();
		second.stopMultiTracker();
		return false;
	}
	triangulateCameras();

	ImgProcUtility::passFrameToUnity(first.getCurrentFrame(), firstFrameData);
	ImgProcUtility::passFrameToUnity(second.getCurrentFrame(), secondFrameData);
	return true;
}

