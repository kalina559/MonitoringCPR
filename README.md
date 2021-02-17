C++/C# computer vision project developed to monitor the parameters of chest compressions during CPR.
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
=============

Components
-------------
- Visual studio C++ solution consisting of 2 projects: 
  - CameraCalibration allowing user to calibrate single cameras, and perform stereo calibration and rectification. It builds into .exe file.
  - ExportToUnity containing functions allowing to pass marker coordinates to Unity's C# scripts. Builds into .dll file located in Unity3D project's Assets folder.
- Unity3D project

Prerequisites
-------------
- OpenCV build with BUILD_opencv_world checked
- Environmental variable OPENCV_DIR set for the OpenCV build folder

Build
-------------
- Clone the repo into a folder, and then run the get_dll_files.ps1 script. It will copy the necessary .dlls into required directories
