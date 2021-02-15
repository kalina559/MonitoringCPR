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
        CaptureCalibrationFrames
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
    public void Quit()
    {
        Application.Quit();
    }
    public void SaveFrames()
    {
        OpenCVInterop.saveCurrentFrames();
    }
    void OnApplicationQuit()
    {
        OpenCVInterop.CloseSDLCameras();
        Debug.Log("Closed SDL cameras in Menu manager");
    }
}
