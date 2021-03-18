using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GraphRendering : MonoBehaviour
{
    public List<GameObject> dots;
    List<float> yValues;
    public float maxValue;
    public GameObject yAxis;
    float yUnit;
    public float highLimit, lowLimit;
    LineRenderer lineRenderer;
    // Start is called before the first frame update
    void Start()
    {
        yValues = new List<float>();
        for (int i = 0; i < 20; ++i)
        {
            yValues.Add(0.0f);
        }
        yUnit = 0.8f / maxValue;

        lineRenderer = GetComponent<LineRenderer>();
        setLineRenderer();
    }

    // Update is called once per frame
    void Update()
    {
        for(int i = 0; i < 20; ++i)
        {
            dots[i].transform.position = new Vector3(dots[i].transform.position.x, yAxis.transform.position.y + yValues[i] * yUnit, dots[i].transform.position.z);
        }
    }

    public void addValue(float newValue)
    {
        for(int i = 0; i < 19; ++i)
        {
            yValues[i] = yValues[i + 1];            
        }
        if(newValue > maxValue)
        {
            newValue = maxValue;
        }
        yValues[19] = newValue;
    }

    void setLineRenderer()
    {
        lineRenderer.SetPosition(0, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y, yAxis.transform.position.z));
        lineRenderer.SetPosition(1, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y, yAxis.transform.position.z));
        lineRenderer.SetPosition(2, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + lowLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(3, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y + lowLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(4, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + lowLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(5, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + highLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(6, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y + highLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(7, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + highLimit * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(8, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + 0.8f, yAxis.transform.position.z));
    }
}
