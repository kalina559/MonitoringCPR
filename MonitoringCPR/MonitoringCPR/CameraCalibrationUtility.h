#pragma once
#include<ImgProcUtility.h>

/**
 * @brief Manages the instances of Vehicle class.
 */
namespace CameraCalibrationUtility
{
	/**
	 * @brief Loads images from a folder to vector of matrices
	 * @param path - Path to the image folder
	 * @param images - Destination vector of matrices
	 */
	void loadPhotos(std::string path, std::vector<cv::Mat>& images);
	/**
	 * @brief Returns a vector of strings with paths to images without a pair in the other camera folder
	 * @param firstPath - Path to the first image folder
	 * @param secondPath - Path to the second image folder
	 * @param format - Format of the image file
	 * @returns Vector of strings with paths to images without a pair
	 */
	std::vector<std::string> checkFramesPairs(std::string firstPath, std::string secondPath, std::string format);
	/**
	 * @brief Return a vector of file names instead of whole file paths 
	 * @param filePaths - Vector of strings with file paths
	 * @returns A vector of file names
	 */
	std::set<std::string> getFileNames(std::vector<cv::String> filePaths);
	/**
	 * @brief Generates a vector of points with elements' of circle grid real coordinates
	 * @param boardSize - cv::Size(height, width) of the calibration pattern. Asymmetric pattern is used in the system
	 * @param distance - Distance between circles in the grid
	 * @param circleCenters - Output vector of points containing the real world circle coordinates
	 */
    void realLifeCirclePositions(cv::Size boardSize, float distance, std::vector<cv::Point3f>& circleCenters);
	/**
	 * @brief Finds circle grid on each from the set of frames
	 * @param images - Vector of matrices with the analysed frames
	 * @param centers - Output vector of circle centers 
	 */
	void getCirclePositions(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& centers);
	/**
	 * @brief Calibrates a single camera
	 * @param calibrationImages - Set of frames used for calibration
	 * @param cameraMatrix - Output matrix of camera's intrinsic parameters
	 * @param distCoefficients - Output matrix of camera's distortion coefficients
	 */
	double singleCameraCalibration(std::vector<cv::Mat> calibrationImages, cv::Mat& cameraMatrix, cv::Mat& distCoefficients);
	/**
	 * @brief Saves matrix to a text file
	 * @param path - Path to the text file
	 * @param mat - Matrix that's being saved
	 */
	bool saveMatrix(std::string path, cv::Mat mat);
	/**
	 * @brief Loads matrix from a text file
	 * @param path - Path to the text folder with the matrix
	 * @param size - Size of the matrix
	 * @param outMatrix - output matrix
	 * @returns True if a matrix has been successfully loaded
	 */
	bool loadMatrix(std::string path, cv::Size size, cv::Mat& outMatrix);
	/**
	 * @brief Calibrates stereo cameras to obtain extrinsic parameters
	 * @param images1 - Vector with the first image set
	 * @param images2 - Vector with the second image set
	 * @param firstCamMatrix - First camera's intrinsic matrix
	 * @param secondCamMatrix - Second camera's intrinsic matrix
	 * @param firstCamCoeffs - first camera's distortion coefficients vector
	 * @param secondCamCoeffs - Second camera's distortion coefficients vector
	 * @param R - Rotation output matrix
	 * @param T - Translation output vector
	 * @param E  - Essential output matrix
	 * @param F - Fundamental output matrix
	 */
	double stereoCalibration(std::vector<cv::Mat> images1, std::vector<cv::Mat> images2, cv::Mat firstCamMatrix, cv::Mat secondCamMatrix, cv::Mat firstCamCoeffs,
		cv::Mat secondCamCoeffs, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F);
	/**
	 * @brief Calibrates stereo cameras to obtain extrinsic parameters
	 * @param firstCamImgs - Vector with the first image set
	 * @param secondCamImgs - Vector with the second image set
	 * @param firstCamMatrix - First camera's output intrinsic matrix
	 * @param secondCamMatrix - Second camera's output intrinsic matrix
	 * @param firstCamCoeffs - first camera's output distortion coefficients vector
	 * @param secondCamCoeffs - Second camera's output distortion coefficients vector
	 */
	void getSingleCamerasCoeffs(std::vector<cv::Mat> firstCamImgs, std::vector<cv::Mat> secondCamImgs,
		cv::Mat& firstCamMatrix, cv::Mat& secondCamMatrix, cv::Mat& firstCamCoeffs, cv::Mat& secondCamCoeffs);	
	/**
	 * @brief Saves matrices obtained during stereo rectification
	 * @param R - Rotation output matrix
	 * @param T - Translation output vector
	 * @param E  - Essential output matrix
	 * @param F - Fundamental output matrix
	 */
	void saveRectifiedMatrices(cv::Mat R1, cv::Mat R2, cv::Mat P1, cv::Mat P2, cv::Mat Q);
};