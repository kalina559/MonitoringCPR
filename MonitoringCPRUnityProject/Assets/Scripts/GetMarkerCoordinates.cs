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
    public GameObject head, neck, hips;
    Plane floorPlane;
    Plane armsPlane;
    Plane armsPlane1;
    public GameObject gameFloorPlane;
    public TextMeshProUGUI firstElbowAngle, secondElbowAngle, armAngle, compressionsRate, compressionsCount, compressionDepth;
    Vector3 rightElbowFirstVec, rightElbowSecondVec, leftElbowFirstVec, leftElbowSecondVec;

    bool downwardMovement = false;
    int downwardMovementFrameCount = 0;
    int compressionCount = 0;
    float lastDistanceToFloor = 0;
    float lastTimeStamp;

    float /*maxDistanceToFloor,*/ maxChestCompression, lastCompressionDepth;
    bool firstMeasurement = true;

    float dummyHeight = 0;

    public GraphRendering armFloorAngleGraph, leftElbowAngleGraph, rightElbowAngleGraph, compressionDepthGraph, compressionRateGraph, handsYPosition;
    private void Start()
    {
        expectedNumberOfMarkerPairs = 8;
        initializeScene();
        floorPlane = new Plane();
    }
    protected override void useMarkerCoordinates()
    {
        getGameFloorPlane();
        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position =
                new Vector3(Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[i].X, -_balls[i].Y, _balls[i].Z), floorPlane.normal), Vector3.right).magnitude, // sprobowac zrobic to samo co z Z, tzn project na vector3.right
                 Math.Abs(floorPlane.GetDistanceToPoint(new Vector3(_balls[i].X, -_balls[i].Y, _balls[i].Z))),
               Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[i].X, -_balls[i].Y, _balls[i].Z), floorPlane.normal), Vector3.forward).magnitude);
            //markers[i].transform.position =
            //    new Vector3(_balls[i].X, // sprobowac zrobic to samo co z Z, tzn project na vector3.right
            //     -_balls[i].Y,
            //   _balls[i].Z);

        }

        neck.transform.position = (markers[1].transform.position - markers[0].transform.position) / 2.0f + markers[0].transform.position;
        if (firstMeasurement == true)
        {
            Vector3 hipsVector = neck.transform.position - markers[4].transform.position;
            hips.transform.position = new Vector3(markers[4].transform.position.x + hipsVector.x * 2, 0.2f, markers[4].transform.position.z + hipsVector.z * 2);
            dummyHeight = markers[4].transform.position.y;

        }
        Vector3 spineVector = neck.transform.position - hips.transform.position;
        neck.transform.up = spineVector;

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
            lastTimeStamp = Time.time;
            firstMeasurement = true;
            lastCompressionDepth = 0;
            initGraphs();            
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
        armsPlane1.Set3Points(
            new Vector3(_balls[0].X, -_balls[0].Y, _balls[0].Z),
            new Vector3(_balls[1].X, -_balls[1].Y, _balls[1].Z),
            new Vector3(_balls[4].X, -_balls[4].Y, _balls[4].Z));
    }

    void checkCompressionParameters()
    {
        var chestCompression = dummyHeight - markers[4].transform.position.y;
        if (firstMeasurement == true)
        {
            //maxDistanceToFloor = distanceToFloor;
            maxChestCompression = chestCompression;
            lastDistanceToFloor = chestCompression;
            downwardMovement = false;
            firstMeasurement = false;
        }
        else
        {
            //if (distanceToFloor > maxDistanceToFloor)
            //{
            //    maxDistanceToFloor = distanceToFloor;
            //}
            if (chestCompression > maxChestCompression)
            {
                maxChestCompression = chestCompression;
            }

            if (Math.Abs(chestCompression - lastDistanceToFloor) > 0.001)
            {
                if (chestCompression > lastDistanceToFloor)
                {
                    if (downwardMovement == false)
                    {
                        ++downwardMovementFrameCount;
                        if (downwardMovementFrameCount == 5)
                        {
                            downwardMovement = true;
                            maxChestCompression = chestCompression;
                        }
                    }
                }
                else if (chestCompression < lastDistanceToFloor)
                {
                    downwardMovementFrameCount = 0;

                    if (downwardMovement == true)
                    {
                        downwardMovementFrameCount = 0;
                        downwardMovement = false;
                        ++compressionCount;
                        lastCompressionDepth = (maxChestCompression) * 1000;
                        compressionDepthGraph.addValue(lastCompressionDepth);
                        maxChestCompression = 0;
                        //maxDistanceToFloor = distanceToFloor;
                    }
                }
            }
            lastDistanceToFloor = chestCompression;
        }
    }

    void getGameFloorPlane()
    {
        floorPlane.Set3Points(
           new Vector3(_balls[7].X, -_balls[7].Y, _balls[7].Z),
            new Vector3(_balls[6].X, -_balls[6].Y, _balls[6].Z),
            new Vector3(_balls[5].X, -_balls[5].Y, _balls[5].Z));
    }

    void updateMeasurementMessages()
    {
        compressionsCount.SetText("Liczba uciśnięć: " + compressionCount);
        compressionsRate.SetText("Częstotliwość ostatnich 5 uciśnięć: " + Math.Round(compressionCount * 60 / (Time.time - lastTimeStamp), 2) + " uciśnięć/minutę");
        if(compressionCount > 0 && compressionCount % 5 == 0)
        {
            compressionRateGraph.addValue(compressionCount * 60 / (Time.time - lastTimeStamp));
            lastTimeStamp = Time.time;
            compressionCount = 1;
        }
        
        firstElbowAngle.SetText("Kąt w prawym łokciu: " + Math.Round(Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec), 2));
        rightElbowAngleGraph.addValue(Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec));
        secondElbowAngle.SetText("Kąt w lewym łokciu: " + Math.Round(Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec), 2));
        leftElbowAngleGraph.addValue(Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec));
        armAngle.SetText("Kąt między rękami, a podłogą: " + Math.Round(Vector3.Angle(floorPlane.normal, armsPlane1.normal), 2));
        armFloorAngleGraph.addValue((Vector3.Angle(floorPlane.normal, armsPlane1.normal)));
        Debug.Log("armsPlane : " + Vector3.Angle(floorPlane.normal, armsPlane.normal));
        compressionDepth.SetText("Głębokość ostatniego uciśnięcia: " + Math.Round(lastCompressionDepth, 1) + "mm");

        handsYPosition.addValue((markers[4].transform.position.y - dummyHeight) * 1000);
    }

    void initGraphs()
    {
        rightElbowAngleGraph.initClearValues();
        leftElbowAngleGraph.initClearValues();
        armFloorAngleGraph.initClearValues();
        compressionRateGraph.initClearValues();
        compressionDepthGraph.initClearValues();
        handsYPosition.initClearValues();
    }
}



