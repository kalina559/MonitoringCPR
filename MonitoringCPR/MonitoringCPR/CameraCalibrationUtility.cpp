#include "CameraCalibrationUtility.h"

void CameraCalibrationUtility::loadPhotos(std::string path, std::vector<cv::Mat>& images)
{
	std::vector<cv::String> fileNames;
	cv::glob(path, fileNames, false);

	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		images.push_back(cv::imread(fileNames[i]));
	}
}
std::vector<std::string> CameraCalibrationUtility::checkFramesPairs(std::string firstPath, std::string secondPath, std::string format)
{
	std::vector<cv::String> firstFilePaths, secondFilePaths;
	cv::glob(firstPath + format, firstFilePaths, false);
	cv::glob(secondPath + format, secondFilePaths, false);
	auto firstFileNames = getFileNames(firstFilePaths);
	auto secondFileNames = getFileNames(secondFilePaths);

	std::vector<std::string> framesWithoutPairPaths;

	for (const std::string& fileName : firstFileNames)
	{
		if (secondFileNames.find(fileName) == secondFileNames.end())
		{
			framesWithoutPairPaths.push_back(firstPath + fileName);
		}
	}
	for (const std::string& fileName : secondFileNames)
	{
		if (firstFileNames.find(fileName) == firstFileNames.end())
		{
			framesWithoutPairPaths.push_back(secondPath + fileName);
		}
	}
	return framesWithoutPairPaths;
}

std::set<std::string> CameraCalibrationUtility::getFileNames(std::vector<cv::String> filePaths)
{
	std::set<std::string> fileNames;
	for (int i = 0; i < filePaths.size(); ++i)
	{
		auto fileName = std::filesystem::path(filePaths[i]).filename();
		fileNames.insert(fileName.string());
	}
	return fileNames;
}

void CameraCalibrationUtility::realLifeCirclePositions(cv::Size boardSize, float distance, std::vector<cv::Point3f>& circleCenters)
{
	for (int i = 0; i < boardSize.height; ++i)
	{
		for (int j = 0; j < boardSize.width; ++j)
		{
			if (i % 2 == 0)
				circleCenters.push_back(cv::Point3f(j * distance, i * distance / 2, 0.0f));
			else
				circleCenters.push_back(cv::Point3f((j + 0.5) * distance, i * distance / 2, 0.0f));      //przesuniete, bo assymetric grid.		BARDZO WAZNY ZNAK +/- 0.5. NajwyraŸniej górny prawy róg wzoru jest 'ob³y'
		}
	}
}

void CameraCalibrationUtility::getCirclePositions(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& centers)
{
	//subpix
	cv::Size winSize = cv::Size(5, 5);
	cv::Size zeroZone = cv::Size(-1, -1);
	cv::Mat grayImg;
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 40, 0.001);

	cv::SimpleBlobDetector::Params params;
	params.minThreshold = 1;

	params.filterByConvexity = 1;
	params.minConvexity = 0.5;

	cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);
	for (size_t i = 0; i < images.size(); ++i) 
	{
		cv::cvtColor(images[i], grayImg, cv::COLOR_BGR2GRAY);
		bool patternFound = cv::findCirclesGrid(grayImg, arrayOfCirclesSize, centers[i], cv::CALIB_CB_ASYMMETRIC_GRID + cv::CALIB_CB_CLUSTERING, blobDetector);
	}
}

