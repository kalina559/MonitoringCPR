using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cylinder : MonoBehaviour
{
    public GameObject firstMarker, secondMarker;
    void Update()
    {
        transform.position = (firstMarker.transform.position - secondMarker.transform.position) / 2.0f + secondMarker.transform.position;

        var scale = transform.localScale; 
        scale.y = (firstMarker.transform.position - secondMarker.transform.position).magnitude/4;
        transform.localScale = scale;
        transform.rotation = Quaternion.FromToRotation(Vector3.up, firstMarker.transform.position - secondMarker.transform.position);
    }
}
