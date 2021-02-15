#include "CameraCalibration.h"

void CameraCalibration::loadPhotos(std::string path, std::vector<cv::Mat>& images)
{
	std::vector<cv::String> fileNames;
	cv::glob(path, fileNames, false);

	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		std::cout << "Processing photo nr: " << i << std::endl;
		cv::Mat resized;
		resize(cv::imread(fileNames[i]), resized, cv::Size(), 0.5, 0.5);
		images.push_back(resized);
	}
}

void CameraCalibration::realLifeCirclePositions(cv::Size boardSize, float distance, std::vector<cv::Point3f>& circleCenters)
{
	for (int i = 0; i < boardSize.height; ++i)
	{
		for (int j = 0; j < boardSize.width; ++j)
		{
			circleCenters.push_back(cv::Point3f(j * distance, i * distance, 0.0f));
		}
	}
}

void CameraCalibration::getCirclePositions(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& centers, cv::Size arrayOfCirclesSize)
{
	for (size_t i = 0; i < images.size(); ++i) {
		bool patternFound = findCirclesGrid(images[i], arrayOfCirclesSize, centers[i], cv::CALIB_CB_SYMMETRIC_GRID);
		//just for debug purposes
		//drawChessboardCorners(images[i], arrayOfCirclesSize, Mat(centers[i]), patternFound);
	}
}

void CameraCalibration::singleCameraCalibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float distanceBetweenCircles, cv::Mat& cameraMatrix, cv::Mat& distCoefficients)
{
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(calibrationImages.size(), worldSpaceCircleCenters[0]);

	std::vector<cv::Mat> rVectors, tVectors;
	distCoefficients = cv::Mat::zeros(8, 1, CV_64F);

	std::vector<std::vector<cv::Point2f> > centers(calibrationImages.size());

	getCirclePositions(calibrationImages, centers, boardSize);
	
	calibrateCamera(worldSpaceCircleCenters, centers, boardSize, cameraMatrix, distCoefficients, rVectors, tVectors);
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

void CameraCalibration::stereoCalibration(std::vector<cv::Mat> images1, std::vector<cv::Mat> images2, cv::Mat firstCamMatrix, cv::Mat secondCamMatrix, cv::Mat firstCamCoeffs,
	cv::Mat secondCamCoeffs, cv::Size boardSize, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F, float distanceBetweenCircles)
{
	std::vector<std::vector<cv::Point2f> > centers1(images1.size());
	CameraCalibration::getCirclePositions(images1, centers1, boardSize);
	std::vector<std::vector<cv::Point2f> > centers2(images2.size());
	CameraCalibration::getCirclePositions(images2, centers2, boardSize);
	std::vector<std::vector<cv::Point3f>> worldSpaceCircleCenters(1);
	CameraCalibration::realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(images1.size(), worldSpaceCircleCenters[0]);

	stereoCalibrate(worldSpaceCircleCenters, centers1, centers2, firstCamMatrix, firstCamCoeffs, secondCamMatrix,
		secondCamCoeffs, cv::Size(1224, 1024), R, T, E, F, cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6));
	//std::cout << "after stereo" << std::endl;
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoCalibration/R", R);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoCalibration/T", T);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoCalibration/E", E);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoCalibration/F", F);
}

void CameraCalibration::getSingleCamerasCoeffs(std::vector<cv::Mat> firstCamImgs, std::vector<cv::Mat> secondCamImgs, cv::Size boardSize, float distanceBetweenCircles,
	cv::Mat& firstCamMatrix, cv::Mat& secondCamMatrix, cv::Mat& firstCamCoeffs, cv::Mat& secondCamCoeffs)
{
	CameraCalibration::singleCameraCalibration(firstCamImgs, boardSize, distanceBetweenCircles, firstCamMatrix, firstCamCoeffs);
	CameraCalibration::singleCameraCalibration(secondCamImgs, boardSize, distanceBetweenCircles, secondCamMatrix, secondCamCoeffs);
	//saving matrices to Unity Project directory
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/singleCamCalibration/firstCamMatrix", firstCamMatrix);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/singleCamCalibration/secondCamMatrix", secondCamMatrix);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/singleCamCalibration/firstCamCoeffs", firstCamCoeffs);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/singleCamCalibration/secondCamCoeffs", secondCamCoeffs);
}

void CameraCalibration::saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q)
{
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoRectifyResults/R1", R1);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoRectifyResults/R2", R2);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoRectifyResults/P1", P1);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoRectifyResults/P2", P2);
	CameraCalibration::saveMatrix("../../MonitoringCPRUnityProject/matrices/stereoRectifyResults/Q", Q);
}
