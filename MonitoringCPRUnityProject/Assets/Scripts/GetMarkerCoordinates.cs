using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
using System.IO;
public class GetMarkerCoordinates : MarkerTracking
{
    enum markerIds
    {
        rightArm,
        leftArm,
        rightElbow,
        leftElbow,
        Hands,
        Floor1,
        Floor2,
        Dummy1,
        Dummy2,
        Floor3,
    }
    public List<CvCoordinates> initialCoordinates;
    public List<GameObject> cylinders;
    public GameObject head, neck, hips;
    Plane floorPlane;
    Plane armsPlane;
    public TextMeshProUGUI firstElbowAngleText, secondElbowAngleText, armAngleText, compressionsRateText, compressionsCountText, compressionDepthText;
    Vector3 rightElbowFirstVec, rightElbowSecondVec, leftElbowFirstVec, leftElbowSecondVec;
    public Toggle saveCheckBox;
    string fileName;
    StreamWriter writer;

    //parametry:
    float leftElbowAngle, rightElbowAngle, armsFloorAngle, compressionRate;
    bool downwardMovement = false;
    int downwardMovementFrameCount = 0;
    int compressionCount = 0;
    float lastDistanceToFloor = 0;
    float lastTimeStamp;
    float maxChestCompression, lastCompressionDepth;
    bool firstMeasurement = true;
    float dummyHeight = 0;
    float currentChestCompression = 0;

    public GraphRendering armFloorAngleGraph, leftElbowAngleGraph, rightElbowAngleGraph, compressionDepthGraph, compressionRateGraph, handsYPositionGraph;
    public GameObject dummy;
    float initialHandLocalXPosition, initialHandLocalZPosition;
    public GameObject desiredLocation;

    float initialRightArmAngle = 0, initialLeftHandAngle = 0;


