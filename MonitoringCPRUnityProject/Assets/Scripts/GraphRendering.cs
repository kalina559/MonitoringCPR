using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GraphRendering : MonoBehaviour
{
    public List<GameObject> dots;
    List<float> yValues;
    public float maxValue;
    public float minValue;
    public GameObject yAxis;
    float yUnit;
    public float highLimit, lowLimit, yAxisLineHeight;
    LineRenderer lineRenderer;
    //public TextMesh maxValueText, minValueText, lowLimitText, highLimitText, YAxisLineText, unitsText;
    public string units;
    // Start is called before the first frame update
    void Start()
    {
        yValues = new List<float>();
        initClearValues();
        yUnit = 0.8f / (maxValue - minValue);

        lineRenderer = GetComponent<LineRenderer>();
        initializeGraph();
    }

    // Update is called once per frame
    void Update()
    {
        for(int i = 0; i < 20; ++i)
        {
            dots[i].transform.position = new Vector3(dots[i].transform.position.x, yAxis.transform.position.y + (yValues[i] - minValue) * yUnit, dots[i].transform.position.z);

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
        for(int i = 0; i < 19; ++i)
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
        
        yValues[19] = newValue;
    }

    void initializeGraph()
    {
        lineRenderer.positionCount = 11;
        lineRenderer.SetPosition(0, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y, yAxis.transform.position.z));
        lineRenderer.SetPosition(1, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(2, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(3, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (yAxisLineHeight - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(4, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(5, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(6, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (lowLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(7, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(8, new Vector3(dots[19].transform.position.x, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(9, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (highLimit - minValue) * yUnit, yAxis.transform.position.z));
        lineRenderer.SetPosition(10, new Vector3(dots[0].transform.position.x, yAxis.transform.position.y + (maxValue - minValue) * yUnit, yAxis.transform.position.z));

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

        unitsText.transform.position = new Vector3(maxValueText.transform.position.x, yAxis.transform.position.y + (maxValue - minValue) * yUnit + 0.3f, maxValueText.transform.position.z);
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
        for (int i = 0; i < 20; ++i)
        {
            yValues.Add(0.0f);
        }
    }
}
