﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PositionAtScreenSpace3 : MonoBehaviour
{
    private float _camDistance;
    float scale;

    void Start()
    {
        _camDistance = Vector3.Distance(Camera.main.transform.position, transform.position);
        scale = 4;
    }

    void Update()
    {
        if (OpenCVMarkerLocation.NormalizedBallsPositions.Count == 0)
            return;

        // skaluje położenie punktu na scenie tak, żeby odpowiadała kadrowi z kamery
        transform.position = (new Vector3(OpenCVMarkerLocation.NormalizedBallsPositions[3].x * scale,
            -OpenCVMarkerLocation.NormalizedBallsPositions[3].y * scale, OpenCVMarkerLocation.NormalizedBallsPositions[3].z * scale));
    }
}