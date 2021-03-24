using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using System.Runtime.InteropServices;
using System;
// Define the structure to be sequential and with the correct byte size (3 ints = 4 bytes * 3 = 12 bytes)
[StructLayout(LayoutKind.Sequential, Size = 12)]
public struct CvCoordinates
{
    public float X, Y, Z;
}
public class MonitoringUtils : MonoBehaviour
{
    public static void setStartButtonText(bool state, GameObject button)
    {
        if (state == false)
        {
            button.GetComponentInChildren<TextMeshProUGUI>().text = "ROZPOCZNIJ";
        }
        else
        {
            button.GetComponentInChildren<TextMeshProUGUI>().text = "PRZERWIJ";
        }
    }
    public static void checkInitResult(int result)
    {
        if (result < 0)
        {
            if (result == -1)
            {
                Debug.LogWarningFormat("Failed to find any PS3 Eye devices.");
            }
            else if (result == -2)
            {
                Debug.LogWarningFormat("Failed to open camera stream.");
            }
            return;
        }
    }

    internal static Vector3 CvCoordinatesToVec3(CvCoordinates coordinates)
    {
        return new Vector3(coordinates.X, -coordinates.Y, coordinates.Z);
    }

    internal static Vector3 getCoordinatesRelativeToPlane(CvCoordinates coordinates, Plane plane)
    {
        return new Vector3(Vector3.Project(Vector3.ProjectOnPlane(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z), plane.normal), Vector3.right).magnitude,
                 Math.Abs(plane.GetDistanceToPoint(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z))),
               Vector3.Project(Vector3.ProjectOnPlane(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z), plane.normal), Vector3.forward).magnitude);
    }
}
//public class frameDisplay
//{
//    private Texture2D firstTex;
//    private Texture2D secondTex;
//    private Color32[] firstPixel32;
//    private Color32[] secondPixel32;
//    private GCHandle firstPixelHandle;
//    private GCHandle secondPixelHandle;
//    private IntPtr firstPixelPtr;
//    private IntPtr secondPixelPtr;

//    internal void Init()
//    {
//        firstTex = new Texture2D(640, 480, TextureFormat.ARGB32, false);
//        secondTex = new Texture2D(640, 480, TextureFormat.ARGB32, false);
//        firstPixel32 = firstTex.GetPixels32();
//        secondPixel32 = secondTex.GetPixels32();
//        //Pin pixel32 array
//        firstPixelHandle = GCHandle.Alloc(firstPixel32, GCHandleType.Pinned);
//        secondPixelHandle = GCHandle.Alloc(secondPixel32, GCHandleType.Pinned);
//        //Get the pinned address
//        firstPixelPtr = firstPixelHandle.AddrOfPinnedObject();
//        secondPixelPtr = secondPixelHandle.AddrOfPinnedObject();
//    }
//    internal void updateTextures()
//    {
//        firstTex.SetPixels32(firstPixel32);
//        secondTex.SetPixels32(secondPixel32);
//        firstTex.Apply();
//        secondTex.Apply();
//    }
//    internal void freeHandles()
//    {
//        if (firstPixelHandle.IsAllocated && secondPixelHandle.IsAllocated)
//        {
//            //Free handle
//            firstPixelHandle.Free();
//            secondPixelHandle.Free();
//        }
//    }
//    internal Tuple<Texture2D, Texture2D> getTextures()
//    {
//        return new Tuple<Texture2D, Texture2D>(firstTex, secondTex);
//    }
//    internal Tuple<IntPtr, IntPtr> getPixelPtrs()
//    {
//        return new Tuple<IntPtr, IntPtr>(firstPixelPtr, secondPixelPtr);
//    }
//}