    Vector3 offset;
    private void Start()
    {
        expectedNumberOfMarkerPairs = 10;
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
               Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[i].X, -_balls[i].Y, _balls[i].Z), floorPlane.normal), Vector3.forward).magnitude) + offset;
        }

        neck.transform.position = (markers[1].transform.position - markers[0].transform.position) / 2.0f + markers[0].transform.position;

        calculateAngles();
        if (firstMeasurement == true)
        {
            setDummyInitialPose();
            setSubjectInitialPose();
            setInitialAngleValues();
            dummyHeight = markers[4].transform.position.y;

        }
        updateDummyPose();

        Vector3 spineVector = neck.transform.position - hips.transform.position;
        neck.transform.up = spineVector;

        
        checkCompressionParameters();
        updateMeasurementMessages();
        if (saveCheckBox.isOn)
        {
            saveDataToTextFile();
        }
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
            compressionRate = 0;
            initialLeftHandAngle = 0;
            initialRightArmAngle = 0;
            initGraphs();
            if (saveCheckBox.isOn)
            {
                fileName = "savedData/" + DateTime.Now.ToString().Replace(':', '.') + ".txt";
                writer = new StreamWriter(fileName);
            }
        }
        else
        {
            beginTracking = false;
        }
    }
    void calculateAngles()
    {
        rightElbowFirstVec = markers[(int)markerIds.rightArm].transform.position - markers[(int)markerIds.rightElbow].transform.position;
        rightElbowSecondVec = markers[(int)markerIds.rightElbow].transform.position - markers[(int)markerIds.Hands].transform.position;
        rightElbowAngle = Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec) - initialRightArmAngle;

        leftElbowFirstVec = markers[(int)markerIds.leftArm].transform.position - markers[(int)markerIds.leftElbow].transform.position;
        leftElbowSecondVec = markers[(int)markerIds.leftElbow].transform.position - markers[(int)markerIds.Hands].transform.position;
        leftElbowAngle = Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec) - initialLeftHandAngle;

        armsPlane.Set3Points(
            new Vector3(_balls[(int)markerIds.rightArm].X, -_balls[(int)markerIds.rightArm].Y, _balls[(int)markerIds.rightArm].Z),
            new Vector3(_balls[(int)markerIds.leftArm].X, -_balls[(int)markerIds.leftArm].Y, _balls[(int)markerIds.leftArm].Z),
            new Vector3(_balls[(int)markerIds.Hands].X, -_balls[(int)markerIds.Hands].Y, _balls[(int)markerIds.Hands].Z));

        armsFloorAngle = Vector3.Angle(floorPlane.normal, armsPlane.normal);
        if (Time.time - lastTimeStamp != 0)
        {
            compressionRate = compressionCount * 60 / (Time.time - lastTimeStamp);
        }
    }
    void checkCompressionParameters()
    {        
        currentChestCompression = dummyHeight - markers[4].transform.position.y;
        if (firstMeasurement == true)
        {
            maxChestCompression = currentChestCompression;
            lastDistanceToFloor = currentChestCompression;
            downwardMovement = false;
            firstMeasurement = false;
        }
        else
        {
            if (currentChestCompression > maxChestCompression)
            {
                maxChestCompression = currentChestCompression;
            }
            if (currentChestCompression > lastDistanceToFloor)
            {
                if (downwardMovement == false)
                {
                    ++downwardMovementFrameCount;
                    if (downwardMovementFrameCount == 5)
                    {
                        downwardMovement = true;
                        maxChestCompression = currentChestCompression;
                    }
                }
            }
            else if (currentChestCompression <= 0)
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
                }
            }
            lastDistanceToFloor = currentChestCompression;
        }
    }
    void getGameFloorPlane()
    {
        floorPlane.Set3Points(
           new Vector3(_balls[(int)markerIds.Floor3].X, -_balls[(int)markerIds.Floor3].Y, _balls[(int)markerIds.Floor3].Z),
            new Vector3(_balls[(int)markerIds.Floor2].X, -_balls[(int)markerIds.Floor2].Y, _balls[(int)markerIds.Floor2].Z),
            new Vector3(_balls[(int)markerIds.Floor1].X, -_balls[(int)markerIds.Floor1].Y, _balls[(int)markerIds.Floor1].Z));

        offset = desiredLocation.transform.position - new Vector3(Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[(int)markerIds.Hands].X, -_balls[(int)markerIds.Hands].Y, _balls[(int)markerIds.Hands].Z), floorPlane.normal), Vector3.right).magnitude, // sprobowac zrobic to samo co z Z, tzn project na vector3.right
                 desiredLocation.transform.position.y,
               Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[(int)markerIds.Hands].X, -_balls[(int)markerIds.Hands].Y, _balls[(int)markerIds.Hands].Z), floorPlane.normal), Vector3.forward).magnitude);
    }
    void updateMeasurementMessages()
    {
        compressionsCountText.SetText("Liczba uciśnięć: " + compressionCount);
        compressionsRateText.SetText("Częstotliwość ostatnich 5 uciśnięć: " + Math.Round(compressionRate, 2) + " uciśnięć/minutę");
        if (compressionCount > 0 && compressionCount % 5 == 0)
        {
            compressionRateGraph.addValue(compressionCount * 60 / (Time.time - lastTimeStamp));
            lastTimeStamp = Time.time;
            compressionCount = 1;
        }

        firstElbowAngleText.SetText("Kąt w prawym łokciu: " + Math.Round(rightElbowAngle, 2));
        rightElbowAngleGraph.addValue(rightElbowAngle);
        secondElbowAngleText.SetText("Kąt w lewym łokciu: " + Math.Round(leftElbowAngle, 2));
        leftElbowAngleGraph.addValue(leftElbowAngle);
        armAngleText.SetText("Kąt między rękami, a podłogą: " + Math.Round(armsFloorAngle, 2));
        armFloorAngleGraph.addValue(armsFloorAngle);
        compressionDepthText.SetText("Głębokość ostatniego uciśnięcia: " + Math.Round(lastCompressionDepth, 1) + "mm");

        handsYPositionGraph.addValue((markers[(int)markerIds.Hands].transform.position.y - dummyHeight) * 1000);
    }
    void initGraphs()
    {
        rightElbowAngleGraph.initClearValues();
        leftElbowAngleGraph.initClearValues();
        armFloorAngleGraph.initClearValues();
        compressionRateGraph.initClearValues();
        compressionDepthGraph.initClearValues();
        handsYPositionGraph.initClearValues();
    }
    void saveDataToTextFile()
    {
        string newLine = String.Format("{0};{1};{2};{3};{4};{5}", DateTime.Now.ToString("HH:mm:ss:fff"), rightElbowAngle, leftElbowAngle, armsFloorAngle, compressionRate, currentChestCompression);
        writer.WriteLine(newLine);

    }
    void setDummyInitialPose()
    {
        dummy.transform.position = new Vector3(markers[(int)markerIds.Hands].transform.position.x, markers[(int)markerIds.Hands].transform.position.y / 2, markers[(int)markerIds.Hands].transform.position.z);
        var torso = dummy.transform.Find("Torso");
        Vector3 scale = torso.localScale;      // Scale it
        scale.y = markers[(int)markerIds.Hands].transform.position.y;
        Vector3 dummyLength = Vector3.ProjectOnPlane((markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position), Vector3.up);
        scale.x = dummyLength.magnitude;
        Vector3 distanceFromHandToDummyEdge = markers[(int)markerIds.Dummy1].transform.position + (markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position) / 2.0f - markers[(int)markerIds.Hands].transform.position;
        scale.z = Vector3.ProjectOnPlane(distanceFromHandToDummyEdge, Vector3.up).magnitude * 2;
        torso.transform.localScale = scale;

        var head = dummy.transform.Find("Head");
        Vector3 headScale = head.localScale;      // Scale it
        headScale.x = scale.y;
        headScale.y = scale.y;
        headScale.z = scale.y;
        head.transform.localScale = headScale;

        initialHandLocalXPosition = (Vector3.Project((markers[(int)markerIds.Hands].transform.position - markers[(int)markerIds.Dummy1].transform.position), (markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position))).magnitude;
        initialHandLocalZPosition = Vector3.Distance(markers[(int)markerIds.Hands].transform.position, dummyLength);
    }
    void updateDummyPose()
    {
        var torso = dummy.transform.Find("Torso");
        Vector3 scale = torso.localScale;

        if (markers[4].transform.position.y < dummyHeight)
        {
            scale.y = markers[(int)markerIds.Hands].transform.position.y;
            torso.transform.localScale = scale;
            dummy.transform.position = new Vector3(dummy.transform.position.x, markers[(int)markerIds.Hands].transform.position.y / 2.0f, dummy.transform.position.z);
        }
        Vector3 dummyLength = Vector3.ProjectOnPlane((markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position), Vector3.up);
        var head = dummy.transform.Find("Head");
        float headToTorsoDistance = scale.x / 2.0f + head.transform.localScale.y / 2.0f;
        head.transform.position = new Vector3(head.transform.position.x, head.transform.localScale.y, head.transform.position.z);

        if (!handsInRightPosition(torso, dummyLength))
        {
            beginTracking = false;
        }
        dummyLength.Normalize();
        dummy.transform.right = dummyLength;
        //dummy.transform.up = Vector3.up; //zmiana
        head.position = dummy.transform.position + dummy.transform.right * headToTorsoDistance;

        var middleOfDummy = (markers[(int)markerIds.Dummy1].transform.position + (markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position) / 2.0f) + (dummy.transform.forward * (torso.transform.localScale.z / 2.0f));   //tutaj cos jest ze znakiem +/-
        dummy.transform.position = new Vector3(middleOfDummy.x, dummy.transform.position.y, middleOfDummy.z);
    }
    bool handsInRightPosition(Transform torso, Vector3 dummyXAxis)
    {
        var currentHandLocalXPosition = (Vector3.Project((markers[(int)markerIds.Hands].transform.position - markers[(int)markerIds.Dummy2].transform.position), (markers[(int)markerIds.Dummy2].transform.position - markers[(int)markerIds.Dummy1].transform.position))).magnitude;
        var currentHandLocalZPosition = Vector3.Distance(markers[(int)markerIds.Hands].transform.position, dummyXAxis);
        return (Math.Abs(currentHandLocalZPosition - initialHandLocalZPosition) < 0.5) && (Math.Abs(currentHandLocalXPosition - initialHandLocalXPosition) < 0.5);
    }
    void setSubjectInitialPose()
    {
        hips.transform.position = dummy.transform.position + dummy.transform.forward * (dummy.transform.localScale.z / 2 + 0.3f);
        hips.transform.position = new Vector3(hips.transform.position.x, 0.6f, hips.transform.position.z);
    }
    void setInitialAngleValues()
    {
        initialLeftHandAngle = leftElbowAngle;
        initialRightArmAngle = rightElbowAngle;
    }
}



