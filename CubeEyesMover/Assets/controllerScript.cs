using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;
using System.Threading;

public class controllerScript : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        UserPositionCallback userPositionCallback = (int x, int y) => { };
        UserGazeCallback userGazeCallback = userGazeCallback_;
        UserMouthCallback userMouthCallback = (double x) => { };
        UserBlinkCallback userBlinkCallback = (bool x, bool y) => { };

        Thread thread = new Thread(() => { getUserFeatures(userPositionCallback, userGazeCallback, userMouthCallback, userBlinkCallback, true); });
        thread.Start();
        r = GetComponent<Rigidbody2D>();
        gazePos_.x = -100;
        gazePos_.y = -100;
       

    }
    public Text instruction;
    // Update is called once per frame
    void Update()
    {
        applyForce(gazePos_, transform.position);
    }
    Vector3 gazePos_;
    void userGazeCallback_(int x,int y)
    {
        gazePos_.x = x;
        gazePos_.y = y;
    }
    void applyForce(Vector3 gazePos, Vector3 objPos)
    {
        objPos.z = Camera.main.nearClipPlane;
        var convObjPos = Camera.main.WorldToScreenPoint(objPos);
        if(gazePos.x!= -100 && gazePos.y!= -100)
        r.AddForce(new Vector2((gazePos.x - convObjPos.x)*0.10f, (Screen.height - gazePos.y - convObjPos.y) * 0.10f));
        instruction.text ="square position: "+ ((int)convObjPos.x).ToString() +", "+ ((int)convObjPos.y).ToString() +
            "\ngaze position: " + (gazePos.x.ToString()) +", "+ 
            ((Screen.height - gazePos.y).ToString());
        //Debug.Log(convMousePos.x);
    }
    private Rigidbody2D r;
    [DllImport("UserFeaturesTracker")]
    public static extern void getUserFeatures(UserPositionCallback userPositionCallback,
    UserGazeCallback userGazeCallback, UserMouthCallback userMouthCallback,
    UserBlinkCallback userBlinkCallback, bool calibration);
    public delegate void UserPositionCallback(int x, int y);
    public delegate void UserGazeCallback(int x, int y);
    public delegate void UserMouthCallback(double opening);
    public delegate void UserBlinkCallback(bool rightEyeBlinked, bool leftEyeBlinked);
}
