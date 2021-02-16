using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

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
        Calibration
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
    public void GoToCalibration()
    {
        if (PlayerPrefs.GetInt("CalibrationValidate") == 1)
        {
            SceneManager.LoadScene((int)Scenes.Calibration);
        }
        else
        {
            Debug.Log("Nie sprawdzono zdjęć");
        }
    }
    public void Quit()
    {
        Application.Quit();
    }
    public void SaveFrames()
    {
        PlayerPrefs.SetInt("CalibrationValidate", 0);
        OpenCVInterop.saveCurrentFrames();
    }
    public void clearCalibrationFramesFolder()
    {
        OpenCVInterop.clearCalibrationFramesFolder();
    }
    void OnApplicationQuit()
    {
        OpenCVInterop.CloseSDLCameras();
        Debug.Log("Closed SDL cameras in Menu manager");
    }
}
