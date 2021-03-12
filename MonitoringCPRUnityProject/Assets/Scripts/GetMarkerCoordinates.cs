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
    public GameObject gameFloorPlane;
    public TextMeshProUGUI firstElbowAngle, secondElbowAngle, armAngle, compressionsRate, compressionsCount, compressionDepth;
    Vector3 rightElbowFirstVec, rightElbowSecondVec, leftElbowFirstVec, leftElbowSecondVec;

    bool downwardMovement = false;
    int downwardMovementFrameCount = 0;
    int compressionCount = 0;
    float lastDistanceToFloor = 0;
    float trackingStartTime;

    float maxDistanceToFloor, minDistanceToFloor, lastCompressionDepth;
    bool firstMeasurement = true;

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
        setGameFloorPlane();

        Camera.main.transform.LookAt(markers[7].transform.position);

        calculateAngles();
        checkCompressionParameters();
        updateMeasurementMessages();
    }
    public override void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
            compressionCount = 0;
            trackingStartTime = Time.time;
            firstMeasurement = true;
            lastCompressionDepth = 0;
        }
        else
        {
            beginTracking = false;
        }
    }

    void calculateAngles()
    {
        rightElbowFirstVec = markers[0].transform.position - markers[2].transform.position;
        rightElbowSecondVec = markers[2].transform.position - markers[4].transform.position;        

        leftElbowFirstVec = markers[1].transform.position - markers[3].transform.position;
        leftElbowSecondVec = markers[3].transform.position - markers[4].transform.position;        

        armsPlane.Set3Points(
           markers[0].transform.position,
           markers[1].transform.position,
           markers[4].transform.position);        
    }

    void checkCompressionParameters()
    {
        var distanceToFloor = floorPlane.GetDistanceToPoint(markers[4].transform.position);
        if(firstMeasurement == true)
        {
            maxDistanceToFloor = distanceToFloor;
            minDistanceToFloor = distanceToFloor;
            lastDistanceToFloor = distanceToFloor;
            firstMeasurement = false;
        }
        else
        {
            if (distanceToFloor > maxDistanceToFloor)
            {
                maxDistanceToFloor = distanceToFloor;
            }
            if (distanceToFloor < minDistanceToFloor)
            {
                minDistanceToFloor = distanceToFloor;
            }

            if (Math.Abs(distanceToFloor - lastDistanceToFloor) > 0.005)
            {
                if (distanceToFloor < lastDistanceToFloor)
                {
                    if (downwardMovement == false)
                    {
                        ++downwardMovementFrameCount;
                        if (downwardMovementFrameCount == 2)
                        {
                            downwardMovement = true;
                            minDistanceToFloor = distanceToFloor;
                        }
                    }
                }
                else if (distanceToFloor > lastDistanceToFloor)
                {
                    downwardMovementFrameCount = 0;

                    if (downwardMovement == true)
                    {
                        downwardMovementFrameCount = 0;
                        downwardMovement = false;
                        ++compressionCount;
                        lastCompressionDepth = (maxDistanceToFloor - minDistanceToFloor) * 1000;
                        maxDistanceToFloor = distanceToFloor;
                    }
                }
            }
            lastDistanceToFloor = distanceToFloor;
        }        
    }

    void setGameFloorPlane()
    {
        floorPlane.Set3Points(
            markers[7].transform.position,
            markers[6].transform.position,
            markers[5].transform.position);

        gameFloorPlane.transform.position = new Vector3(markers[6].transform.position.x, markers[6].transform.position.y, markers[6].transform.position.z);

        if (Vector3.Dot(floorPlane.normal, Vector3.down) > 0)    //making plane's normal vector is facing up
        {
            gameFloorPlane.transform.rotation = Quaternion.FromToRotation(Vector3.up, -floorPlane.normal);
        }
        else
        {
            gameFloorPlane.transform.rotation = Quaternion.FromToRotation(Vector3.up, floorPlane.normal);
        }
    }

    void updateMeasurementMessages()
    {
        compressionsCount.SetText("Liczba uciśnięć: " + compressionCount);
        compressionsRate.SetText("Częstotliwość: " + compressionCount * 60 / (Time.time - trackingStartTime) + " uciśnięć/minutę");
        firstElbowAngle.SetText("Kąt w prawym łokciu: " + Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec));
        secondElbowAngle.SetText("Kąt w lewym łokciu: " + Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec));
        armAngle.SetText("Kąt miêdzy rękami, a podłogą: " + Vector3.Angle(floorPlane.normal, armsPlane.normal));
        compressionDepth.SetText("Głębokość ostatniego uciśnięcia: " + Math.Round(lastCompressionDepth, 1) + "mm");
    }
}



