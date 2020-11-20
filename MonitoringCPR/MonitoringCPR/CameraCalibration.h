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
class CameraCalibration
{
public:
	static void loadPhotos(string, vector<Mat>&);

	static void realLifeCirclePositions(Size, float, vector<Point3f>&);

	static void getCirclePositions(vector<Mat>, vector<vector<Point2f>>&, Size, bool = false);

	static void cameraCalibration(vector<Mat>, Size, float, Mat&, Mat&);

	static bool saveMatrix(string, Mat);

	static bool loadMatrix(string, int, int, Mat&);

	static void stereoCalibration(string, string, Mat, Mat, Mat, Mat, Size, Mat&, Mat&, Mat&, Mat&, float);

};