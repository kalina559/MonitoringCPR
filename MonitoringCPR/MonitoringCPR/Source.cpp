#include "CameraCalibration.h"

const float distanceBetweenCircles = 0.037f;
const cv::Size arrayOfCirclesSize = cv::Size(7, 5);

int main()
{
	std::vector<cv::Mat> images1, images2;
	CameraCalibration::loadPhotos("../images/calibration/camera1help/*.png", images1); 
	CameraCalibration::loadPhotos("../images/calibration/camera2help/*.png", images2);

	cv::Mat firstCamCoeffs, secondCamCoeffs, firstCamMatrix, secondCamMatrix;

	CameraCalibration::getSingleCamerasCoeffs(images1, images2, arrayOfCirclesSize, distanceBetweenCircles, firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs);

	cv::Mat R, T, E, F;

	CameraCalibration::stereoCalibration(images1, images2,
		firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs, arrayOfCirclesSize, R, T, E, F, distanceBetweenCircles);

	cv::Mat R1, R2, P1, P2, Q;
	stereoRectify(firstCamMatrix, firstCamCoeffs, secondCamMatrix, secondCamCoeffs, cv::Size(1224, 1024), R, T, R1, R2, P1, P2, Q);
	CameraCalibration::saveRectifiedMatrices(R1, R2, P1, P2, Q);
}

