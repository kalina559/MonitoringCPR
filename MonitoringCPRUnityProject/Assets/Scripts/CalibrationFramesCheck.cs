using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
public class CalibrationFramesCheck : MonoBehaviour
{
    [DllImport("ExportToUnity")]
    internal static extern void showInvalidFrame(IntPtr firstFrame, IntPtr secondFrame);

    private Texture2D firstTex;
    private Texture2D secondTex;

    private Color32[] firstPixel32;
    private Color32[] secondPixel32;

    private GCHandle firstPixelHandle;
    private GCHandle secondPixelHandle;

    private IntPtr firstPixelPtr;
    private IntPtr secondPixelPtr;

    int invalidFrames = 0, totalFrames = 0;
    public TextMeshProUGUI messageLabel;
    public TextMeshProUGUI pairNumberLabel;
    private int currentPairNumber = 1;
    private int validPairsCount;
    void Start()
    {
        OpenCVInterop.checkCalibrationFrames(ref invalidFrames, ref totalFrames);
        validPairsCount = totalFrames - invalidFrames;
        updateLabels();
        Debug.Log("CalibFramesCheck start");
        InitTexture();
        GameObject.Find("firstFrame").GetComponent<RawImage>().texture = firstTex;
        GameObject.Find("secondFrame").GetComponent<RawImage>().texture = secondTex;
        MatToTexture2D();
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
        OpenCVInterop.showValidFrame(firstPixelPtr, secondPixelPtr);
        //Update the Texture2D with array updated in C++
        firstTex.SetPixels32(firstPixel32);
        secondTex.SetPixels32(secondPixel32);
        firstTex.Apply();
        secondTex.Apply();
    }
    private void updateLabels()
    {
        messageLabel.SetText("Usunięto " + invalidFrames + " nieprawidłowych par zdjęć z całkowitej liczby " + totalFrames + " par.");
        pairNumberLabel.SetText(currentPairNumber + " / " + validPairsCount);
    }
    private void OnDisable()
    {
        //Free handle
        firstPixelHandle.Free();
        secondPixelHandle.Free();
        Debug.Log("Freed textures in onDisable displayImage");
    }

    public void moveToNextFrame()
    {
        if (OpenCVInterop.moveToNextFrames())
        {
            currentPairNumber++;
            updateLabels();
            MatToTexture2D();
        }
        else
        {
            PlayerPrefs.SetInt("CalibrationValidate", 1);
            SceneManager.LoadScene((int)Menu.Scenes.CalibrationMenu);
        }
    }
    public void deleteCurrentFrames()
    {
        invalidFrames++;
        validPairsCount = totalFrames - invalidFrames;
        OpenCVInterop.deleteCurrentFrames();
        if(!OpenCVInterop.moveToNextFrames())
        {
            PlayerPrefs.SetInt("CalibrationValidate", 1);
            SceneManager.LoadScene((int)Menu.Scenes.CalibrationMenu);
        }
       // updateLabels();
    }
}
