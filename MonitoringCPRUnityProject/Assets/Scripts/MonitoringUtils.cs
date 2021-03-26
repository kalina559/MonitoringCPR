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

    internal static Plane getPlaneFromCvCoordinates(CvCoordinates first, CvCoordinates second, CvCoordinates third)
    {
        Plane plane = new Plane();
        plane.Set3Points(
           MonitoringUtils.CvCoordinatesToVec3(first),
            MonitoringUtils.CvCoordinatesToVec3(second),
            MonitoringUtils.CvCoordinatesToVec3(third));
        return plane;
    }
    internal static Vector3 getCoordinatesRelativeToPlane(CvCoordinates coordinates, Plane plane)
    {
        return new Vector3(Vector3.Project(Vector3.ProjectOnPlane(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z), plane.normal), Vector3.ProjectOnPlane(Vector3.right, plane.normal)).magnitude,
               Math.Abs(plane.GetDistanceToPoint(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z))),
             Vector3.Project(Vector3.ProjectOnPlane(new Vector3(coordinates.X, -coordinates.Y, coordinates.Z), plane.normal), Vector3.Cross(plane.normal, Vector3.ProjectOnPlane(Vector3.right, plane.normal))).magnitude);
    }

    internal static float calculateElbowAngle(CvCoordinates arm, CvCoordinates elbow, CvCoordinates hands)
    {
        var firstVec = MonitoringUtils.CvCoordinatesToVec3(arm) - MonitoringUtils.CvCoordinatesToVec3(elbow);
        var secondVec = MonitoringUtils.CvCoordinatesToVec3(elbow) - MonitoringUtils.CvCoordinatesToVec3(hands);
        return Math.Abs(Vector3.Angle(firstVec, secondVec));
    }

    internal static Vector3 setOffsetVector(Vector3 desiredLocation, Plane plane, CvCoordinates coordinates)
    {
        Vector3 offsetVector = desiredLocation - getCoordinatesRelativeToPlane(coordinates, plane);
        offsetVector.y = 0;
        return offsetVector;
    }
}