using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;
using System.Text;

public class StereoCalibration : MonoBehaviour
{
    public Image messagePanel;
    public void GoToCalibration()
    {
        //if (isValidationUpToDate())
        //{
        //    if (PlayerPrefs.GetInt("CalibrationValidate") == 1)
        //    {
        //        SceneManager.LoadScene((int)Menu.Scenes.Calibration);
        //    }
        //    else
        //    {
        //        messagePanel.gameObject.SetActive(true);
        //        Debug.Log("Nie sprawdzono zdjęć");
        //    }
        //}
        //else
        //{
        //    PlayerPrefs.SetInt("CalibrationValidate", 0);
        //    messagePanel.gameObject.SetActive(true);
        //    Debug.Log("Wprowadzono zmiany w pliku ze zdjęciami.");
        //}
        if (isValidationUpToDate())
        {
            
                SceneManager.LoadScene((int)Menu.Scenes.Calibration);

        }
        else
        {
            messagePanel.gameObject.SetActive(true);
            Debug.Log("Wprowadzono zmiany w pliku ze zdjęciami.");
        }
    }

    public void CloseMessagePanel()
    {
        messagePanel.gameObject.SetActive(false);
    }

    bool isValidationUpToDate()
    {
        //PlayerPrefs.GetString("FirstDataString", Marshal.PtrToStringAuto(text));
        //int firstLength = 0, secondLength = 0;
        //string firstDataString = " ", secondDataString = " ";
        //OpenCVInterop.getImageData(ref firstDataString, ref secondDataString, firstLength, secondLength);
        //return (firstDataString == PlayerPrefs.GetString("FirstDataString") && secondDataString == PlayerPrefs.GetString("SecondDataString"));

        string s = OpenCVInterop.getImageData();
       // OpenCVInterop.getImageData(str);
        Debug.Log("Zapisana wartość: " + PlayerPrefs.GetInt("FirstDataString") + " Obecna wartość: " + s);
        return PlayerPrefs.GetString("FirstDataString") == s;
    }
}
