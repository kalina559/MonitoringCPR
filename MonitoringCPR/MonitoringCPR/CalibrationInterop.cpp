#pragma once
#include "CameraCalibration.h"
#include"StereoCapture.h"

std::vector<std::pair<std::string, std::string>> validStereoFramesPaths;

extern "C" bool __declspec(dllexport) __stdcall checkStereoCalibrationFrames(int& invalidFramesCount, int& framesWithoutPair, int& totalFramesCount)
{
	validStereoFramesPaths.clear();
	std::string path1 = "../MonitoringCPR/CalibrationImages/Stereo/firstCam/";
	std::string path2 = "../MonitoringCPR/CalibrationImages/Stereo/secondCam/";
	std::string imgFormat = "*.jpg";

	auto singleFrames = CameraCalibration::checkFramesPairs(path1, path2, imgFormat);
	framesWithoutPair = singleFrames.size();
	while (singleFrames.size() > 0)
	{
		remove((singleFrames.back()).c_str());
		singleFrames.pop_back();
	}
	int invalid = 0;

	std::vector<cv::String> fileNames1, fileNames2;
	std::vector<int> invalidIds;
	cv::glob(path1 + imgFormat, fileNames1, false);
	cv::glob(path2 + imgFormat, fileNames2, false);
	if (fileNames1.size() == fileNames2.size())
	{
		totalFramesCount = fileNames1.size();
	}
	for (size_t i = 0; i < fileNames1.size(); ++i)
	{
		cv::Mat img1 = cv::imread(fileNames1[i]);
		cv::Mat img2 = cv::imread(fileNames2[i]);
		cv::Mat gray1, gray2;
		cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
		cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);
		std::vector<cv::Point2f> centers1, centers2;
		cv::Ptr<cv::FeatureDetector> blobDetector = ImgProcUtility::initBlobDetector();

		bool patternFound1 = findCirclesGrid(gray1, arrayOfCirclesSize, centers1, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
		bool patternFound2 = findCirclesGrid(gray2, arrayOfCirclesSize, centers2, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

		if (patternFound1 && patternFound2)
		{
			invalidIds.push_back(i);
			validStereoFramesPaths.push_back({ fileNames1[i], fileNames2[i] });
		}
		else
		{
			++invalid;
			remove(fileNames1[i].c_str());
			remove(fileNames2[i].c_str());
		}
	}
	invalidFramesCount = invalid;
	if (totalFramesCount == invalidFramesCount)
	{
		return false; 
	}
	return true;     
}
extern "C" void __declspec(dllexport) __stdcall showValidStereoFrame(unsigned char* firstFrameData, unsigned char* secondFrameData)
{
	auto firstFrame = cv::imread(validStereoFramesPaths.back().first);
	auto secondFrame = cv::imread(validStereoFramesPaths.back().second);

	cv::Mat gray1, gray2;
	cvtColor(firstFrame, gray1, cv::COLOR_BGR2GRAY);
	cvtColor(secondFrame, gray2, cv::COLOR_BGR2GRAY);
	std::vector<cv::Point2f> centers1, centers2;

	cv::SimpleBlobDetector::Params params;
	params.minArea = 10;
	params.minThreshold = 1;

	params.filterByConvexity = 1;
	params.minConvexity = 0.5;

	cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);

	bool patternFound1 = findCirclesGrid(gray1, arrayOfCirclesSize, centers1, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
	bool patternFound2 = findCirclesGrid(gray2, arrayOfCirclesSize, centers2, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

	drawChessboardCorners(firstFrame, arrayOfCirclesSize, cv::Mat(centers1), patternFound1);
	drawChessboardCorners(secondFrame, arrayOfCirclesSize, cv::Mat(centers2), patternFound2);

	cv::Mat firstArgbImg, secondArgbImg;
	cv::cvtColor(firstFrame, firstArgbImg, cv::COLOR_BGR2RGBA);
	cv::cvtColor(secondFrame, secondArgbImg, cv::COLOR_BGR2RGBA);
	std::memcpy(firstFrameData, firstArgbImg.data, firstArgbImg.total() * firstArgbImg.elemSize());
	std::memcpy(secondFrameData, secondArgbImg.data, secondArgbImg.total() * secondArgbImg.elemSize());
}
extern "C" void __declspec(dllexport) __stdcall clearStereoCalibrationFramesFolder()
{
	std::string command = "del /Q ";
	std::string path = "..\\MonitoringCPR\\CalibrationImages\\Stereo\\firstCam\\*.jpg";
	system(command.append(path).c_str());

	command = "del /Q ";
	path = "..\\MonitoringCPR\\CalibrationImages\\Stereo\\secondCam\\*.jpg";
	system(command.append(path).c_str());
}
extern "C" void __declspec(dllexport) __stdcall deleteCurrentStereoFrames()
{
	remove(validStereoFramesPaths.back().first.c_str());
	remove(validStereoFramesPaths.back().second.c_str());
}
extern "C" bool __declspec(dllexport) __stdcall moveToNextStereoFrames()
{
	if (validStereoFramesPaths.size() > 1)
	{
		validStereoFramesPaths.pop_back();
	}
	else
	{
		return false;
	}
}
extern "C" void __declspec(dllexport) __stdcall  stereoCalibrate(int& pairCount, int& time, bool& isFinished)
{
	Uint32 start_ticks = SDL_GetTicks();
	std::vector<cv::Mat> images1, images2, singleFrames1, singleFrames2;

	CameraCalibration::loadPhotos("..\\MonitoringCPR\\CalibrationImages\\Stereo\\firstCam\\*.jpg", images1);
	CameraCalibration::loadPhotos("..\\MonitoringCPR\\CalibrationImages\\Stereo\\secondCam\\*.jpg", images2);

	CameraCalibration::loadPhotos("..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\firstCam\\*.jpg", singleFrames1);
	CameraCalibration::loadPhotos("..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\secondCam\\*.jpg", singleFrames2);

	cv::Mat firstCamCoeffs, secondCamCoeffs, firstCamMatrix, secondCamMatrix;

	CameraCalibration::getSingleCamerasCoeffs(singleFrames1, singleFrames2, arrayOfCirclesSize, distanceBetweenCircles, firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs);

	cv::Mat R, T, E, F;

	double rms = CameraCalibration::stereoCalibration(images1, images2,
		firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs, arrayOfCirclesSize, R, T, E, F, distanceBetweenCircles);

	std::ofstream outStream("RMS.txt", std::ofstream::app);
	if (outStream)
	{
		outStream << "stereo RMS: " << rms << "\n";
	}
	outStream.close();

	cv::Mat R1, R2, P1, P2, Q;
	stereoRectify(firstCamMatrix, firstCamCoeffs, secondCamMatrix, secondCamCoeffs, cv::Size(1224, 1024), R, T, R1, R2, P1, P2, Q);
	CameraCalibration::saveRectifiedMatrices(R1, R2, P1, P2, Q);
	Uint32 end_ticks = SDL_GetTicks();
	pairCount = images1.size();
	time = (end_ticks - start_ticks) / 1000;
	isFinished = true;
	StereoCapture::getInstance()->initializeMatrices();
}

extern "C" int __declspec(dllexport) __stdcall  getEstimatedCalibrationTime()  //mozna troche dopracowac
{
	std::vector<cv::String> fileNamesVec = ImgProcUtility::getFileNames("..\\MonitoringCPR\\CalibrationImages\\Stereo\\firstCam\\*.jpg");	
	int x = fileNamesVec.size();            //number of pairs of frames
	/*double estimatedTime = 0.0008 * pow(x, 3) - 0.0138 * pow(x, 2) + 0.393 * x - 1.2881;*/
	double estimatedTime = 0.0004 * pow(x, 3) + 0.0216 * pow(x, 2) - 0.5134 * x + 2.8644;

	return ceil(estimatedTime);
}
//KALIBRACJA POJEDYNCZYCH KAMER:
std::vector<std::string> validSingleCameraFramesPaths;

extern "C" void __declspec(dllexport) __stdcall showValidSingleFrame(unsigned char* data)
{
	auto frame = cv::imread(validSingleCameraFramesPaths.back());

	cv::Mat gray;
	cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
	std::vector<cv::Point2f> centers;

	cv::SimpleBlobDetector::Params params;
	params.minThreshold = 1;

	params.filterByConvexity = 1;
	params.minConvexity = 0.5;

	cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);

	bool patternFound1 = findCirclesGrid(gray, arrayOfCirclesSize, centers, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

	drawChessboardCorners(frame, arrayOfCirclesSize, cv::Mat(centers), patternFound1);

	cv::Mat argbImg;
	cv::cvtColor(frame, argbImg, cv::COLOR_BGR2RGBA);
	std::memcpy(data, argbImg.data, argbImg.total() * argbImg.elemSize());
}
extern "C" bool __declspec(dllexport) __stdcall  checkSingleCameraCalibrationFrames(int& invalidFramesCount, int& totalFramesCount, int cameraId)
{
	std::string path;
	validSingleCameraFramesPaths.clear();
	if (cameraId == singleCameraId::firstCamera)
	{
		path = "../MonitoringCPR/CalibrationImages/SingleCamera/firstCam/";
	}
	else if(cameraId == singleCameraId::secondCamera)
	{
		path = "../MonitoringCPR/CalibrationImages/SingleCamera/secondCam/";
	}
	int invalid = 0;

	std::vector<cv::String> fileNames;
	cv::glob(path, fileNames, false);

	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		cv::Mat img = cv::imread(fileNames[i]);

		cv::Mat gray;
		cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		std::vector<cv::Point2f> centers;

		cv::Ptr<cv::FeatureDetector> blobDetector = ImgProcUtility::initBlobDetector();

		bool patternFound = findCirclesGrid(gray, arrayOfCirclesSize, centers, cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);

		if (patternFound)
		{
			validSingleCameraFramesPaths.push_back(fileNames[i]);
		}
		else
		{
			++invalid;
			remove(fileNames[i].c_str());
		}
	}

	totalFramesCount = fileNames.size();
	invalidFramesCount = invalid;

	if (invalid == totalFramesCount)
	{
		return false;  //brak poprawnych zdjec
	}
	return true;     //wszystko git
}
extern "C" void __declspec(dllexport) __stdcall deleteCurrentSingleCameraFrame()
{
	remove(validSingleCameraFramesPaths.back().c_str());
}
extern "C" bool __declspec(dllexport) __stdcall moveToNextSingleCameraFrame()
{
	if (validSingleCameraFramesPaths.size() > 1)
	{
		validSingleCameraFramesPaths.pop_back();
	}
	else
	{
		return false;
	}
}
extern "C" void __declspec(dllexport) __stdcall clearSingleCameraFramesFolder(int cameraId)
{
	std::string command = "del /Q ";
	std::string path;
	if (cameraId == singleCameraId::firstCamera)
	{
		path = "..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\firstCam\\*.jpg";
	}
	else if (singleCameraId::secondCamera)
	{
		path = "..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\secondCam\\*.jpg";
	}
	system(command.append(path).c_str());
}
enum frameSetMode
{
	first,
	second,
	stereo
};
extern "C" __declspec(dllexport) BSTR __stdcall  getFramesSetId(int mode)
{
	if (mode == frameSetMode::first)
	{
		return ImgProcUtility::getFrameSetId("..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\firstCam\\*.jpg");
	}
	else if (mode == frameSetMode::second)
	{
		return ImgProcUtility::getFrameSetId("..\\MonitoringCPR\\CalibrationImages\\SingleCamera\\secondCam\\*.jpg");
	}
	else if (mode == frameSetMode::stereo)
	{
		bstr_t temp = ImgProcUtility::getFrameSetId("..\\MonitoringCPR\\CalibrationImages\\Stereo\\firstCam\\*.jpg");
		temp += ImgProcUtility::getFrameSetId("..\\MonitoringCPR\\CalibrationImages\\Stereo\\secondCam\\*.jpg");
		return temp.copy();
	}
}