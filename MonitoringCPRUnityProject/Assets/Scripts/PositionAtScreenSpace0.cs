using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PositionAtScreenSpace0 : MonoBehaviour
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

        //Debug.Log("pozycja kulki 0: " + OpenCVMarkerLocation.NormalizedBallsPositions[0].x.ToString());
        // skaluje położenie punktu na scenie tak, żeby odpowiadała kadrowi z kamery
        transform.position = (new Vector3(OpenCVMarkerLocation.NormalizedBallsPositions[0].x * scale, 
            - OpenCVMarkerLocation.NormalizedBallsPositions[0].y * scale, OpenCVMarkerLocation.NormalizedBallsPositions[0].z * scale));
    }
}