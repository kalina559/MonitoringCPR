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
#include<filesystem>
namespace CameraCalibration
{
	void loadPhotos(std::string path, std::vector<cv::Mat>& images);
	std::vector<std::string> checkFramesPairs(std::string firstPath, std::string secondPath, std::string format);
	std::set<std::string> getFileNames(std::vector<cv::String> filePaths);
    void realLifeCirclePositions(cv::Size boardSize, float distance, std::vector<cv::Point3f>& circleCenters);
	void getCirclePositions(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& centers, cv::Size arrayOfCirclesSize);
	void singleCameraCalibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float distanceBetweenCircles, cv::Mat& cameraMatrix, cv::Mat& distCoefficients);
	bool saveMatrix(std::string name, cv::Mat mat);
	bool loadMatrix(std::string name, int cols, int rows, cv::Mat& outMatrix);
	void stereoCalibration(std::vector<cv::Mat> images1, std::vector<cv::Mat> images2, cv::Mat firstCamMatrix, cv::Mat secondCamMatrix, cv::Mat firstCamCoeffs,
		cv::Mat secondCamCoeffs, cv::Size boardSize, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F, float distanceBetweenCircles);
	void getSingleCamerasCoeffs(std::vector<cv::Mat> firstCamImgs, std::vector<cv::Mat> secondCamImgs, cv::Size boardSize, float distanceBetweenCircles,
		cv::Mat& firstCamMatrix, cv::Mat& secondCamMatrix, cv::Mat& firstCamCoeffs, cv::Mat& secondCamCoeffs);	
	void saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q);
};