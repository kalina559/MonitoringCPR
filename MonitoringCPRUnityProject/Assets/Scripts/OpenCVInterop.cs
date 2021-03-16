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
    internal unsafe static extern void recordedTrackMarkers(CvCoordinates* outBalls, int maxOutBallsCount);
    [DllImport("MonitoringCPR")]
    internal unsafe static extern void realTimeMonitoring(IntPtr firstFrame, IntPtr secondFrame, int width, int height, CvCoordinates* outBalls, ref bool beginTracking, ref Int64 first, ref Int64 second);
    [DllImport("MonitoringCPR")]
    internal unsafe static extern void realTimeMonitoring(IntPtr firstFrame, IntPtr secondFrame, int width, int height, CvCoordinates* outBalls, ref bool beginTracking);
    [DllImport("MonitoringCPR")]
    internal unsafe static extern bool recordedDetectMarkers(int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void GetCurrentFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void GetCurrentGrayscaleFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height,int threshLevel);
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
    internal static extern void showValidFrame(IntPtr firstFrame, IntPtr secondFrame);
    [DllImport("MonitoringCPR")]
    internal static extern bool deleteCurrentFrames();
    [DllImport("MonitoringCPR")]
    internal static extern bool moveToNextFrames();
    [DllImport("MonitoringCPR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.BStr)]
    internal static extern string getFramesSetId();
    [DllImport("MonitoringCPR")]
    internal static extern void stereoCalibrate(ref int pairCount, ref int time, ref bool isFinished);
    [DllImport("MonitoringCPR")]
    internal static extern bool detectMarkers(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern int getEstimatedCalibrationTime();
    [DllImport("MonitoringCPR")]
    internal static extern void saveThreshLevel(int threshLevel);
    [DllImport("MonitoringCPR")]
    internal static extern void updateThreshLevel(int threshLevel);
    [DllImport("MonitoringCPR")]
    internal static extern int getThreshLevel();
    [DllImport("MonitoringCPR")]
    internal static extern int setExpectedNumberOfMarkerPairs(int number);
}
