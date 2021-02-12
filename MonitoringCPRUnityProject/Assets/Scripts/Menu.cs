using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Menu : MonoBehaviour
{
    public void StartMonitoring()
    {
        SceneManager.LoadScene(2);
    }

    public void PrepareCameras()
    {
        // chyba nie ma sensu robić nowej sceny, po prostu wywywoła się tu funkcja z .dll
        SceneManager.LoadScene(1);
    }

    public void GoToMainMenu()
    {
        SceneManager.LoadScene(0);
    }
    public void Quit()
    {
        Application.Quit();
    }
}
