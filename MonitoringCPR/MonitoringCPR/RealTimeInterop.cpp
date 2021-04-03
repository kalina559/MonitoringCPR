#pragma once
#include "StereoCapture.h"
#include "ImgProcUtility.h"
extern "C"
{
	int __declspec(dllexport) __stdcall InitSDLCameras(int& outCameraWidth, int& outCameraHeight)
	{
		StereoCapture::getInstance()->initializeMatrices();
		return StereoCapture::getInstance()->initCameras();
	}

	void __declspec(dllexport) __stdcall GetCalibrationFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
	{
		StereoCapture::getInstance()->updateFrames(width, height);
		cv::Mat firstFrameCopy, secondFrameCopy;
		StereoCapture::getInstance()->getFirstCapture().getCurrentFrame().copyTo(firstFrameCopy);
		StereoCapture::getInstance()->getSecondCapture().getCurrentFrame().copyTo(secondFrameCopy);

		cv::Ptr<cv::FeatureDetector> blobDetector = ImgProcUtility::initBlobDetector();

		ImgProcUtility::drawDetectedCirclesGrid(firstFrameCopy, blobDetector);
		ImgProcUtility::drawDetectedCirclesGrid(secondFrameCopy, blobDetector);

		ImgProcUtility::passFrameToUnity(firstFrameCopy, firstFrameData);
		ImgProcUtility::passFrameToUnity(secondFrameCopy, secondFrameData);
	}

	void __declspec(dllexport) __stdcall GetCurrentThreshFrame(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height, int threshLevel)
	{
		StereoCapture::getInstance()->updateFrames(width, height);
		cv::Mat firstGray, secondGray;
		cvtColor(StereoCapture::getInstance()->getFirstCapture().getCurrentFrame(), firstGray, cv::COLOR_BGR2GRAY);
		cvtColor(StereoCapture::getInstance()->getSecondCapture().getCurrentFrame(), secondGray, cv::COLOR_BGR2GRAY);
		auto threshImages = ImgProcUtility::thresholdImages({ firstGray, secondGray }, threshLevel);

		ImgProcUtility::passFrameToUnity(threshImages.first, firstFrameData);
		ImgProcUtility::passFrameToUnity(threshImages.second, secondFrameData);
	}

	void __declspec(dllexport) __stdcall saveCurrentFrames(int captureMode)
	{
		std::string timestamp = "img" + ImgProcUtility::getCurrentDateStr() + "_" + ImgProcUtility::getCurrentTimeStr() + ".jpg";
		std::replace(timestamp.begin(), timestamp.end(), ':', '_');
		std::replace(timestamp.begin(), timestamp.end(), '/', '_');
		if (captureMode == 0)
		{
			cv::imwrite("../MonitoringCPR/CalibrationImages/SingleCamera/firstCam/" + timestamp, StereoCapture::getInstance()->getFirstCapture().getCurrentFrame());
		}
		else if (captureMode == 1)
		{
			cv::imwrite("../MonitoringCPR/CalibrationImages/SingleCamera/secondCam/" + timestamp, StereoCapture::getInstance()->getSecondCapture().getCurrentFrame());
		}
		else if (captureMode == 2)
		{
			cv::imwrite("../MonitoringCPR/CalibrationImages/Stereo/firstCam/" + timestamp, StereoCapture::getInstance()->getFirstCapture().getCurrentFrame());
			cv::imwrite("../MonitoringCPR/CalibrationImages/Stereo/secondCam/" + timestamp, StereoCapture::getInstance()->getSecondCapture().getCurrentFrame());
		}
	}

	bool __declspec(dllexport) __stdcall detectMarkers(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height)
	{
		if (StereoCapture::getInstance()->getTrackingState())   //jeœli istniej¹ jakieœ trackery
		{
			StereoCapture::getInstance()->setTrackingState(false);
			StereoCapture::getInstance()->getFirstCapture().stopMultiTracker();
			StereoCapture::getInstance()->getSecondCapture().stopMultiTracker();
		}
		StereoCapture::getInstance()->updateFrames(width, height);

		cv::Mat firstFrame, secondFrame;
		bool firstFrameDetectionResult = StereoCapture::getInstance()->getFirstCapture().detectMarkers(firstFrame);
		bool secondFrameDetectionResult = StereoCapture::getInstance()->getSecondCapture().detectMarkers(secondFrame);

		ImgProcUtility::passFrameToUnity(firstFrame, firstFrameData);
		ImgProcUtility::passFrameToUnity(secondFrame, secondFrameData);

		return firstFrameDetectionResult && secondFrameDetectionResult;
	}

	void __declspec(dllexport) __stdcall setExpectedNumberOfMarkerPairs(int number)
	{
		StereoCapture::getInstance()->setExpectedNumberOfMarkerPairs(number);
	}

	extern "C" void __declspec(dllexport) __stdcall realTimeMonitoring(unsigned char* firstFrameData, unsigned char* secondFrameData, int width, int height, ImgProcUtility::Coordinates * outBalls, bool& performTracking, int64 & delay)
	{
		auto cap = StereoCapture::getInstance();
		cap->updateFrames(width, height, delay);

		if (!cap->realTimeMonitoring(firstFrameData, secondFrameData, performTracking))
		{
			return;
		}

		ImgProcUtility::homogenousToCartesianCoordinates(cap->getTriangCoordinates(), outBalls);
	}

	void __declspec(dllexport) __stdcall saveThreshLevel(int threshLevel)
	{
		std::ofstream outputFile;
		outputFile.open("thresholdLevel.txt");
		outputFile << threshLevel;
		outputFile.close();
		StereoCapture::getInstance()->setTreshLevel(threshLevel);
	}
	void __declspec(dllexport) __stdcall updateThreshLevel(int threshLevel)
	{
		StereoCapture::getInstance()->setTreshLevel(threshLevel);
	}
	int __declspec(dllexport) __stdcall  getThreshLevel()
	{
		return StereoCapture::getInstance()->getFirstCapture().getThreshLevel();
	}
	void __declspec(dllexport) __stdcall  CloseSDLCameras()
	{
		StereoCapture::getInstance()->freeCameras();
	}
	float __declspec(dllexport) __stdcall  getFPS()
	{
		return StereoCapture::getInstance()->getFPS();
	}
}