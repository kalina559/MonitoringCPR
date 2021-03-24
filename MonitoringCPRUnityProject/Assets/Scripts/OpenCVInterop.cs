using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

internal static class OpenCVInterop
{
   //potwierdzanie poprawności zdjęć
    [DllImport("MonitoringCPR")]
    internal static extern void GetCalibrationFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern bool checkStereoCalibrationFrames(ref int invalidFrames, ref int singleFrames, ref int totalFrames);
    [DllImport("MonitoringCPR")]
    internal static extern bool checkSingleCameraCalibrationFrames(ref int invalidFrames, ref int totalFrames, int cameraId);
    [DllImport("MonitoringCPR")]
    internal static extern void showValidStereoFrame(IntPtr firstFrame, IntPtr secondFrame);
    [DllImport("MonitoringCPR")]
    internal static extern void showValidSingleFrame(IntPtr frame);
    [DllImport("MonitoringCPR")]
    internal static extern bool deleteCurrentStereoFrames();
    [DllImport("MonitoringCPR")]
    internal static extern bool deleteCurrentSingleCameraFrame();
    [DllImport("MonitoringCPR")]
    internal static extern bool moveToNextStereoFrames();
    [DllImport("MonitoringCPR")]
    internal static extern bool moveToNextSingleCameraFrame();



    // wykonywanie zdjęć do kalibracji
    [DllImport("MonitoringCPR")]
    internal static extern void saveCurrentFrames(int captureMode);
    [DllImport("MonitoringCPR")]
    internal static extern void clearStereoCalibrationFramesFolder();
    [DllImport("MonitoringCPR")]
    internal static extern void clearSingleCameraFramesFolder(int cameraId);


    //generowanie łańcucha znaków reprezentującego pliki znajdujące się w folderach
    //[DllImport("MonitoringCPR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    //[return: MarshalAs(UnmanagedType.BStr)]    
    //internal static extern string getStereoFramesSetId();
    [DllImport("MonitoringCPR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.BStr)]
    internal static extern string getFramesSetId(int cameraId);




    //kalibracja
    [DllImport("MonitoringCPR")]
    internal static extern void stereoCalibrate(ref int pairCount, ref int time, ref bool isFinished);   
    [DllImport("MonitoringCPR")]
    internal static extern int getEstimatedCalibrationTime();        
    

    //real time monitoring
    [DllImport("MonitoringCPR")]
    internal static extern int setExpectedNumberOfMarkerPairs(int number);
    [DllImport("MonitoringCPR")]
    internal unsafe static extern void realTimeMonitoring(IntPtr firstFrame, IntPtr secondFrame, int width, int height, CvCoordinates* outBalls, ref bool beginTracking, ref Int64 delay);
    [DllImport("MonitoringCPR")]
    internal unsafe static extern void realTimeMonitoring(IntPtr firstFrame, IntPtr secondFrame, int width, int height, CvCoordinates* outBalls, ref bool beginTracking);
    [DllImport("MonitoringCPR")]
    internal static extern void GetCurrentGrayscaleFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height, int threshLevel);
    [DllImport("MonitoringCPR")]
    internal static extern bool detectMarkers(IntPtr firstFrame, IntPtr secondFrame, int width, int height);
    [DllImport("MonitoringCPR")]
    internal static extern void saveThreshLevel(int threshLevel);
    [DllImport("MonitoringCPR")]
    internal static extern int getThreshLevel();



    //obsługa kamer
    [DllImport("MonitoringCPR", EntryPoint = "InitSDLCameras")]
    internal static extern int InitSDLCameras(ref int outCameraWidth, ref int outCameraHeight);
    [DllImport("MonitoringCPR")]
    internal static extern void CloseSDLCameras();
}
