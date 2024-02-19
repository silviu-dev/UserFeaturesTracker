using System;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;

public class testScript : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        UserPositionCallback userPositionCallback = userPositionCallback_;
        UserGazeCallback userGazeCallback = (int x, int y) => {};
        UserMouthCallback userMouthCallback = userMouthCallback_;
        UserBlinkCallback userBlinkCallback = userBlinkCallback_;

        Thread thread = new Thread(() => { getUserFeatures(userPositionCallback, userGazeCallback, userMouthCallback, userBlinkCallback, false); });
        thread.Start();

        babyHead = Instantiate(prefab, new Vector3(0, 0, -50), Quaternion.identity);
        babyHead.transform.localScale = new Vector3(100, 100, 100);

    }
    private void controlMouth(float opening)
    {

        for (int i = 0; i < babyHead.transform.childCount; ++i)
        {
            Transform currentItem = babyHead.transform.GetChild(i);

            if (currentItem.name.Equals("Head"))
            {
                var rb2d = currentItem.GetComponent<SkinnedMeshRenderer>();
                rb2d.SetBlendShapeWeight(41, opening);
                break;
            }

        }
    }
    private void controlEyes(bool rightEyeBlink, bool leftEyeBlink)
    {
        for (int i = 0; i < babyHead.transform.childCount; ++i)
        {
            Transform currentItem = babyHead.transform.GetChild(i);
            if (currentItem.name.Equals("Head"))
            {
                var rb2d = currentItem.GetComponent<SkinnedMeshRenderer>();
                if (rightEyeBlink)//set right eye on 100% open
                {
                    rb2d.SetBlendShapeWeight(46, 100);
                }
                else//set right eye on 0% open
                {
                    rb2d.SetBlendShapeWeight(46, 0);
                }
                if (leftEyeBlink)//set left eye on 100% open
                {
                    rb2d.SetBlendShapeWeight(45, 100);
                }
                else//set left eye on 0% open
                {
                    rb2d.SetBlendShapeWeight(45, 0);
                }
                continue;
            }

        }
    }
    void controlCamera()
    {
        if (X_ != oldX_ || Y_ != oldX_)//if the user moved from the last time
        {
            if (oldX_ != -1 && oldY_ != -1)//if it's not first call of userPositionCallback from plug-in 
            {
                var deltaX = oldX_ - X_;
                var deltaY = oldY_ - Y_;
                Debug.Log(deltaX);
                //rotate the main camera on (x, y, z)
                Camera.main.transform.Rotate(3 * deltaY * Time.deltaTime, -3 * deltaX * Time.deltaTime, 0f);
                //move the main camera on (x, y, z)
                Camera.main.transform.position = Camera.main.transform.position +
                    new Vector3(-5 * deltaX * Time.deltaTime, 5 * deltaY * Time.deltaTime, 0f);
            }
            oldX_ = X_;
            oldY_ = Y_;
        }
    }
    void userPositionCallback_(int x, int y)
    {
        X_=x;
        Y_=y;
    }
    void userMouthCallback_(double o)
    {
        mouthOpening = (float)o;
    }

    void userBlinkCallback_(bool rightBlink, bool leftBlink)
    {
        rightEyeBlink = rightBlink;
        leftEyeBlink = leftBlink;
    }
    private int X_=-1;
    private int Y_=-1;
    private int oldX_ = -1;
    private int oldY_ = -1;
    private float mouthOpening = 0;
    private bool rightEyeBlink;
    private bool leftEyeBlink;
    // Update is called once per frame
    void Update()
    {
        controlMouth(2.8f* mouthOpening);
        controlEyes(rightEyeBlink, leftEyeBlink);
        controlCamera();
    }
    public GameObject prefab;
    private GameObject babyHead;
    [DllImport("UserFeaturesTracker")]
    public static extern void getUserFeatures(UserPositionCallback userPositionCallback,
        UserGazeCallback userGazeCallback, UserMouthCallback userMouthCallback,
        UserBlinkCallback userBlinkCallback, bool calibration);
    public delegate void UserPositionCallback(int x, int y);
    public delegate void UserGazeCallback(int x, int y);
    public delegate void UserMouthCallback(double opening);
    public delegate void UserBlinkCallback(bool rightEyeBlinked, bool leftEyeBlinked);
}