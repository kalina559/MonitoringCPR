using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SubjectModel : MonoBehaviour
{
    Transform head, hips, neck;
    void Start()
    {
        hips = transform.Find("Hips");
        head = transform.Find("Head");
        neck = transform.Find("Neck");
    }
    public void setSubjectInitialPose(Vector3 offset, Transform dummy)
    {
        hips.position = dummy.position + dummy.forward * (dummy.localScale.z / 2 + 0.3f) + offset;
        hips.position = new Vector3(hips.position.x, 0.6f, hips.position.z);
    }
    public void updateSubjectPose(Vector3 rightArm, Vector3 leftArm)
    {
        neck.transform.position = (leftArm - rightArm) / 2.0f + rightArm;
        neck.up = neck.position - hips.position;
    }
}
