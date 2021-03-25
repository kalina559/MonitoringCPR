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
    public List<GameObject> markers, cylinders;
    public List<CvCoordinates> initialCoordinates;
    public GameObject head, neck, hips, dummy, desiredLocation;
    Plane floorPlane, armsPlane;
    public TextMeshProUGUI firstElbowAngleText, secondElbowAngleText, armAngleText, compressionsRateText, compressionsCountText, compressionDepthText;
    Vector3 rightElbowFirstVec, rightElbowSecondVec, leftElbowFirstVec, leftElbowSecondVec, offset;
    public Toggle saveCheckBox;
    string fileName;
    StreamWriter writer;

    //parametry:
    float leftElbowAngle, rightElbowAngle, armsFloorAngle, compressionRate;
    bool downwardMovement = false, firstMeasurement = true;
    int downwardMovementFrameCount = 0, compressionCount = 0;
    float lastDistanceToFloor = 0, lastTimeStamp, maxChestCompression, lastCompressionDepth, dummyHeight = 0, currentChestCompression = 0,
         initialHandLocalXPosition, initialHandLocalZPosition, initialRightArmAngle = 0, initialLeftHandAngle = 0;
    public GraphRendering armFloorAngleGraph, leftElbowAngleGraph, rightElbowAngleGraph, compressionDepthGraph, compressionRateGraph, handsYPositionGraph;

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
            markers[i].transform.position = MonitoringUtils.getCoordinatesRelativeToPlane(_balls[i], floorPlane) + offset;
        }
        calculateAngles();
        if (firstMeasurement == true)
        {
            setDummyInitialPose();            
            setInitialAngleValues();
            setOffset();
            setSubjectInitialPose();
            dummyHeight = markers[4].transform.position.y;

        }
        updateDummyPose();
        updateSubjectPose();
        checkCompressionParameters();
        updateMeasurementMessages();

        if (saveCheckBox.isOn)
        {
            saveDataToTextFile();
        }
    }
    public override void changeMode()
    {
        if (performTracking == false && allMarkersDetected == true)
        {
            performTracking = true;
            clearVariables();
            initGraphs();
            if (saveCheckBox.isOn)
            {
                fileName = "savedData/" + DateTime.Now.ToString().Replace(':', '.') + ".txt";
                writer = new StreamWriter(fileName);
            }
        }
        else
        {
            performTracking = false;
        }
    }
    void calculateAngles()
    {
        rightElbowFirstVec = MonitoringUtils.CvCoordinatesToVec3(_balls[0]) - MonitoringUtils.CvCoordinatesToVec3(_balls[2]);
        rightElbowSecondVec = MonitoringUtils.CvCoordinatesToVec3(_balls[2]) - MonitoringUtils.CvCoordinatesToVec3(_balls[4]);
        rightElbowAngle = Math.Abs(Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec) - initialRightArmAngle);


        //var test1 = markers[0].transform.position - markers[2].transform.position;
        //var test2 = markers[2].transform.position - markers[4].transform.position;
        //var testAngle = Math.Abs(Vector3.Angle(test1, test2) - initialRightArmAngle);
        //Debug.Log("Różnica między układami współrzędnych: " + (rightElbowAngle - testAngle));

        leftElbowFirstVec = MonitoringUtils.CvCoordinatesToVec3(_balls[1]) - MonitoringUtils.CvCoordinatesToVec3(_balls[3]);
        leftElbowSecondVec = MonitoringUtils.CvCoordinatesToVec3(_balls[3]) - MonitoringUtils.CvCoordinatesToVec3(_balls[4]);
        leftElbowAngle = Math.Abs(Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec) - initialLeftHandAngle);

        armsPlane.Set3Points(
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.rightArm]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.leftArm]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.Hands]));

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
                    if (downwardMovementFrameCount == 10)
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
           MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.Floor3]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.Floor2]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.Floor1]));
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
            performTracking = false;
        }
        dummyLength.Normalize();
        dummy.transform.right = dummyLength;
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
        hips.transform.position = dummy.transform.position + dummy.transform.forward * (dummy.transform.localScale.z / 2 + 0.3f) + offset;
        hips.transform.position = new Vector3(hips.transform.position.x, 0.6f, hips.transform.position.z);
    }
    void setInitialAngleValues()
    {
        initialLeftHandAngle = leftElbowAngle;
        initialRightArmAngle = rightElbowAngle;
    }

    void updateSubjectPose()
    {
        neck.transform.position = (markers[(int)markerIds.leftArm].transform.position - markers[(int)markerIds.rightArm].transform.position) / 2.0f + markers[(int)markerIds.rightArm].transform.position;
        neck.transform.up = neck.transform.position - hips.transform.position;
    }

    void clearVariables()
    {
        frameCount = 0;
        compressionCount = 0;
        lastTimeStamp = Time.time;
        firstMeasurement = true;
        lastCompressionDepth = 0;
        compressionRate = 0;
        initialLeftHandAngle = 0;
        initialRightArmAngle = 0;
    }

    void setOffset()
    {
        offset = desiredLocation.transform.position - new Vector3(Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[(int)markerIds.Hands].X, -_balls[(int)markerIds.Hands].Y, _balls[(int)markerIds.Hands].Z), floorPlane.normal), Vector3.right).magnitude, // sprobowac zrobic to samo co z Z, tzn project na vector3.right
                 desiredLocation.transform.position.y,
               Vector3.Project(Vector3.ProjectOnPlane(new Vector3(_balls[(int)markerIds.Hands].X, -_balls[(int)markerIds.Hands].Y, _balls[(int)markerIds.Hands].Z), floorPlane.normal), Vector3.forward).magnitude);
    }
}
