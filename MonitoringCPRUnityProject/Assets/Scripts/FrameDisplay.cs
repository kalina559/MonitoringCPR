using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;

public class FrameDisplay
{
    private Texture2D firstTex;
    private Texture2D secondTex;
    private Color32[] firstPixel32;
    private Color32[] secondPixel32;
    private GCHandle firstPixelHandle;
    private GCHandle secondPixelHandle;
    private IntPtr firstPixelPtr;
    private IntPtr secondPixelPtr;

    internal void Init()
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
    internal void updateTextures()
    {
        firstTex.SetPixels32(firstPixel32);
        secondTex.SetPixels32(secondPixel32);
        firstTex.Apply();
        secondTex.Apply();
    }
    internal void freeHandles()
    {
        if (firstPixelHandle.IsAllocated && secondPixelHandle.IsAllocated)
        {
            //Free handle
            firstPixelHandle.Free();
            secondPixelHandle.Free();
        }
    }
    internal Tuple<Texture2D, Texture2D> getTextures()
    {
        return new Tuple<Texture2D, Texture2D>(firstTex, secondTex);
    }
    internal Tuple<IntPtr, IntPtr> getPixelPtrs()
    {
        return new Tuple<IntPtr, IntPtr>(firstPixelPtr, secondPixelPtr);
    }
}