double CameraCalibrationUtility::singleCameraCalibration(std::vector<cv::Mat> calibrationImages, cv::Mat& cameraMatrix, cv::Mat& distCoefficients)
{
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	realLifeCirclePositions(arrayOfCirclesSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(calibrationImages.size(), worldSpaceCircleCenters[0]);

	std::vector<cv::Mat> rVectors, tVectors;
	distCoefficients = cv::Mat::zeros(8, 1, CV_64F);

	std::vector<std::vector<cv::Point2f> > centers(calibrationImages.size());

	getCirclePositions(calibrationImages, centers);

	return calibrateCamera(worldSpaceCircleCenters, centers, arrayOfCirclesSize, cameraMatrix, distCoefficients, rVectors, tVectors);
}

bool CameraCalibrationUtility::saveMatrix(std::string path, cv::Mat mat)
{
	std::ofstream outStream(path);
	if (outStream)
	{
		size_t rows = mat.rows;
		size_t columns = mat.cols;

		for (size_t i = 0; i < rows; ++i)
		{
			for (size_t j = 0; j < columns; ++j)
			{
				double value = mat.at<double>(i, j);
				outStream << value << std::endl;
			}
		}
		outStream.close();
		return true;
	}
	return false;
}

bool CameraCalibrationUtility::loadMatrix(std::string path, cv::Size size, cv::Mat& outMatrix)
{
	std::ifstream ifStream(path);
	if (ifStream)
	{
		for (size_t i = 0; i < size.width; ++i)
		{
			for (size_t j = 0; j < size.height; ++j)
			{
				ifStream >> outMatrix.at<double>(i, j);
			}
		}
		ifStream.close();
		return true;
	}
	return false;
}

double CameraCalibrationUtility::stereoCalibration(std::vector<cv::Mat> images1, std::vector<cv::Mat> images2, cv::Mat firstCamMatrix, cv::Mat secondCamMatrix, cv::Mat firstCamCoeffs,
	cv::Mat secondCamCoeffs, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F)
{
	std::vector<std::vector<cv::Point2f> > centers1(images1.size());
	CameraCalibrationUtility::getCirclePositions(images1, centers1);
	std::vector<std::vector<cv::Point2f> > centers2(images2.size());
	CameraCalibrationUtility::getCirclePositions(images2, centers2);
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	CameraCalibrationUtility::realLifeCirclePositions(arrayOfCirclesSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(images1.size(), worldSpaceCircleCenters[0]);

	double rms = stereoCalibrate(worldSpaceCircleCenters, centers1, centers2, firstCamMatrix, firstCamCoeffs, secondCamMatrix,
		secondCamCoeffs, cv::Size(1224, 1024), R, T, E, F, cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6));
	//std::cout << "after stereo" << std::endl;
	CameraCalibrationUtility::saveMatrix("matrices/stereoCalibration/R", R);
	CameraCalibrationUtility::saveMatrix("matrices/stereoCalibration/T", T);  //todo te sciezki jako const string
	CameraCalibrationUtility::saveMatrix("matrices/stereoCalibration/E", E);
	CameraCalibrationUtility::saveMatrix("matrices/stereoCalibration/F", F);
	return rms;
}

void CameraCalibrationUtility::getSingleCamerasCoeffs(std::vector<cv::Mat> firstCamImgs, std::vector<cv::Mat> secondCamImgs,
	cv::Mat& firstCamMatrix, cv::Mat& secondCamMatrix, cv::Mat& firstCamCoeffs, cv::Mat& secondCamCoeffs)
{
	double firstRMS = CameraCalibrationUtility::singleCameraCalibration(firstCamImgs, firstCamMatrix, firstCamCoeffs);
	double secondRMS = CameraCalibrationUtility::singleCameraCalibration(secondCamImgs, secondCamMatrix, secondCamCoeffs);

	std::ofstream outStream("RMS.txt", std::ofstream::app);
	if (outStream)
	{
		std::string timestamp = ImgProcUtility::getCurrentDateStr() + "	" + ImgProcUtility::getCurrentTimeStr();
		outStream << timestamp + "\nfirst camera RMS: " + std::to_string(firstRMS) + "\nsecond camera RMS: " + std::to_string(secondRMS) + "\n";
	}
	outStream.close();

	//saving matrices to Unity Project directory
	CameraCalibrationUtility::saveMatrix("matrices/singleCamCalibration/firstCamMatrix", firstCamMatrix);
	CameraCalibrationUtility::saveMatrix("matrices/singleCamCalibration/secondCamMatrix", secondCamMatrix);
	CameraCalibrationUtility::saveMatrix("matrices/singleCamCalibration/firstCamCoeffs", firstCamCoeffs);
	CameraCalibrationUtility::saveMatrix("matrices/singleCamCalibration/secondCamCoeffs", secondCamCoeffs);
}

void CameraCalibrationUtility::saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q)
{
	CameraCalibrationUtility::saveMatrix("matrices/stereoRectifyResults/R1", R1);
	CameraCalibrationUtility::saveMatrix("matrices/stereoRectifyResults/R2", R2);
	CameraCalibrationUtility::saveMatrix("matrices/stereoRectifyResults/P1", P1);
	CameraCalibrationUtility::saveMatrix("matrices/stereoRectifyResults/P2", P2);
	CameraCalibrationUtility::saveMatrix("matrices/stereoRectifyResults/Q", Q);
}
