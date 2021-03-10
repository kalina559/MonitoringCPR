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
    Plane floorPlane;
    public GameObject plane;
    //Vector3 startingPosition;
    private void Start()
    {
        expectedNumberOfMarkerPairs = 8;
        initializeScene();
        floorPlane = new Plane();
    }
    protected override void useMarkerCoordinates()
    {


        

        

       
        
        //if (offsetSet == false)
        //{
        //    offset = new CvCoordinates();
        //    saveOffset();
        //    offsetSet = true;
        //}

        for (int i = 0; i < _balls.Length; i++)
        {
            markers[i].transform.position =
                new Vector3(_balls[i].X/* - offset.X*/,
                -_balls[i].Y /*- offset.Y*/,
               _balls[i].Z /*- offset.Z*/);
            //Vector3.ProjectOnPlane(new Vector3(_balls[i].X, -_balls[i].Y, _balls[i].Z), floorPlane.normal).magnitude - offset.Z);

        }
       
        floorPlane.Set3Points(
            new Vector3(markers[7].transform.position.x, markers[7].transform.position.y, markers[7].transform.position.z),
            new Vector3(markers[6].transform.position.x, markers[6].transform.position.y, markers[6].transform.position.z),
            new Vector3(markers[5].transform.position.x, markers[5].transform.position.y, markers[5].transform.position.z));
        plane.transform.rotation = new Quaternion(floorPlane.normal.x, floorPlane.normal.y, floorPlane.normal.z, 0);

        plane.transform.position = new Vector3(markers[5].transform.position.x, markers[5].transform.position.y, markers[5].transform.position.z);
        //plane.transform.TransformDirection(floorPlane.normal);


        //Camera.main.transform.position = new Vector3(_balls[7].X - 2f, -_balls[7].Y + 0.39f, _balls[7].Z - 0.29f);
        //var camOrientation = new Vector3(_balls[6].X, -_balls[6].Y, _balls[6].Z) - new Vector3(_balls[7].X, -_balls[7].Y, _balls[7].Z);
        //Camera.main.transform.forward = camOrientation;
    }
    public void saveOffset()
    {
        offset.X = _balls[4].X - 0.24f;
        offset.Y = -_balls[4].Y + 0.148f;
        offset.Z = _balls[4].Z + 10.3f;
        //offset.Z = Vector3.ProjectOnPlane(new Vector3(_balls[4].X, -_balls[4].Y, _balls[4].Z), floorPlane.normal).magnitude + 9.129f;
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




