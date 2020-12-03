#include "CameraCalibration.h"
#include<opencv2/tracker.hpp>
#include<opencv2/cuda.hpp>
#include<cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/tracking.hpp>

int main()
{
	cv::Mat e = cv::imread("../images/movement/seria2/camera1/20181123_123957_camera_1_000.png");
	cv::imshow("proba", e);
	waitKey(0);
}