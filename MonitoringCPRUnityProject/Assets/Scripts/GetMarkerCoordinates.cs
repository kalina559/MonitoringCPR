using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
public class GetMarkerCoordinates : MarkerTracking
{
    public List<CvCoordinates> initialCoordinates;
    public List<GameObject> cylinders;
    Plane floorPlane;
    Plane armsPlane;
    public GameObject plane;
    public TextMeshProUGUI firstElbowAngle;
    public TextMeshProUGUI secondElbowAngle;
    public TextMeshProUGUI armAngle;
    public TextMeshProUGUI compressionsRate;
    public TextMeshProUGUI compressionsCount;

    bool DownwardMovement = false;
    int downwardMovementFrameCount = 0;
    bool firstMeasurement = true;
    int compressionCount = 0;
    float lastYCoordinate = 0;
    float trackingStartTime;

    private void Start()
    {
        expectedNumberOfMarkerPairs = 8;
        initializeScene();
        floorPlane = new Plane();
    }
    protected override void useMarkerCoordinates()
    {
        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position =
                new Vector3(_balls[i].X,
                -_balls[i].Y,
               _balls[i].Z);
        }




        floorPlane.Set3Points(
            markers[7].transform.position,
            markers[6].transform.position,
            markers[5].transform.position);

        plane.transform.position = new Vector3(markers[6].transform.position.x, markers[6].transform.position.y, markers[6].transform.position.z);
        if (Vector3.Dot(floorPlane.normal, Vector3.down) > 0)
        {
            plane.transform.rotation = Quaternion.FromToRotation(Vector3.up, -floorPlane.normal);
        }
        else
        {
            plane.transform.rotation = Quaternion.FromToRotation(Vector3.up, floorPlane.normal);
        }
        Camera.main.transform.LookAt(markers[7].transform.position);

        calculateAngles();

        if (firstMeasurement == true)
        {
            firstMeasurement = false;
        }
        else
        {
            checkCompressionParameters();
            lastYCoordinate = markers[4].transform.position.y;
        }
    }
    public override void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
            compressionCount = 0;
            trackingStartTime = Time.time;
        }
        else
        {
            beginTracking = false;
        }
    }

    void calculateAngles()
    {
        var rightElbowFirstVec = markers[0].transform.position - markers[2].transform.position;
        var rightElbowSecondVec = markers[2].transform.position - markers[4].transform.position;
        firstElbowAngle.SetText("Kąt w prawym łokciu: " + Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec));

        var leftElbowFirstVec = markers[1].transform.position - markers[3].transform.position;
        var leftElbowSecondVec = markers[3].transform.position - markers[4].transform.position;
        secondElbowAngle.SetText("Kąt w lewym łokciu: " + Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec));

        armsPlane.Set3Points(
           markers[0].transform.position,
           markers[1].transform.position,
           markers[4].transform.position);

        armAngle.SetText("Kąt miêdzy rêkami, a podłogą: " + Vector3.Angle(floorPlane.normal, armsPlane.normal));
    }

    void checkCompressionParameters()
    {
        if (Math.Abs(markers[4].transform.position.y - lastYCoordinate) > 0.001)
        {
            if (markers[4].transform.position.y < lastYCoordinate)
            {
                if (DownwardMovement == false)
                {
                    ++downwardMovementFrameCount;
                    if (downwardMovementFrameCount == 2)
                    {
                        DownwardMovement = true;
                    }
                }
            }
            else if (markers[4].transform.position.y > lastYCoordinate)
            {
                downwardMovementFrameCount = 0;

                if (DownwardMovement == true)
                {
                    downwardMovementFrameCount = 0;
                    DownwardMovement = false;
                    ++compressionCount;
                    Debug.Log("Liczba uciśnięć: " + compressionCount);
                    compressionsCount.SetText("Liczba uciśnięć: " + compressionCount);
                    compressionsRate.SetText("Częstotliwość: " + compressionCount * 60 / (Time.time - trackingStartTime) + " uciśnięć/minutę");
                }
            }
        }
    }
}



