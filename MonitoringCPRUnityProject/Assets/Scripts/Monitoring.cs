using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
using System.IO;
public class Monitoring : MarkerTracking
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
    public List<GameObject> markers;
    public GameObject desiredLocation;
    Plane floorPlane;
    public TextMeshProUGUI firstElbowAngleText, secondElbowAngleText, armAngleText, compressionsRateText, compressionsCountText, compressionDepthText;
    public GraphRendering armFloorAngleGraph, leftElbowAngleGraph, rightElbowAngleGraph, compressionDepthGraph, compressionRateGraph, handsYPositionGraph;
    Vector3 offset;
    public Toggle saveCheckBox;
    string fileName;
    public CPRDummy dummy;
    public SubjectModel subject;


    bool firstMeasurement = true;
    int compressionCount = 0;
    float leftElbowAngle, rightElbowAngle, armsFloorAngle, compressionRate, lastTimeStamp, maxCompressionDepth, lastCompressionDepth, currentChestCompression = 0,
         initialRightArmAngle = 0, initialLeftHandAngle = 0;   
    private void Start()
    {
        expectedNumberOfMarkerPairs = 10;
        initializeScene();
        initGraphs();
        floorPlane = new Plane();
    }
    protected override void useMarkerCoordinates()
    {
        floorPlane = MonitoringUtils.getPlaneFromCvCoordinates(_balls[(int)markerIds.Floor3], _balls[(int)markerIds.Floor2], _balls[(int)markerIds.Floor1]);        
        calculateAngles();
        if (firstMeasurement == true)
        {
            setInitialValues();
        }
        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position = MonitoringUtils.getCoordinatesRelativeToPlane(_balls[i], floorPlane) + offset;
        }
        dummy.updateDummyPose(
            markers[(int)markerIds.Dummy1].transform.position, 
            markers[(int)markerIds.Dummy2].transform.position, 
            markers[(int)markerIds.Hands].transform.position, ref performTracking);
        subject.updateSubjectPose(markers[(int)markerIds.rightArm].transform.position, markers[(int)markerIds.leftArm].transform.position);

        checkCompressionParameters();
        calculateCompressionRate();
        updateMeasurementMessages();
        updateGraphs();


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
            resetVariables();

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
        rightElbowAngle = Math.Abs(MonitoringUtils.calculateElbowAngle(_balls[0], _balls[2], _balls[4]) - initialRightArmAngle);
        leftElbowAngle = Math.Abs(MonitoringUtils.calculateElbowAngle(_balls[1], _balls[3], _balls[4]) - initialLeftHandAngle);
        var armsPlane = new Plane();
        armsPlane.Set3Points(
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.rightArm]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.leftArm]),
            MonitoringUtils.CvCoordinatesToVec3(_balls[(int)markerIds.Hands]));

        armsFloorAngle = Vector3.Angle(floorPlane.normal, armsPlane.normal);        
    }
    void checkCompressionParameters()
    {
        currentChestCompression = dummy.getDummyHeight() - markers[4].transform.position.y;
        if (firstMeasurement == true)
        {
            maxCompressionDepth = currentChestCompression;
            firstMeasurement = false;
        }
        else
        {
            if (currentChestCompression > maxCompressionDepth)
            {
                maxCompressionDepth = currentChestCompression;
            }            
            else if (currentChestCompression <= 0)
            {
                if(maxCompressionDepth >= 0.02)
                {
                    ++compressionCount;
                    lastCompressionDepth = maxCompressionDepth * 1000;
                    compressionDepthGraph.addValue(lastCompressionDepth);
                    maxCompressionDepth = 0;
                }
            }
        }
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
        secondElbowAngleText.SetText("Kąt w lewym łokciu: " + Math.Round(leftElbowAngle, 2));
        armAngleText.SetText("Kąt między rękami, a podłogą: " + Math.Round(armsFloorAngle, 2));
        
        compressionDepthText.SetText("Głębokość ostatniego uciśnięcia: " + Math.Round(lastCompressionDepth, 1) + "mm");
    }
    void initGraphs()
    {
        rightElbowAngleGraph.initializeGraphPoints();
        leftElbowAngleGraph.initializeGraphPoints();
        armFloorAngleGraph.initializeGraphPoints();
        compressionRateGraph.initializeGraphPoints();
        compressionDepthGraph.initializeGraphPoints();
        handsYPositionGraph.initializeGraphPoints();
    }
    void updateGraphs()
    {
        rightElbowAngleGraph.addValue(rightElbowAngle);
        leftElbowAngleGraph.addValue(leftElbowAngle);
        armFloorAngleGraph.addValue(armsFloorAngle);
        handsYPositionGraph.addValue((markers[(int)markerIds.Hands].transform.position.y - dummy.getDummyHeight()) * 1000);
    }
    void saveDataToTextFile()
    {
        string newLine = String.Format("{0};{1};{2};{3};{4};{5}", DateTime.Now.ToString("HH:mm:ss:fff"), rightElbowAngle, leftElbowAngle, armsFloorAngle, compressionRate, currentChestCompression);
        writer.WriteLine(newLine);
    }
    void setInitialAngleValues()
    {
        initialLeftHandAngle = leftElbowAngle;
        initialRightArmAngle = rightElbowAngle;
    }
    void resetVariables()
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
    void calculateCompressionRate()
    {
        if (Time.time - lastTimeStamp != 0)
        {
            compressionRate = compressionCount * 60 / (Time.time - lastTimeStamp);
        }
    }
    void setInitialValues()
    {
        offset = MonitoringUtils.setOffsetVector(desiredLocation.transform.position, floorPlane, _balls[(int)markerIds.Hands]);
        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position = MonitoringUtils.getCoordinatesRelativeToPlane(_balls[i], floorPlane) + offset;
        }
        dummy.setInitialPose(markers[(int)markerIds.Dummy1].transform.position, markers[(int)markerIds.Dummy2].transform.position, markers[(int)markerIds.Hands].transform.position);
        setInitialAngleValues();        
        subject.setSubjectInitialPose(offset, dummy.transform);
    }

}
