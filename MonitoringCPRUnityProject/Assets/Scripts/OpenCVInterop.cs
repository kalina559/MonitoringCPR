using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

internal static class OpenCVInterop
{
    [DllImport("MonitoringCPR")]
    internal static extern int Init(ref int outCameraWidth, ref int outCameraHeight);

    [DllImport("MonitoringCPR")]
    internal static extern int Close();

    [DllImport("MonitoringCPR")]
    internal static extern int SetScale(int downscale);

    [DllImport("MonitoringCPR")]
    internal unsafe static extern void Detect(CvCoordinates* outBalls, int maxOutBallsCount, ref int outDetectedBallsCount);

    [DllImport("MonitoringCPR")]
    internal static extern void GetCurrentFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);





    [DllImport("MonitoringCPR")]
    internal static extern void GetCurrentGrayscaleFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void GetCalibrationFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void CloseSDLCameras();
    [DllImport("MonitoringCPR")]
    internal static extern void saveCurrentFrames();
    [DllImport("MonitoringCPR")]
    internal static extern void clearCalibrationFramesFolder();

    [DllImport("MonitoringCPR")]
    internal static extern bool checkCalibrationFrames(ref int invalidFrames, ref int singleFrames, ref int totalFrames);

    [DllImport("MonitoringCPR")]
    internal static extern bool showValidFrame(IntPtr firstFrame, IntPtr secondFrame);
    [DllImport("MonitoringCPR")]
    internal static extern bool deleteCurrentFrames();
    [DllImport("MonitoringCPR")]
    internal static extern bool moveToNextFrames();
    [DllImport("MonitoringCPR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.BStr)]
    internal static extern string getFramesSetId();


    [DllImport("MonitoringCPR")]
    internal static extern void saveId();
    [DllImport("MonitoringCPR")]
    internal static extern bool checkId();
    [DllImport("MonitoringCPR")]
    internal static extern void stereoCalibrate(ref int pairCount, ref int time, ref bool isFinished);
    [DllImport("MonitoringCPR")]
    internal static extern void detectMarkers(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void trackMarkers(IntPtr firstFrame, IntPtr secondFrame, int width, int height, ref double distance, ref bool beginTracking);

    [DllImport("MonitoringCPR")]
    internal static extern int getEstimatedCalibrationTime();



}
