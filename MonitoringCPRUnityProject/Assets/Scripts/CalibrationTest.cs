using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
using System.IO;
public class CalibrationTest : MarkerTracking
{
    public GameObject expectedLengthMenu;
    public TextMeshProUGUI detectedDistance;
    public TextMeshProUGUI detectedError;
    public TMP_InputField expectedLength;
    StreamWriter writer;
    double distance = 0;
    List<Vector3> previousMarkerPositions;
    public Toggle saveCheckBox;
    void Start()
    {
        previousMarkerPositions = new List<Vector3>();
        expectedNumberOfMarkerPairs = 2;
        initializeScene();        
    }
    protected override void useMarkerCoordinates()
    {
        distance = calculateDistanceBetweenMarkers(0, 1);
        detectedDistance.SetText("Odległość: " + Math.Round(distance * 1000, 2).ToString());
        detectedError.SetText("Błąd: " + Math.Round(Math.Abs(PlayerPrefs.GetInt("expectedLength") - distance * 1000), 2).ToString());
        if (saveCheckBox.isOn)
        {
            writeToTextFile();
        }
    }

    public override void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
            previousMarkerPositions.Clear();
            if (saveCheckBox.isOn)
            {
                writer = new StreamWriter("savedData/" + DateTime.Now.ToString().Replace(':', '.') + "calibrationError.txt");
            }
        }
        else
        {
            beginTracking = false;
        }
    }
    public void openExpectedLengthMenu()
    {
        expectedLengthMenu.gameObject.SetActive(true);
        mainMenu.gameObject.SetActive(false);
    }
    public void closeExpectedLengthMenu()
    {
        expectedLengthMenu.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(true);
    }
    public void saveExpectedLength()
    {
        string expectedLengthStr = expectedLength.text;
        int expectedLengthValue;
        if (Int32.TryParse(expectedLengthStr, out expectedLengthValue))
            PlayerPrefs.SetInt("expectedLength", expectedLengthValue);
    }
    double calculateDistanceBetweenMarkers(int firstMarker, int secondMarker)
    {
        return Math.Sqrt(Math.Pow(_balls[firstMarker].X - _balls[secondMarker].X, 2) + Math.Pow(_balls[firstMarker].Y - _balls[secondMarker].Y, 2)
        + Math.Pow(_balls[firstMarker].Z - _balls[secondMarker].Z, 2));
    }

    void writeToTextFile()
    {
        float firstMarkerPositionChange = getMarkerPositionChange(0);
        float secondMarkerPositionChange = getMarkerPositionChange(1);
        string newLine = String.Format("{0};{1};{2};{3};{4};{5}", DateTime.Now.ToString("HH:mm:ss:fff"), Math.Abs(PlayerPrefs.GetInt("expectedLength") - distance * 1000), 
            distance * 1000, delay, firstMarkerPositionChange * 1000, secondMarkerPositionChange * 1000);
        writer.WriteLine(newLine);
    }

    float getMarkerPositionChange(int index)
    {
        if (index >= previousMarkerPositions.Count)
        {
            previousMarkerPositions.Add(MonitoringUtils.CvCoordinatesToVec3(_balls[index]));
            return 0;
        }
        Vector3 currentMarkerPosition = MonitoringUtils.CvCoordinatesToVec3(_balls[index]);
        float distance =  Vector3.Distance(previousMarkerPositions[index], currentMarkerPosition);
        previousMarkerPositions[index] = currentMarkerPosition;

        return distance;
    }
}
