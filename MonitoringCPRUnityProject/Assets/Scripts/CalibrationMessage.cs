using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using TMPro;
using UnityEngine.UI;
using System;

public class CalibrationMessage : MonoBehaviour
{
    Thread thread;
    public Button calibrateButton;
    int estimatedTimeInSeconds = 0;
    int frameCount = 0, seconds = 0;
    bool done = false;
    float timer;
    int timerSeconds = 0;
    Time calibrationStart;
    // Start is called before the first frame update
    void Start()
    {
        if (OpenCVInterop.checkId())
        {
            estimatedTimeInSeconds = OpenCVInterop.getEstimatedCalibrationTime();
            gameObject.GetComponentInChildren<TextMeshProUGUI>().SetText("PRZEWIDYWANY CZAS KALIBRACJI: " + formatTimeInSeconds(estimatedTimeInSeconds));
            calibrateButton.gameObject.SetActive(true);
        }
        else
        {
            gameObject.GetComponentInChildren<TextMeshProUGUI>().SetText("WPROWADZONO ZMIANY W FOLDERACH ZE ZDJĘCIAMI");
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (thread != null)
        {
            if (thread.IsAlive)
            {
                gameObject.GetComponentInChildren<TextMeshProUGUI>().SetText("DO KOŃCA KALIBRACJI ZOSTAŁO OKOŁO : " + formatTimeInSeconds(estimatedTimeInSeconds - timerSeconds));
                timer += Time.deltaTime;
                if (timer >= 1)
                {
                    timer = 0f;
                    if (timerSeconds < estimatedTimeInSeconds)
                        timerSeconds++;
                }
            }
            else
                gameObject.GetComponentInChildren<TextMeshProUGUI>().SetText("UKOŃCZONO KALIBRACJĘ NA PODSTAWIE " + frameCount + " ZDJĘĆ W " + formatTimeInSeconds(seconds));
        }
    }

    public void OpenWindow()
    {
        gameObject.SetActive(true);
    }
    public void CloseWindow()
    {
        gameObject.SetActive(false);
    }

    public void startCalibrationThread()
    {
        thread = new Thread(stereoCalibrate);
        thread.Start();
        calibrateButton.gameObject.SetActive(false);
    }

    void stereoCalibrate()
    {
        OpenCVInterop.stereoCalibrate(ref frameCount, ref seconds, ref done);
    }

    string formatTimeInSeconds(int seconds)
    {
        TimeSpan t = TimeSpan.FromSeconds(seconds);
        string formattedTime = "";

        if (t.Hours != 0)
            formattedTime += t.Hours + " H ";
        if (t.Minutes != 0)
            formattedTime += t.Minutes + " M ";

        formattedTime += t.Seconds + " S ";

        return formattedTime;
    }
    private void OnDisable()
    {
        if (thread != null)
            thread.Abort();
    }
}
