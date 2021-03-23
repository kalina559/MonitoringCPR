using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
using System.Threading;
public class SingleCameraCalibrationFrameCheck : MonoBehaviour
{
    public TMP_Dropdown captureMode;
    private Texture2D tex;
    private Color32[] pixel32;
    private GCHandle pixelHandle;
    private IntPtr pixelPtr;

    public Image errorMessagePanel;
    int invalidFrames = 0, totalFrames = 0;
    public TextMeshProUGUI messageLabel;
    public TextMeshProUGUI frameNumberLabel;
    private int currentPairNumber = 1;
    private int validPairsCount;
    Thread thread;
    bool checkFinished = true;
    bool checkResult;

    void Start()
    {
        errorMessagePanel.GetComponentInChildren<TextMeshProUGUI>().SetText("Trwa wst�pne sprawdzanie poprawno�ci zdj��");
        errorMessagePanel.gameObject.SetActive(true);
        errorMessagePanel.GetComponentInChildren<Button>().gameObject.SetActive(false);
        startFrameCheckThread();
    }
    private void Update()
    {
        if (thread != null && !thread.IsAlive && checkFinished)     //when frameCheck is finished
        {
            if (checkResult)
            {
                errorMessagePanel.gameObject.SetActive(false);
                validPairsCount = totalFrames - invalidFrames;
                updateLabels();
                InitTexture();
                GameObject.Find("Frame").GetComponent<RawImage>().texture = tex;
                MatToTexture2D();
            }
            else
            {
                errorMessagePanel.GetComponentInChildren<TextMeshProUGUI>().SetText("Nie znaleziono �adnej poprawnej pary zdj��");
                errorMessagePanel.GetComponentInChildren<Button>(true).gameObject.SetActive(true);
            }
            checkFinished = false;
        }
    }
    void InitTexture()
    {
        tex = new Texture2D(640, 480, TextureFormat.ARGB32, false);
        pixel32 = tex.GetPixels32();
        //Pin pixel32 array
        pixelHandle = GCHandle.Alloc(pixel32, GCHandleType.Pinned);
        //Get the pinned address
        pixelPtr = pixelHandle.AddrOfPinnedObject();
    }

    void MatToTexture2D()
    {
        OpenCVInterop.showValidSingleFrame(pixelPtr);
        //Update the Texture2D with array updated in C++
        tex.SetPixels32(pixel32);        
        tex.Apply();
    }
    private void updateLabels()
    {
        messageLabel.SetText("Liczba znalezionych par zdj��: " + totalFrames + "\nLiczba usuni�tych par nieprawid�owych zdj��: " + invalidFrames);
        frameNumberLabel.SetText(currentPairNumber + " / " + validPairsCount);
    }
    private void OnDisable()
    {
        if (pixelHandle.IsAllocated)
        {
            //Free handle
            pixelHandle.Free();
            Debug.Log("Freed textures in onDisable displayImage");
        }
        if (thread != null)
            thread.Abort();
    }
    public void moveToNextFrame()
    {
        if (OpenCVInterop.moveToNextSingleCameraFrame())
        {
            currentPairNumber++;
            updateLabels();
            MatToTexture2D();
        }
        else
        {
            PlayerPrefs.SetString("SingleCameraValidationId" + captureMode.value, OpenCVInterop.getSingleCameraFramesSetId(captureMode.value));
            SceneManager.LoadScene((int)Menu.Scenes.FrameCheckMenu);
        }
    }
    public void deleteCurrentFrames()
    {
        invalidFrames++;
        validPairsCount = totalFrames - invalidFrames;
        OpenCVInterop.deleteCurrentSingleCameraFrame();
        currentPairNumber--;
        moveToNextFrame();
    }
    public void startFrameCheckThread()
    {
        OpenCVInterop.clearSingleCameraFramesVector();
        thread = new Thread(frameCheck);
        thread.Start();
    }
    void frameCheck()
    {
        checkResult = OpenCVInterop.checkSingleCameraCalibrationFrames(ref invalidFrames, ref totalFrames, captureMode.value);
    }
}
