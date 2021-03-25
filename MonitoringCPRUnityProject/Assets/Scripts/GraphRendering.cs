using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class GraphRendering : MonoBehaviour
{
    //public List<GameObject> dots;
    
    List<float> yValues;
    public float maxValue;
    public float minValue;
    public int numberOfPoints;
    public GameObject yAxis;
    float yUnit, xUnit;
    float xAxisLeft, xAxisRight;
    float xAxisLength;
    public float highLimit, lowLimit, yAxisLineHeight;
    LineRenderer lineRenderer;
    //public TextMesh maxValueText, minValueText, lowLimitText, highLimitText, YAxisLineText, unitsText;
    public string units;
    public SpriteRenderer circle;

    List<SpriteRenderer> dots;
    // Start is called before the first frame update
    void Start()
    {
        dots = new List<SpriteRenderer>(numberOfPoints);
        lineRenderer = GetComponent<LineRenderer>();
        yValues = new List<float>();
        yUnit = 0.8f / (maxValue - minValue);
        xAxisLeft = transform.position.x - 0.43f * transform.localScale.x;
        xAxisRight = transform.position.x + 0.43f * transform.localScale.x;
        xAxisLength = Math.Abs(xAxisRight - xAxisLeft);
        xUnit = (xAxisLength / numberOfPoints);
        initClearValues();       
        initializeGraph();
    }

    // Update is called once per frame
    void Update()
    {
        for(int i = 0; i < numberOfPoints; ++i)
        {
            dots[i].gameObject.transform.position = new Vector3(/*dots[i].transform.position.x*/  (xAxisLeft + i * xUnit)  , yAxis.transform.position.y + (yValues[i] - minValue) * yUnit, yAxis.transform.position.z);

            if(yValues[i] >= lowLimit && yValues[i] <= highLimit)
            {
                dots[i].gameObject.GetComponent<SpriteRenderer>().color = Color.green;
            }
            else
            {
                dots[i].gameObject.GetComponent<SpriteRenderer>().color = Color.red;
            }
        }
    }

    public void addValue(float newValue)
    {
        for(int i = 0; i < numberOfPoints - 1; ++i)
        {
            yValues[i] = yValues[i + 1];            
        }
        if(newValue > maxValue)
        {
            newValue = maxValue;
        }
        else if(newValue < minValue)
        {
            newValue = minValue;
        }
        
        yValues[numberOfPoints - 1] = newValue;
    }

    void initializeGraph()
    {
        lineRenderer.sortingLayerName = "Background";
        
        lineRenderer.positionCount = 11;
        lineRenderer.SetPosition(0, new Vector3(xAxisLeft, yAxis.transform.position.y, yAxis.transform.position.z));
        lineRenderer.SetPosition(1, new Vector3(xAxisLeft, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(2, new Vector3(xAxisRight, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(3, new Vector3(xAxisLeft, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(4, new Vector3(xAxisLeft, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(5, new Vector3(xAxisRight, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(6, new Vector3(xAxisLeft, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(7, new Vector3(xAxisLeft, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(8, new Vector3(xAxisRight, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(9, new Vector3(xAxisLeft, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(10, new Vector3(xAxisLeft, yAxis.transform.position.y + (maxValue - minValue) * yUnit, yAxis.transform.position.z));

        var unitsText = transform.Find("UnitsName").GetComponent<TextMesh>();
        var maxValueText = transform.Find("MaxValueText").GetComponent<TextMesh>();
        var minValueText = transform.Find("MinValueText").GetComponent<TextMesh>();
        var lowLimitText = transform.Find("LowLimitText").GetComponent<TextMesh>();
        var highLimitText = transform.Find("HighLimitText").GetComponent<TextMesh>();
        var YAxisLineText = transform.Find("YAxisLineText").GetComponent<TextMesh>();

        unitsText.text = "(" + units + ")";
        maxValueText.text = maxValue.ToString();
        minValueText.text = minValue.ToString();
        lowLimitText.text = lowLimit.ToString();
        highLimitText.text = highLimit.ToString();

        unitsText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (maxValue - minValue) * yUnit + 0.2f, maxValueText.transform.position.z);
        maxValueText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (maxValue - minValue) * yUnit + 0.1f, maxValueText.transform.position.z);
        minValueText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + 0.1f, maxValueText.transform.position.z);
        lowLimitText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (lowLimit - minValue) * yUnit + 0.1f, maxValueText.transform.position.z);
        highLimitText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (highLimit - minValue) * yUnit + 0.1f, maxValueText.transform.position.z);        

        if (minValue != yAxisLineHeight)
        {
            YAxisLineText.text = yAxisLineHeight.ToString();
            YAxisLineText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit + 0.1f, maxValueText.transform.position.z);
        }
        else
        {
            YAxisLineText.gameObject.SetActive(false);
        }
    }

    public void initClearValues()
    {
        for (int i = 0; i < numberOfPoints; ++i)
        {
            //GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            //var scale = sphere.transform.localScale;
            //scale.x = 0.025f;
            //scale.y = 0.05f;
            //scale.z = 0.025f;
            //sphere.transform.localScale = scale;
            //newDots.Add(sphere);
            SpriteRenderer clone = Instantiate(circle, transform, true);
            clone.sortingLayerName = "Foreground";
            var scale = clone.gameObject.transform.localScale;
            scale.x = xUnit/ 1.5f ;
            scale.y = xUnit/ 1.5f ;
            
            clone.gameObject.transform.localScale = scale;

            dots.Add(clone);
            yValues.Add(0.0f);
        }
    }
}
