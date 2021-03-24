using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.Runtime.InteropServices;
using UnityEngine.UI;

public class Init : MonoBehaviour
{
    
    public Image errorMessagePanel;

    public static bool camerasInitialized = false;
    // Start is called before the first frame update
    void Start()
    {
        initializeCameras();
        camerasInitialized = true;
    }

    public void initializeCameras()
    {
        errorMessagePanel.gameObject.SetActive(false);
        int camWidth = 640, camHeight = 480;
        Debug.Log("init SDL");
        int result = OpenCVInterop.InitSDLCameras(ref camWidth, ref camHeight);  
        if (result < 0)
        {
            errorMessagePanel.gameObject.SetActive(true);
            if (result == -1)
            {
                Debug.LogWarningFormat("Failed to detect cameras", GetType());
            }
            else if (result == -2)
            {
                Debug.LogWarningFormat("Failed to initialize cameras", GetType());
            }
        }
        else
        {
            SceneManager.LoadScene((int)Menu.Scenes.MainMenu);
        }
    }
}
