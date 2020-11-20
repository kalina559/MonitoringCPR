#include "CameraCalibration.h"

using namespace std;
using namespace cv;

const float distanceBetweenCircles = 0.037f;
const Size arrayOfCirclesSize = Size(7, 5);

int main()
{
	vector<Mat> images1, images2;
	CameraCalibration::loadPhotos("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/kalibracja/camera1help/*.png", images1);  //TODO utworzyc lokalizacje w folderze projektu, do ktorego wrzuca sie zdjecia do kalibracji
	CameraCalibration::loadPhotos("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/kalibracja/camera2help/*.png", images2);

	Mat firstCamCoeffs, secondCamCoeffs;
	Mat firstCamMatrix = Mat::eye(3, 3, CV_64F);
	Mat secondCamMatrix = Mat::eye(3, 3, CV_64F);
	CameraCalibration::cameraCalibration(images1, arrayOfCirclesSize, distanceBetweenCircles, firstCamMatrix, firstCamCoeffs);
	CameraCalibration::cameraCalibration(images2, arrayOfCirclesSize, distanceBetweenCircles, secondCamMatrix, secondCamCoeffs);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/firstCamMatrix", firstCamMatrix);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/secondCamMatrix", secondCamMatrix);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/firstCamCoeffs", firstCamCoeffs);
	CameraCalibration::saveMatrix("matrices/singleCamCalibration/secondCamCoeffs", secondCamCoeffs);

	Mat R, T, E, F;

	CameraCalibration::stereoCalibration("C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/kalibracja/camera1help/*.png", "C:/Users/Kalin/Desktop/in퓓nierka/kalibracja/zdjecia/kalibracja/camera2help/*.png",  //TODO utworzyc lokalizacje w folderze projektu, do ktorego wrzuca sie zdjecia do kalibracji
		firstCamMatrix, secondCamMatrix, firstCamCoeffs, secondCamCoeffs, arrayOfCirclesSize, R, T, E, F, distanceBetweenCircles);

	cout << "R = " << endl << "" << R << endl << endl;
	cout << "T = " << endl << "" << T << endl << endl;
	cout << "E = " << endl << "" << E << endl << endl;
	cout << "F = " << endl << "" << F << endl << endl;

	//stereoRectify
	Mat R1, R2, P1, P2, Q;
	std::cout << "before rectify" << std::endl;
	stereoRectify(firstCamMatrix, firstCamCoeffs, secondCamMatrix, secondCamCoeffs, Size(1224, 1024), R, T, R1, R2, P1, P2, Q);
	std::cout << "after rectify" << std::endl;

	cout << "R1 = " << endl << "" << R1 << endl << endl;
	cout << "R2 = " << endl << "" << R2 << endl << endl;
	cout << "P1 = " << endl << "" << P1 << endl << endl;
	cout << "P2 = " << endl << "" << P2 << endl << endl;
	cout << "Q = " << endl << "" << Q << endl << endl;

	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/R1", R1);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/R2", R2);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/P1", P1);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/P2", P2);
	CameraCalibration::saveMatrix("matrices/stereoRectifyResults/Q", Q);
}