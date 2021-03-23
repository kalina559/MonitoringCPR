using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cylinder : MonoBehaviour
{
    public GameObject firstMarker, secondMarker;
    // Update is called once per frame
    void Update()
    {
        transform.position = (firstMarker.transform.position - secondMarker.transform.position) / 2.0f + secondMarker.transform.position;

        var v3T = transform.localScale;      // Scale it
        v3T.y = (firstMarker.transform.position - secondMarker.transform.position).magnitude/4;
        transform.localScale = v3T;
        // Rotate it
        transform.rotation = Quaternion.FromToRotation(Vector3.up, firstMarker.transform.position - secondMarker.transform.position);
    }
}
