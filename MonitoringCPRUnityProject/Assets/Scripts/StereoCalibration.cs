using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;
using System.Text;
using TMPro;


public class StereoCalibration : MonoBehaviour
{
    public Image messagePanel;
    //public TextMeshProUGUI messageLabel;
    public void GoToCalibration()
    {
        if (OpenCVInterop.checkId())
        {
            int pairCount = 0, time = 0;
            //SceneManager.LoadScene((int)Menu.Scenes.Calibration);
            OpenCVInterop.stereoCalibrate(ref pairCount, ref time);
            messagePanel.GetComponentInChildren<TextMeshProUGUI>().SetText("Ilość par zdjęć: " + pairCount + " Czas kalibracji: " + time);
            messagePanel.gameObject.SetActive(true);
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
