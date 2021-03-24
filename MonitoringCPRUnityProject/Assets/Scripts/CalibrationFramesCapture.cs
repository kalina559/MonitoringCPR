using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;

public class CalibrationFramesCapture    : MonoBehaviour
{
    FrameDisplay display;
    private bool _ready;
    public TMP_Dropdown captureMode;

    void Start()
    {
        display = new FrameDisplay();
        InitTexture();
        GameObject.Find("firstFrame").GetComponent<RawImage>().texture = display.getTextures().Item1;
        GameObject.Find("secondFrame").GetComponent<RawImage>().texture = display.getTextures().Item2;
        _ready = true;
    }
    void Update()
    {
        if (!_ready)
        {
            return;
        }
        MatToTexture2D();
    }
    void InitTexture()
    {
        display.Init();
    }

    void MatToTexture2D()
    {
        OpenCVInterop.GetCalibrationFrame(display.getPixelPtrs().Item1, display.getPixelPtrs().Item2, display.getTextures().Item1.width, display.getTextures().Item2.height);
        display.updateTextures();
    }
    private void OnDisable()
    {
        display.freeHandles();
        Debug.Log("Freed textures in onDisable displayImage");
    }

    public void SaveFrames()
    {
        OpenCVInterop.saveCurrentFrames(captureMode.value);
    }

    public void clearCalibrationFramesFolder()
    {
        if (captureMode.value == 0)
        {
            OpenCVInterop.clearSingleCameraFramesFolder(0);
        }
        else if (captureMode.value == 1)
        {
            OpenCVInterop.clearSingleCameraFramesFolder(1);
        }
        else if (captureMode.value == 2)
        {
            OpenCVInterop.clearStereoCalibrationFramesFolder();
        }
    }
}
