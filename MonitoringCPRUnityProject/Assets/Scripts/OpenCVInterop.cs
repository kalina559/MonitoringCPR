using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using System.Runtime.InteropServices;

internal static class OpenCVInterop
{
    [DllImport("ExportToUnity")]
    internal static extern int Init(ref int outCameraWidth, ref int outCameraHeight);

    [DllImport("ExportToUnity")]
    internal static extern int Close();

    [DllImport("ExportToUnity")]
    internal static extern int SetScale(int downscale);

    [DllImport("ExportToUnity")]
    internal unsafe static extern void Detect(CvCoordinates* outBalls, int maxOutBallsCount, ref int outDetectedBallsCount);

    [DllImport("ExportToUnity")]
    internal static extern void GetCurrentFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);





    [DllImport("ExportToUnity")]
    internal static extern void GetSDLCurrentFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("ExportToUnity")]
    internal static extern void CloseSDLCameras();
    [DllImport("ExportToUnity")]
    internal static extern void saveCurrentFrames();
    [DllImport("ExportToUnity")]
    internal static extern void clearCalibrationFramesFolder();

    [DllImport("ExportToUnity")]
    internal static extern void checkCalibrationFrames(ref int invalidFrames, ref int totalFrames);    

    [DllImport("ExportToUnity")]
    internal static extern bool showInvalidFrame(IntPtr firstFrame, IntPtr secondFrame);
    [DllImport("ExportToUnity")]
    internal static extern bool deleteCurrentFrames();
    [DllImport("ExportToUnity")]
    internal static extern bool moveToNextFrames();
}
