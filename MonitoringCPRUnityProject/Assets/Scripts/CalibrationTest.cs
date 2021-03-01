using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;

public class CalibrationTest : MonoBehaviour
{
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
    void Start()
    {

        Debug.Log("displayImage start");
        InitTexture();
        GameObject.Find("firstFrame").GetComponent<RawImage>().texture = firstTex;
        GameObject.Find("secondFrame").GetComponent<RawImage>().texture = secondTex;
        _ready = true;

    }
    void Update()
    {
        if (!_ready)
        {
            return;
        }
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
                OpenCVInterop.detectMarkers(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height);
            }
            else
            {
                OpenCVInterop.trackMarkers(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height, ref distance, ref beginTracking);
                detectedDistance.SetText("Odległość: " + Math.Round(distance * 1000, 2).ToString());
                detectedError.SetText("Błąd: " + Math.Round(Math.Abs(PlayerPrefs.GetInt("expectedLength") - distance * 1000), 2).ToString());
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
        if (beginTracking == false)
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
    public void openExpectedLengthMenu()
    {
        mainMenu.gameObject.SetActive(false);
        expectedLengthMenu.gameObject.SetActive(true);
    }
    public void closeExpectedLengthMenu()
    {
        expectedLengthMenu.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(true);
    }
    public void saveExpectedLength()
    {
        string expectedLengthStr = expectedLength.text;
        int expectedLengthValue;
        if(Int32.TryParse(expectedLengthStr, out expectedLengthValue))
        PlayerPrefs.SetInt("expectedLength", expectedLengthValue);
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
