using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
using System.Text;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using System.IO;
public class CalibrationFramesCheck : MonoBehaviour
{
    private Texture2D firstTex;
    private Texture2D secondTex;

    private Color32[] firstPixel32;
    private Color32[] secondPixel32;

    private GCHandle firstPixelHandle;
    private GCHandle secondPixelHandle;

    private IntPtr firstPixelPtr;
    private IntPtr secondPixelPtr;

    public Image errorMessagePanel;
    int invalidFrames = 0, totalFrames = 0, singleFrames = 0;
    public TextMeshProUGUI messageLabel;
    public TextMeshProUGUI pairNumberLabel;
    public TextMeshProUGUI messageText;
    private int currentPairNumber = 1;
    private int validPairsCount;
    void Start()
    {
        int result = OpenCVInterop.checkCalibrationFrames(ref invalidFrames, ref singleFrames, ref totalFrames);
        if (result == 0)
        {
            validPairsCount = totalFrames - invalidFrames;
            updateLabels();
            Debug.Log("CalibFramesCheck start");
            InitTexture();
            GameObject.Find("firstFrame").GetComponent<RawImage>().texture = firstTex;
            GameObject.Find("secondFrame").GetComponent<RawImage>().texture = secondTex;
            MatToTexture2D();
        }
        else if(result == -1)
        {
            messageText.SetText("Różne liczby zdjęć w folderach");
            errorMessagePanel.gameObject.SetActive(true);            
        }
        else
        {
            messageText.SetText("Nie znaleziono żadnej poprawnej pary zdjęć");
            errorMessagePanel.gameObject.SetActive(true);
        }
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
        messageLabel.SetText("Liczba znalezionych par zdjęć: " + totalFrames +"\nLiczba usuniętych par nieprawidłowych zdjęć: " + invalidFrames + "\nLiczba usuniętych zdjęć, które nie miały pary: " + singleFrames);
        pairNumberLabel.SetText(currentPairNumber + " / " + validPairsCount);
    }
    private void OnDisable()
    { 
        if (firstPixelHandle.IsAllocated && secondPixelHandle.IsAllocated)
        {
            //Free handle
            firstPixelHandle.Free();
            secondPixelHandle.Free();
            Debug.Log("Freed textures in onDisable displayImage");
        }
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
            OpenCVInterop.saveId();
            SceneManager.LoadScene((int)Menu.Scenes.CalibrationMenu);
        }
    }
    public void deleteCurrentFrames()
    {
        invalidFrames++;
        validPairsCount = totalFrames - invalidFrames;
        OpenCVInterop.deleteCurrentFrames();

        currentPairNumber--;
        moveToNextFrame();
    }
}
