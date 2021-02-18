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
        if (isValidationUpToDate())
        {           
                SceneManager.LoadScene((int)Menu.Scenes.Calibration);
        }
        else
        {
            messagePanel.gameObject.SetActive(true);
        }
    }

    public void CloseMessagePanel()
    {
        messagePanel.gameObject.SetActive(false);
    }

    bool isValidationUpToDate()
    {
        string s = OpenCVInterop.getFramesSetId();
        Debug.Log("Zapisana wartość: " + PlayerPrefs.GetString("FirstDataString") + " Obecna wartość: " + s);
        return PlayerPrefs.GetString("FirstDataString") == s;
    }
}
