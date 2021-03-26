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
        CaptureCalibrationFrames,
        FrameCheckMenu,
        SingleCameraCalibrationFramesCheck,
        StereoCalibrationFramesCheck,
        CalibrationTest,
        Monitoring
    }
    public void GoToMonitoring()
    {
        SceneManager.LoadScene((int)Scenes.Monitoring);
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
    public void GoToFrameCheckMenu()
    {
        SceneManager.LoadScene((int)Scenes.FrameCheckMenu);
    }
    public void GoToSingleCameraCalibrationFramesCheck()
    {
        SceneManager.LoadScene((int)Scenes.SingleCameraCalibrationFramesCheck);
    }
    public void GoToStereoCalibrationFramesCheck()
    {
        SceneManager.LoadScene((int)Scenes.StereoCalibrationFramesCheck);
    }
    public void GoToCalibrationTest()
    {
        SceneManager.LoadScene((int)Scenes.CalibrationTest);
    }
    public void Quit()
    {
        Application.Quit();
    }
    void OnApplicationQuit()
    {
        if (Init.camerasInitialized)
        {
            OpenCVInterop.CloseSDLCameras();
        }
    }
}
