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
    frameDisplay display;
    public GameObject expectedLengthMenu;
    public TextMeshProUGUI detectedDistance;
    public TextMeshProUGUI detectedError;
    public TMP_InputField expectedLength;
    StreamWriter writetext;
    double distance = 0;

    void Start()
    {
        expectedNumberOfMarkerPairs = 2;
        initializeScene();
        writetext = new StreamWriter("savedData/calibrationError.txt");
    }
    protected override void useMarkerCoordinates()
    {
        distance = calculateDistanceBetweenMarkers(0, 1);
        detectedDistance.SetText("Odległość: " + Math.Round(distance * 1000, 2).ToString());
        detectedError.SetText("Błąd: " + Math.Round(Math.Abs(PlayerPrefs.GetInt("expectedLength") - distance * 1000), 2).ToString());
        string newLine = String.Format("{0};{1};{2};{3}", DateTime.Now.ToString("HH:mm:ss:fff"), Math.Abs(PlayerPrefs.GetInt("expectedLength") - distance * 1000), distance * 1000, delay);
        writetext.WriteLine(newLine);
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
}
