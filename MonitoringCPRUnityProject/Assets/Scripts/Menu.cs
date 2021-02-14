using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Menu : MonoBehaviour
{
    enum Scenes
    {
        MainMenu,
        CalibrationMenu,
        MarkerMovement,
        CaptureCalibrationFrames
    }
    public void GoToMarkerMovement()
    {
        var e = Scenes.MarkerMovement;
        SceneManager.LoadScene((int)Scenes.MarkerMovement);
    }

    public void GoToCalibrationMenu()
    {
        // chyba nie ma sensu robić nowej sceny, po prostu wywywoła się tu funkcja z .dll
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
}
