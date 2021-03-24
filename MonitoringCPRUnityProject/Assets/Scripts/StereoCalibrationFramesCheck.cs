using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;
using System.Threading;
public class StereoCalibrationFramesCheck : MonoBehaviour
{
    FrameDisplay display;
    public Image errorMessagePanel;
    int invalidFrames = 0, totalFrames = 0, singleFrames = 0;
    public TextMeshProUGUI messageLabel;
    public TextMeshProUGUI pairNumberLabel;
    private int currentPairNumber = 1;
    private int validPairsCount;
    Thread thread;
    bool checkFinished = true;
    bool checkResult;
    void Start()
    {
        display = new FrameDisplay();
        errorMessagePanel.GetComponentInChildren<TextMeshProUGUI>().SetText("Trwa wstępne sprawdzanie poprawności zdjęć");
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
                GameObject.Find("firstFrame").GetComponent<RawImage>().texture = display.getTextures().Item1;
                GameObject.Find("secondFrame").GetComponent<RawImage>().texture = display.getTextures().Item2;
                MatToTexture2D();
            }
            else
            {
                errorMessagePanel.GetComponentInChildren<TextMeshProUGUI>().SetText("Nie znaleziono żadnej poprawnej pary zdjęć");
                errorMessagePanel.GetComponentInChildren<Button>(true).gameObject.SetActive(true);

            }
            checkFinished = false;
        }
    }
    void InitTexture()
    {
        display.Init();
    }

    void MatToTexture2D()
    {
        OpenCVInterop.showValidStereoFrame(display.getPixelPtrs().Item1, display.getPixelPtrs().Item2);

        display.updateTextures();
    }
    private void updateLabels()
    {
        messageLabel.SetText("Liczba znalezionych par zdjęć: " + totalFrames +"\nLiczba usuniętych par nieprawidłowych zdjęć: " + invalidFrames + "\nLiczba usuniętych zdjęć, które nie miały pary: " + singleFrames);
        pairNumberLabel.SetText(currentPairNumber + " / " + validPairsCount);
    }
    private void OnDisable()
    {
        display.freeHandles();
        if (thread != null)
            thread.Abort();
    }
    public void moveToNextFrame()
    {
        if (OpenCVInterop.moveToNextStereoFrames())
        {
            currentPairNumber++;
            updateLabels();
            MatToTexture2D();
        }
        else
        {
            PlayerPrefs.SetString("StereoValidationId", OpenCVInterop.getFramesSetId(2));
            SceneManager.LoadScene((int)Menu.Scenes.CalibrationMenu);
        }
    }
    public void deleteCurrentFrames()
    {
        invalidFrames++;
        validPairsCount = totalFrames - invalidFrames;
        OpenCVInterop.deleteCurrentStereoFrames();
        currentPairNumber--;
        moveToNextFrame();
    }
    public void startFrameCheckThread()
    {
        thread = new Thread(frameCheck);
        thread.Start();
    }
    void frameCheck()
    {
        checkResult = OpenCVInterop.checkStereoCalibrationFrames(ref invalidFrames, ref singleFrames, ref totalFrames);
    }
}
