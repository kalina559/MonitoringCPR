#include "CameraCalibration.h"

using namespace std;
using namespace cv;

void CameraCalibration::loadPhotos(std::string path, std::vector<cv::Mat>& images)
{
	vector<cv::String> fileNames;
	glob(path, fileNames, false);

	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		cout << "Processing photo nr: " << i << endl;
		Mat resized;
		resize(imread(fileNames[i]), resized, Size(), 0.5, 0.5);
		images.push_back(resized);
	}
}

void CameraCalibration::realLifeCirclePositions(Size boardSize, float distance, vector<Point3f>& circleCenters)
{
	for (int i = 0; i < boardSize.height; ++i)
	{
		for (int j = 0; j < boardSize.width; ++j)
		{
			circleCenters.push_back(Point3f(j * distance, i * distance, 0.0f));
		}
	}
}

void CameraCalibration::getCirclePositions(vector<Mat> images, vector<vector<Point2f>>& centers, Size arrayOfCirclesSize, bool showResults)
{
	for (size_t i = 0; i < images.size(); ++i) {
		bool patternFound = findCirclesGrid(images[i], arrayOfCirclesSize, centers[i], CALIB_CB_SYMMETRIC_GRID);
		drawChessboardCorners(images[i], arrayOfCirclesSize, Mat(centers[i]), patternFound);

		if (showResults == true)
		{
			imshow("e", images[i]);
			waitKey(0);
		}
	}
}

void CameraCalibration::cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float distanceBetweenCircles, Mat& cameraMatrix, Mat& distCoefficients)
{
	vector<vector<Point3f>> worldSpaceCircleCenters(1);
	realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(calibrationImages.size(), worldSpaceCircleCenters[0]);

	vector<Mat> rVectors, tVectors;
	distCoefficients = Mat::zeros(8, 1, CV_64F);


	vector<vector<Point2f> > centers(calibrationImages.size());

	getCirclePositions(calibrationImages, centers, boardSize);
	
	calibrateCamera(worldSpaceCircleCenters, centers, boardSize, cameraMatrix, distCoefficients, rVectors, tVectors);
}

bool CameraCalibration::saveMatrix(string name, Mat mat)
{
	ofstream outStream(name);
	if (outStream)
	{
		size_t rows = mat.rows;
		size_t columns = mat.cols;

		for (size_t i = 0; i < rows; ++i)
		{
			for (size_t j = 0; j < columns; ++j)
			{
				double value = mat.at<double>(i, j);
				outStream << value << endl;
			}
		}
		outStream.close();
		return true;
	}
	return false;
}

bool CameraCalibration::loadMatrix(string name, int cols, int rows, Mat& outMatrix)
{
	ifstream ifStream(name);
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

void CameraCalibration::stereoCalibration(string firstPath, string secondPath, Mat firstCamMatrix, Mat secondCamMatrix, Mat firstCamCoeffs,
	Mat secondCamCoeffs, Size boardSize, Mat& R, Mat& T, Mat& E, Mat& F, float distanceBetweenCircles)
{
	vector<Mat> images1;
	CameraCalibration::loadPhotos(firstPath, images1);
	vector<Mat> images2;
	CameraCalibration::loadPhotos(secondPath, images2);

	vector<vector<Point2f> > centers1(images1.size());
	CameraCalibration::getCirclePositions(images1, centers1, boardSize);
	vector<vector<Point2f> > centers2(images2.size());
	CameraCalibration::getCirclePositions(images2, centers2, boardSize);

	vector<vector<Point3f>> worldSpaceCircleCenters(1);
	CameraCalibration::realLifeCirclePositions(boardSize, distanceBetweenCircles, worldSpaceCircleCenters[0]);
	worldSpaceCircleCenters.resize(images1.size(), worldSpaceCircleCenters[0]);

	stereoCalibrate(worldSpaceCircleCenters, centers1, centers2, firstCamMatrix, firstCamCoeffs, secondCamMatrix,
		secondCamCoeffs, Size(1224, 1024), R, T, E, F, CALIB_FIX_INTRINSIC, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6));
	//std::cout << "after stereo" << std::endl;
	CameraCalibration::saveMatrix("matrices/stereoCalibration/R", R);
	CameraCalibration::saveMatrix("matrices/stereoCalibration/T", T);
	CameraCalibration::saveMatrix("matrices/stereoCalibration/E", E);
	CameraCalibration::saveMatrix("matrices/stereoCalibration/F", F);


}
