#pragma once
#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/videoio/videoio.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/features2d.hpp>
#include<iostream>
#include<fstream>
#include<string>

using namespace std;
using namespace cv;
namespace CameraCalibration
{
	void loadPhotos(string path, vector<Mat>& images);

    void realLifeCirclePositions(Size boardSize, float distance, vector<Point3f>& circleCenters);

	void getCirclePositions(vector<Mat> images, vector<vector<Point2f>>& centers, Size arrayOfCirclesSize);

	void singleCameraCalibration(vector<Mat> calibrationImages, Size boardSize, float distanceBetweenCircles, Mat& cameraMatrix, Mat& distCoefficients);

	bool saveMatrix(string name, Mat mat);

	bool loadMatrix(string name, int cols, int rows, Mat& outMatrix);

	void stereoCalibration(vector<Mat> images1, vector<Mat> images2, Mat firstCamMatrix, Mat secondCamMatrix, Mat firstCamCoeffs,
		Mat secondCamCoeffs, Size boardSize, Mat& R, Mat& T, Mat& E, Mat& F, float distanceBetweenCircles);

	void getSingleCamerasCoeffs(vector<Mat> firstCamImgs, vector<Mat> secondCamImgs, Size boardSize, float distanceBetweenCircles,
		Mat& firstCamMatrix, Mat& secondCamMatrix, Mat& firstCamCoeffs, Mat& secondCamCoeffs);
	
	void saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q);
};