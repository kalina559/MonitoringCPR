using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
public class GetMarkerCoordinates : MarkerTracking
{
    public List<CvCoordinates> initialCoordinates;
    CvCoordinates offset;
    bool offsetSet = false;
    //Vector3 startingPosition;
    private void Start()
    {
        expectedNumberOfMarkerPairs = 8;
        initializeScene();
        floorPlane = new Plane();
    }
    protected override void useMarkerCoordinates()
    {
        if(offsetSet == false)
        {
            offset = new CvCoordinates();
            saveOffset();
            offsetSet = true;
        }
        for (int i = 0; i < _balls.Length; i++)
        {  
            markers[i].transform.position = 
                new Vector3(_balls[i].X - offset.X,
                -_balls[i].Y - offset.Y,
                _balls[i].Z - offset.Z);
        }
    }
    public void saveOffset()
    {
        offset.X = _balls[4].X - 1f;
        offset.Y = -_balls[4].Y + 0.2f;
        offset.Z = _balls[4].Z + 10.3f;
    }
    public override void changeMode()
    {
        if (beginTracking == false && allMarkersDetected == true)
        {
            beginTracking = true;
            frameCount = 0;
            compressionCount = 0;
            trackingStartTime = Time.time; 
        }
        else
        {
            beginTracking = false;
        }
    }

    void calculateAngles()
    {
        var rightElbowFirstVec = markers[0].transform.position - markers[2].transform.position;
        var rightElbowSecondVec = markers[2].transform.position - markers[4].transform.position;
        firstElbowAngle.SetText("K�t w prawym �okciu: " + Vector3.Angle(rightElbowFirstVec, rightElbowSecondVec));

        var leftElbowFirstVec = markers[1].transform.position - markers[3].transform.position;
        var leftElbowSecondVec = markers[3].transform.position - markers[4].transform.position;
        secondElbowAngle.SetText("K�t w lewym �okciu: " + Vector3.Angle(leftElbowFirstVec, leftElbowSecondVec));

        armsPlane.Set3Points(
           markers[0].transform.position,
           markers[1].transform.position,
           markers[4].transform.position);

        armAngle.SetText("K�t mi�dzy r�kami, a pod�og�: " + Vector3.Angle(floorPlane.normal, armsPlane.normal));
    }

    void checkCompressionParameters()
    {
        if (Math.Abs(markers[4].transform.position.y - lastYCoordinate) > 0.001)
        {
            if (markers[4].transform.position.y < lastYCoordinate)
            {
                if (DownwardMovement == false)
                {
                    ++downwardMovementFrameCount;
                    if (downwardMovementFrameCount == 10)
                    {
                        DownwardMovement = true;
                    }
                }
            }
            else if (markers[4].transform.position.y > lastYCoordinate)
            {
                downwardMovementFrameCount = 0;

                if (DownwardMovement == true)
                {
                    downwardMovementFrameCount = 0;
                    DownwardMovement = false;
                    ++compressionCount;
                    Debug.Log("liczba uci�ni��: " + compressionCount);
                    compressionsCount.SetText("Liczba uci�ni��: " + compressionCount);
                    compressionsRate.SetText("Cz�stotliwo��: " + compressionCount * 60 / (Time.time - trackingStartTime) + " uci�ni��/minut�");
                }
            }
        }        
    }
}




