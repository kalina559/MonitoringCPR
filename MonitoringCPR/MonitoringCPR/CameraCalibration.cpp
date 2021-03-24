#include "CameraCalibration.h"

void CameraCalibration::loadPhotos(std::string path, std::vector<cv::Mat>& images)
{
	std::vector<cv::String> fileNames;
	cv::glob(path, fileNames, false);

	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		std::cout << "Processing photo nr: " << i << std::endl;
		images.push_back(cv::imread(fileNames[i]));
	}
}

std::vector<std::string> CameraCalibration::checkFramesPairs(std::string firstPath, std::string secondPath, std::string format)
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

std::set<std::string> CameraCalibration::getFileNames(std::vector<cv::String> filePaths)
{
	//std::vector<std::string> fileNames;
	std::set<std::string> fileNames;
	for (int i = 0; i < filePaths.size(); ++i)
	{
		auto fileName = std::filesystem::path(filePaths[i]).filename();
		fileNames.insert(fileName.string());
	}
	return fileNames;
}

void CameraCalibration::realLifeCirclePositions(cv::Size boardSize, float distance, std::vector<cv::Point3f>& circleCenters)
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

void CameraCalibration::getCirclePositions(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& centers, cv::Size arrayOfCirclesSize)
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

double CameraCalibration::singleCameraCalibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float distanceBetweenCircles, cv::Mat& cameraMatrix, cv::Mat& distCoefficients)
{
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(calibrationImages.size(), worldSpaceCircleCenters[0]);

	std::vector<cv::Mat> rVectors, tVectors;
	distCoefficients = cv::Mat::zeros(8, 1, CV_64F);

	std::vector<std::vector<cv::Point2f> > centers(calibrationImages.size());

	getCirclePositions(calibrationImages, centers, boardSize);

	return calibrateCamera(worldSpaceCircleCenters, centers, boardSize, cameraMatrix, distCoefficients, rVectors, tVectors);
}

bool CameraCalibration::saveMatrix(std::string name, cv::Mat mat)
{
	std::ofstream outStream(name);
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

bool CameraCalibration::loadMatrix(std::string name, int cols, int rows, cv::Mat& outMatrix)
{
	std::ifstream ifStream(name);
	if (ifStream)
	{
		for (size_t i = 0; i < cols; ++i)
		{
			for (size_t j = 0; j < rows; ++j)
			{
				ifStream >> outMatrix.at<double>(i, j);
			}
		}
		ifStream.close();
		return true;
	}
	return false;
}

double CameraCalibration::stereoCalibration(std::vector<cv::Mat> images1, std::vector<cv::Mat> images2, cv::Mat firstCamMatrix, cv::Mat secondCamMatrix, cv::Mat firstCamCoeffs,
	cv::Mat secondCamCoeffs, cv::Size boardSize, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F, float distanceBetweenCircles)
{
	std::vector<std::vector<cv::Point2f> > centers1(images1.size());
	CameraCalibration::getCirclePositions(images1, centers1, boardSize);
	std::vector<std::vector<cv::Point2f> > centers2(images2.size());
	CameraCalibration::getCirclePositions(images2, centers2, boardSize);
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	CameraCalibration::realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(images1.size(), worldSpaceCircleCenters[0]);

	double rms = stereoCalibrate(worldSpaceCircleCenters, centers1, centers2, firstCamMatrix, firstCamCoeffs, secondCamMatrix,
		secondCamCoeffs, cv::Size(1224, 1024), R, T, E, F, cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6));
	//std::cout << "after stereo" << std::endl;
	CameraCalibration::saveMatrix("matrices/stereoCalibration/R", R);
	CameraCalibration::saveMatrix("matrices/stereoCalibration/T", T);  //todo te sciezki jako const string
	CameraCalibration::saveMatrix("matrices/stereoCalibration/E", E);
	CameraCalibration::saveMatrix("matrices/stereoCalibration/F", F);
	return rms;
}

void CameraCalibration::getSingleCamerasCoeffs(std::vector<cv::Mat> firstCamImgs, std::vector<cv::Mat> secondCamImgs, cv::Size boardSize, float distanceBetweenCircles,
	cv::Mat& firstCamMatrix, cv::Mat& secondCamMatrix, cv::Mat& firstCamCoeffs, cv::Mat& secondCamCoeffs)
{
	double firstRMS = CameraCalibration::singleCameraCalibration(firstCamImgs, boardSize, distanceBetweenCircles, firstCamMatrix, firstCamCoeffs);
	double secondRMS = CameraCalibration::singleCameraCalibration(secondCamImgs, boardSize, distanceBetweenCircles, secondCamMatrix, secondCamCoeffs);

	std::ofstream outStream("RMS.txt", std::ofstream::app);
	if (outStream)
	{
		std::string timestamp = ImgProcUtility::getCurrentDateStr() + "	" + ImgProcUtility::getCurrentTimeStr();
		outStream << timestamp + "\nfirst camera RMS: " + std::to_string(firstRMS) + "\nsecond camera RMS: " + std::to_string(secondRMS) + "\n";
	}
	outStream.close();

	//saving matrices to Unity Project directory
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/firstCamMatrix", firstCamMatrix);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/secondCamMatrix", secondCamMatrix);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/firstCamCoeffs", firstCamCoeffs);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/secondCamCoeffs", secondCamCoeffs);
}

void CameraCalibration::saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q)
{
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/R1", R1);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/R2", R2);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/P1", P1);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/P2", P2);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/Q", Q);
}
