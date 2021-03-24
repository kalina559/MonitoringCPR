using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
public class MarkerTracking : MonoBehaviour
{
    frameDisplay display;
    //Game Objects
    public GameObject thresholdMenu, mainMenu, changeModeButton;
    public TextMeshProUGUI threshLevelDisplay;
    public List<GameObject> markers;
    public RawImage firstFrame, secondFrame;
    //used to count fps
    protected int frameCount = 0;
    float lastTimeStamp;
    //scene logic
    protected bool allMarkersDetected = false;
    protected bool beginTracking = false;
    protected bool adjustThreshLevel = false;
    //marker tracking variables
    protected CvCoordinates[] _balls;
    int threshValue;
    protected int expectedNumberOfMarkerPairs;
    protected Int64 delay;
    protected void Update()
    {
        MatToTexture2D();
        MonitoringUtils.setStartButtonText(beginTracking, changeModeButton);
        checkFPS();
    }
    protected void InitTexture()
    {
        display.Init();
    }
    protected void MatToTexture2D()
    {
        var textures = display.getTextures();
        var pixelPtrs = display.getPixelPtrs();
        if (adjustThreshLevel == true)
        {
            updateThresholdMenu(textures, pixelPtrs);
        }
        else
        {
            if (beginTracking == false)
            {
                allMarkersDetected = OpenCVInterop.detectMarkers(pixelPtrs.Item1, pixelPtrs.Item2, 640, 480);
            }
            else
            {
                readMarkerCoordinates(textures, pixelPtrs);
                useMarkerCoordinates();
            }
        }
        display.updateTextures();
    }
    public virtual void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
        }
        else
        {
            beginTracking = false;
        }
    }
    public void saveThreshLevel()
    {
        OpenCVInterop.saveThreshLevel(threshValue);
    }
    public void openThresholdMenu()
    {
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
    protected void checkFPS()
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
    protected void readMarkerCoordinates(Tuple<Texture2D, Texture2D> textures, Tuple<IntPtr, IntPtr> pixelPtrs)
    {
        unsafe
        {
            fixed (CvCoordinates* outBalls = _balls)
            {
                OpenCVInterop.realTimeMonitoring(pixelPtrs.Item1, pixelPtrs.Item2, textures.Item1.width, textures.Item2.height, outBalls, ref beginTracking, ref delay) ;
                Debug.Log("delay between frames:" + delay);
            }
        }
    }
    protected virtual void useMarkerCoordinates()
    {
    }
    protected void updateThresholdMenu(Tuple<Texture2D, Texture2D> textures, Tuple<IntPtr, IntPtr> pixelPtrs)
    {
        threshValue = (int)thresholdMenu.GetComponentInChildren<Slider>().value;
        threshLevelDisplay.text = threshValue.ToString();
        OpenCVInterop.GetCurrentThreshFrame(pixelPtrs.Item1, pixelPtrs.Item2, textures.Item1.width, textures.Item2.height, threshValue);
    }
    protected void OnDisable()
    {
        display.freeHandles();
    }

    protected void initializeScene()
    {
        OpenCVInterop.setExpectedNumberOfMarkerPairs(expectedNumberOfMarkerPairs);
        display = new frameDisplay();
        InitTexture();
        var textures = display.getTextures();
        firstFrame.texture = textures.Item1;
        secondFrame.texture = textures.Item2;
        _balls = new CvCoordinates[expectedNumberOfMarkerPairs]; //tworzymy bufor o podanej wielkoœci
    }
}