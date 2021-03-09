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
        expectedNumberOfMarkerPairs = 5;
        //startingPosition = new Vector3(0.23f, 0.8f, -9.28f);
        initializeScene();
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
        }
        else
        {
            beginTracking = false;
            offsetSet = false;
        }
    }
}




