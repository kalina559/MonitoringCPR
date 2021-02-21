C++/C# computer vision project developed to monitor the parameters of chest compressions during CPR.
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
=============

Components
-------------
- Visual studio C++ project that uses OpenCV to analyse frames from 2 PS3Eye cameras. It builds into a .dll file located in Unity3D project's Assets folder. 
- Unity3D project that allows user to:
  - Take photos that can be used later for calibration. User gets instant feedback whether the calibration grid is detected on the images.
  - Manually check the validity of pairs of calibration frames and delete them if e.g. detected grid doesn't reflect reality.
  - Perform stereo calibration of the cameras. Estimated time of the calibration is calculated beforehand and user is shown how much time is left throughout the process.
  - Check the validity of the calibration by putting two markers in front of the camera. The app calculates the distance between the markers and the user can compare the values.
  - Monitor the parameters of chest compressions during CPR.

Prerequisites
-------------
- OpenCV build with BUILD_opencv_world checked
- Environmental variable OPENCV_DIR set for the OpenCV build folder

Build
-------------
- Clone the repo into a folder, and then run the get_dll_files.ps1 script. It will copy the necessary .dlls into required directories
