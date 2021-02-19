using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class Menu : MonoBehaviour
{
    public enum Scenes
    {
        Init,
        MainMenu,
        CalibrationMenu,
        MarkerMovement,
        CaptureCalibrationFrames,
        CalibrationFramesCheck,
        Calibration,
        CalibrationTest
    }
    public void GoToMarkerMovement()
    {
        SceneManager.LoadScene((int)Scenes.MarkerMovement);
    }

    public void GoToCalibrationMenu()
    {
        SceneManager.LoadScene((int)Scenes.CalibrationMenu);
    }

    public void GoToMainMenu()
    {
        SceneManager.LoadScene((int)Scenes.MainMenu);
    }
    public void GoToCaptureCalibrationFrames()
    {
        SceneManager.LoadScene((int)Scenes.CaptureCalibrationFrames);
    }
    public void GoToCalibrationFramesCheck()
    {
        SceneManager.LoadScene((int)Scenes.CalibrationFramesCheck);
    }
    public void GoToCalibrationTest()
    {
        SceneManager.LoadScene((int)Scenes.CalibrationTest);
    }
    public void Quit()
    {
        Application.Quit();
    }
    public void SaveFrames()
    {
        //PlayerPrefs.SetInt("CalibrationValidate", 0);
        OpenCVInterop.saveCurrentFrames();
    }
    public void clearCalibrationFramesFolder()
    {
        //PlayerPrefs.SetInt("CalibrationValidate", 0);
        OpenCVInterop.clearCalibrationFramesFolder();
    }
    void OnApplicationQuit()
    {
        if (Init.camerasInitialized)
        {
            OpenCVInterop.CloseSDLCameras();
            Debug.Log("Closed SDL cameras in Menu manager");
        }
    }
}
