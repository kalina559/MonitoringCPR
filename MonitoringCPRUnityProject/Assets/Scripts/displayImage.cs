using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;

public class displayImage : MonoBehaviour
{
    [DllImport("ExportToUnity")]
    private static extern void GetCurrentFrame(IntPtr firstFrame, IntPtr secondFrame, int width, int height);

    private Texture2D firstTex;
    private Texture2D secondTex;

    private Color32[] firstPixel32;
    private Color32[] secondPixel32;

    private GCHandle firstPixelHandle;
    private GCHandle secondPixelHandle;

    private IntPtr firstPixelPtr;
    private IntPtr secondPixelPtr;


    private bool _ready;
    void Start()
    {
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
        InitTexture();
        GameObject.Find("firstFrame").GetComponent<RawImage>().texture = firstTex;
        GameObject.Find("secondFrame").GetComponent<RawImage>().texture = secondTex;
        //gameObject.GetComponent<RawImage>().texture = tex;
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
        firstTex = new Texture2D(512, 512, TextureFormat.ARGB32, false);
        secondTex = new Texture2D(512, 512, TextureFormat.ARGB32, false);
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
        GetCurrentFrame(firstPixelPtr, secondPixelPtr, firstTex.width, firstTex.height);
        //Update the Texture2D with array updated in C++
        firstTex.SetPixels32(firstPixel32);
        secondTex.SetPixels32(secondPixel32);
        firstTex.Apply();
        secondTex.Apply();
    }

    void OnApplicationQuit()
    {
        //Free handle
        firstPixelHandle.Free();
        secondPixelHandle.Free();
        if (_ready)
        {
            OpenCVInterop.Close();
        }
    }
}
