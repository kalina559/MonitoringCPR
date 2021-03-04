using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;
using TMPro;
public class GetMarkerCoordinates : MarkerTracking
{
    private void Start()
    {
        expectedNumberOfMarkerPairs = 5;
        initializeScene();
    }
    protected override void useMarkerCoordinates()
    {
        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position = (new Vector3(_balls[i].X * 1, -_balls[i].Y * 1, _balls[i].Z * 1));
        }
    }
}




