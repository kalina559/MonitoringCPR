using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
public class GetMarkerCoordinates : MonoBehaviour
{

    public static List<Vector3> NormalizedBallsPositions { get; private set; }
    public static Vector2 CameraResolution;
    private Texture2D firstTex;
    private Texture2D secondTex;
    private Color32[] firstPixel32;
    private Color32[] secondPixel32;
    private GCHandle firstPixelHandle;
    private GCHandle secondPixelHandle;
    private IntPtr firstPixelPtr;
    private IntPtr secondPixelPtr;
    private bool _ready;
    bool beginTracking = false;
    bool adjustThreshLevel = false;
    double distance = 0;
    public GameObject thresholdMenu;
    public GameObject expectedLengthMenu;
    public GameObject mainMenu;
    public TextMeshProUGUI threshLevelDisplay;
    public TextMeshProUGUI detectedDistance;
    public TextMeshProUGUI detectedError;
    public Button changeModeButton;
    int threshValue;
    public TMP_InputField expectedLength;
    int frameCount = 0;
    float lastTimeStamp;

    /// <summary>
    /// Downscale factor to speed up detection.
    /// </summary>
    private const int DetectionDownScale = 1;
    private int _maxBallsDetectCount = 5;
    private CvCoordinates[] _balls;

    public List<GameObject> markers;
    bool detectMarkers = true;
    bool allMarkersDetected = false;
    void Start()
    {
        OpenCVInterop.setExpectedNumberOfMarkerPairs(5);
        Debug.Log("displayImage start");
        InitTexture();
        GameObject.Find("firstFrame").GetComponent<RawImage>().texture = firstTex;
        GameObject.Find("secondFrame").GetComponent<RawImage>().texture = secondTex;
        _ready = true;

        int camWidth = 0, camHeight = 0;
        int result = OpenCVInterop.Init(ref camWidth, ref camHeight);
        if (result < 0)
        {
            if (result == -1)
            {
                Debug.LogWarningFormat("[{0}] Failed to find cascades definition.", GetType());
            }
            else if (result == -2)
            {
                Debug.LogWarningFormat("[{0}] Failed to open camera stream.", GetType());
            }

            return;
        }

        CameraResolution = new Vector2(camWidth, camHeight);
        _balls = new CvCoordinates[_maxBallsDetectCount]; //tworzymy bufor o podanej wielkoœci
        NormalizedBallsPositions = new List<Vector3>();
        OpenCVInterop.SetScale(DetectionDownScale);
        _ready = true;
    }

    void OnApplicationQuit()
    {
        if (_ready)
        {
            OpenCVInterop.Close();
        }
    }

    void Update()
    {

        if (!_ready)
        {
            return;
        }
        Debug.Log("starttracking " + beginTracking);
        MatToTexture2D();

        if (beginTracking == false)
        {
            changeModeButton.GetComponentInChildren<TextMeshProUGUI>().text = "ROZPOCZNIJ";
        }
        else
        {
            changeModeButton.GetComponentInChildren<TextMeshProUGUI>().text = "PRZERWIJ";
        }

        checkFPS();
    }

    void InitTexture()
    {
        firstTex = new Texture2D(640, 480, TextureFormat.ARGB32, false);
        secondTex = new Texture2D(640, 480, TextureFormat.ARGB32, false);
        firstPixel32 = firstTex.GetPixels32();
        secondPixel32 = secondTex.GetPixels32();
        //Pin pixel32 array
        firstPixelHandle = GCHandle.Alloc(firstPixel32, GCHandleType.Pinned);
        secondPixelHandle = GCHandle.Alloc(secondPixel32, GCHandleType.Pinned);
        //Get the pinned address
        firstPixelPtr = firstPixelHandle.AddrOfPinnedObject();
        secondPixelPtr = secondPixelHandle.AddrOfPinnedObject();
    }

    void MatToTexture2D()
    {
        //Convert Mat to Texture2D
        if (adjustThreshLevel == true)
        {
            threshValue = (int)thresholdMenu.GetComponentInChildren<Slider>().value;
            threshLevelDisplay.text = threshValue.ToString();
            Debug.Log("e" + threshValue);
            OpenCVInterop.GetCurrentGrayscaleFrame(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height, threshValue);
        }
        else
        {

            if (beginTracking == false)
            {
                allMarkersDetected = OpenCVInterop.detectMarkers(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height);
            }
            else
            {
                int detectedBallsCount = 0;
                unsafe
                {
                    fixed (CvCoordinates* outBalls = _balls)
                    {
                        OpenCVInterop.realTimeMonitoring(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height, outBalls, ref detectedBallsCount);
                    }
                }

                for (int i = 0; i < _balls.Length; i++)
                {
                    markers[i].transform.position = (new Vector3(_balls[i].X * 1,
                -_balls[i].Y * 1, _balls[i].Z * 1));

                }
                Debug.Log("length " + _balls.Length);

            }
        }
        firstTex.SetPixels32(firstPixel32);
        secondTex.SetPixels32(secondPixel32);
        firstTex.Apply();
        secondTex.Apply();
    }

    private void OnDisable()
    {
        //Free handle
        firstPixelHandle.Free();
        secondPixelHandle.Free();
        Debug.Log("Freed textures in onDisable displayImage");
    }
    public void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
            //changeModeButton.GetComponentInChildren<TextMeshProUGUI>().text = "PRZERWIJ";
        }
        else
        {
            beginTracking = false;
            //changeModeButton.GetComponentInChildren<TextMeshProUGUI>().text = "ROZPOCZNIJ";
        }
    }

    public void saveThreshLevel()
    {
        OpenCVInterop.saveThreshLevel(threshValue);
    }
    public void openThresholdMenu()
    {
        Debug.Log("openthresh");
        thresholdMenu.GetComponentInChildren<Slider>().value = OpenCVInterop.getThreshLevel();
        mainMenu.gameObject.SetActive(false);
        thresholdMenu.gameObject.SetActive(true);
        adjustThreshLevel = true;
    }

    public void closeThresholdMenu()
    {
        thresholdMenu.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(true);
        adjustThreshLevel = false;
    }
    void checkFPS()
    {
        ++frameCount;
        if (frameCount % 10 == 0)
        {
            float now = Time.time;
            Debug.Log("fps: " + frameCount / (now - lastTimeStamp));
            lastTimeStamp = now;
            frameCount = 0;
        }
    }
}

