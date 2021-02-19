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
        if (OpenCVInterop.checkId())
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
}
