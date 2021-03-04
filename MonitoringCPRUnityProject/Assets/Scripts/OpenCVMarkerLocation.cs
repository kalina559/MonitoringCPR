//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;
//using System.Runtime.InteropServices;
//using System;
//using UnityEngine.UI;

//public class OpenCVMarkerLocation : MonoBehaviour
//{

//    public static List<Vector3> NormalizedBallsPositions { get; private set; }
//    public static Vector2 CameraResolution;
//    public Image firstFrame;

//    /// <summary>
//    /// Downscale factor to speed up detection.
//    /// </summary>
//    private const int DetectionDownScale = 1;
//    private bool _ready;
//    private int _maxBallsDetectCount = 10;
//    private CvCoordinates[] _balls;

//    public GameObject firstMarker;
//    public GameObject secondMarker;
//    public List<GameObject> markers;
//    bool detectMarkers = true;
//    bool beginTracking = false;
//    void Start()
//    {
//        //firstFrame = 
//        int camWidth = 0, camHeight = 0;
//        int result = OpenCVInterop.Init(ref camWidth, ref camHeight);
//        if (result < 0)
//        {
//            if (result == -1)
//            {
//                Debug.LogWarningFormat("[{0}] Failed to find cascades definition.", GetType());
//            }
//            else if (result == -2)
//            {
//                Debug.LogWarningFormat("[{0}] Failed to open camera stream.", GetType());
//            }

//            return;
//        }

//        CameraResolution = new Vector2(camWidth, camHeight);
//        _balls = new CvCoordinates[_maxBallsDetectCount]; //tworzymy bufor o podanej wielkości
//        NormalizedBallsPositions = new List<Vector3>();
//        OpenCVInterop.SetScale(DetectionDownScale);
//        _ready = true;
//    }

//    void OnApplicationQuit()
//    {
//        if (_ready)
//        {
//            OpenCVInterop.Close();
//        }
//    }

//    void Update()
//    {
//        Debug.Log("detectMarkers " + detectMarkers);
//        if (detectMarkers == true)
//        {
//            if (OpenCVInterop.recordedDetectMarkers(640, 480))
//            {
//                detectMarkers = false;
//            }
//        }
//        else
//        {
//            if (!_ready)
//                return;

//            int detectedBallsCount = 0;
//            unsafe
//            {
//                fixed (CvCoordinates* outBalls = _balls)
//                {
//                    OpenCVInterop.recordedTrackMarkers(outBalls, _maxBallsDetectCount, ref beginTracking);
//                }
//            }
//            NormalizedBallsPositions.Clear();
//            for (int i = 0; i < markers.Count; i++)
//            {
//                NormalizedBallsPositions.Add(new Vector3((_balls[i].X * DetectionDownScale),
//                    (_balls[i].Y * DetectionDownScale), _balls[i].Z * DetectionDownScale));

//                markers[i].transform.position = (new Vector3(OpenCVMarkerLocation.NormalizedBallsPositions[i].x * 1,
//            - OpenCVMarkerLocation.NormalizedBallsPositions[i].y * 1, OpenCVMarkerLocation.NormalizedBallsPositions[i].z * 1));

//            }



//            Debug.Log("update");
//            if (Input.GetKey(KeyCode.K) == true)        // jeśli wciśniemy k to wychodzimy z apki     TODO ZMIENIC NA ESC
//            {
//                Debug.Log("wcisnieto escape");
//                NormalizedBallsPositions.Clear();
//                Application.Quit();
//            }
//        }
//    }
//}

//// Define the functions which can be called from the .dll.


//// Define the structure to be sequential and with the correct byte size (3 ints = 4 bytes * 3 = 12 bytes)
//[StructLayout(LayoutKind.Sequential, Size = 12)]
//public struct CvCoordinates
//{
//    public float X, Y, Z;
//}

