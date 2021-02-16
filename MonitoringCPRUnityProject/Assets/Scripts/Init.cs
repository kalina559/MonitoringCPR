using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.Runtime.InteropServices;


public class Init : MonoBehaviour
{
    [DllImport("ExportToUnity")]
    internal static extern int InitSDLCameras(ref int outCameraWidth, ref int outCameraHeight);
    // Start is called before the first frame update
    void Start()
    {
        int camWidth = 640, camHeight = 480;
        Debug.Log("init SDL");
        int result = InitSDLCameras(ref camWidth, ref camHeight);
        if (result < 0)
        {
            if (result == -1)
            {
                Debug.LogWarningFormat("Failed to detect cameras", GetType());
            }
            else if (result == -2)
            {
                Debug.LogWarningFormat("Failed to initialize cameras", GetType());
            }

            // return;
        }
        SceneManager.LoadScene((int)Menu.Scenes.MainMenu);
    }
}
