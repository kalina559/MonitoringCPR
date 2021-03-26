using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
public class CPRDummy : MonoBehaviour
{
    Transform torso, head;
    float height;
    float initialHandLocalZPosition, initialHandLocalXPosition;
    void Start()
    {
        torso = transform.Find("Torso");
        head = transform.Find("Head");
    }
    public void updateDummyPose(Vector3 firstDummyMarker, Vector3 secondDummyMarker, Vector3 handsMarker, ref bool performTracking)
    {
        Vector3 scale = torso.localScale;

        if (handsMarker.y < height)
        {
            scale.y = handsMarker.y;
            torso.transform.localScale = scale;
            transform.position = new Vector3(transform.position.x, handsMarker.y / 2.0f,transform.position.z);
        }
        Vector3 dummmyEdgeVector = Vector3.ProjectOnPlane((secondDummyMarker - firstDummyMarker), Vector3.up);
        float headToTorsoDistance = scale.x / 2.0f + head.transform.localScale.y / 2.0f;
        head.transform.position = new Vector3(head.transform.position.x, head.transform.localScale.y, head.transform.position.z);

        if (!handsInRightPosition(firstDummyMarker, secondDummyMarker, handsMarker, dummmyEdgeVector))
        {
            performTracking = false;
        }
        dummmyEdgeVector.Normalize();
        transform.right = dummmyEdgeVector;
        head.position = transform.position + transform.right * headToTorsoDistance;

        var middleOfDummy = (firstDummyMarker + (secondDummyMarker - firstDummyMarker) / 2.0f) + (transform.forward * (torso.transform.localScale.z / 2.0f));   //tutaj cos jest ze znakiem +/-
        transform.position = new Vector3(middleOfDummy.x, transform.position.y, middleOfDummy.z);
    }

    public void setInitialPose(Vector3 firstDummyMarker, Vector3 secondDummyMarker, Vector3 handsMarker)
    {
        transform.position = new Vector3(handsMarker.x, handsMarker.y / 2, handsMarker.z);
        Vector3 scale = torso.localScale;      
        scale.y = handsMarker.y;
        Vector3 dummyEdgeVector = Vector3.ProjectOnPlane((secondDummyMarker - firstDummyMarker), Vector3.up);
        scale.x = dummyEdgeVector.magnitude;
        Vector3 distanceFromHandToDummyEdge = firstDummyMarker + (secondDummyMarker - firstDummyMarker) / 2.0f - handsMarker;
        scale.z = Vector3.ProjectOnPlane(distanceFromHandToDummyEdge, Vector3.up).magnitude * 2;
        torso.transform.localScale = scale;

        Vector3 headScale = head.localScale;   
        headScale.x = scale.y;
        headScale.y = scale.y;
        headScale.z = scale.y;
        head.transform.localScale = headScale;

        initialHandLocalXPosition = (Vector3.Project((handsMarker - firstDummyMarker), (secondDummyMarker - firstDummyMarker))).magnitude;
        initialHandLocalZPosition = Vector3.Distance(handsMarker, dummyEdgeVector);
        height = handsMarker.y;
    }

    public float getDummyHeight()
    {
        return height;
    }


    bool handsInRightPosition(Vector3 firstDummyMarker, Vector3 secondDummyMarker, Vector3 handsMarker, Vector3 dummyEdgeVector)
    {
        var currentHandLocalXPosition = (Vector3.Project((firstDummyMarker - secondDummyMarker), (secondDummyMarker - firstDummyMarker))).magnitude;
        var currentHandLocalZPosition = Vector3.Distance(firstDummyMarker, dummyEdgeVector);
        return (Math.Abs(currentHandLocalZPosition - initialHandLocalZPosition) < 0.5) && (Math.Abs(currentHandLocalXPosition - initialHandLocalXPosition) < 0.5);
    }
}
