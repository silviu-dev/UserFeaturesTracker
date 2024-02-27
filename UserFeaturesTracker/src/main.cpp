#include<dlib/opencv.h>
#include<dlib/image_processing.h>
#include<dlib/image_processing/frontal_face_detector.h>
#include<iostream>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <chrono>
#include <tuple>
#include <Windows.h>
#include <future>
#include <thread>
#include <iostream>

#include "calibration.hpp"
#include "helpers.hpp"
#include "pupilDetector.hpp"
#include "UserFeaturesTracker.hpp"

using namespace std;
using namespace std::chrono;
using namespace cv;
using namespace dlib;

extern "C" __declspec(dllexport)
void getUserFeatures(UserPositionCallback userPositionCallback, UserGazeCallback userGazeCallback, 
	UserMouthCallback userMouthCallback, UserBlinkCallback userBlinkCallback, bool calib)
{
    namedWindow("eyeTracker", WINDOW_NORMAL);
    std::cout<<"silviu 1\n";
    if (calib)
    {std::cout<<"silviu 2\n";
        calibration();
    }
    std::cout<<"silviu 3\n";
    decision_function<kernel_type> rightEyeRegX, rightEyeRegY, leftEyeRegX, leftEyeRegY;
    std::cout<<"silviu 4\n";
    deserialize("rightEyeRegX.dat") >> rightEyeRegX;
    std::cout<<"silviu 5\n";
    deserialize("rightEyeRegY.dat") >> rightEyeRegY;
    deserialize("leftEyeRegX.dat") >> leftEyeRegX;
    deserialize("leftEyeRegY.dat") >> leftEyeRegY;

    float resizedHeight = 480;
    Mat frame, croppedFrame, leftEye, rightEye;
    std::vector<dlib::rectangle> faces;

    VideoCapture cam(0);
    if (!cam.isOpened()) {
        cout << "cannot open webcam!\n";
        return;
    }
    frontal_face_detector faceDetector = get_frontal_face_detector();
    shape_predictor landmarkDetector;
    deserialize("shape_predictor_68_face_landmarks.dat") >> landmarkDetector;
    setWindowProperty("eyeTracker", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    int screenWidth = cv::getWindowImageRect("eyeTracker").width;
    int screenHeight = cv::getWindowImageRect("eyeTracker").height;
    double imageToScreenFactorX, imageToScreenFactorY;

    steady_clock::time_point start1, start2, stop1, stop2;
    while (true)
    {
        //zstart1 = high_resolution_clock::now();
        cam >> frame;
        imageToScreenFactorX = (double)screenWidth / frame.cols;
        imageToScreenFactorY = (double)screenHeight / frame.rows;
        resize(frame, croppedFrame, Size(), frame.rows / resizedHeight, frame.rows / resizedHeight, INTER_CUBIC);
        auto start1 = high_resolution_clock::now();
        cv_image<bgr_pixel> dlibImage(croppedFrame);
        cv_image<bgr_pixel> fulldlibImage(frame);
        faces = faceDetector(dlibImage);
        //stop1 = high_resolution_clock::now();
        //auto duration = duration_cast<milliseconds>(stop1 - start1);
        //cout << "Time taken for face detection: "
        //    << duration.count() << " milliseconds" << endl;
        //start2 = high_resolution_clock::now();

        if (faces.size() != 0)
        {
            dlib::rectangle rect(int(faces[0].left() * frame.rows / resizedHeight),
                int(faces[0].top() * frame.rows / resizedHeight),
                int(faces[0].right() * frame.rows / resizedHeight),
                int(faces[0].bottom() * frame.rows / resizedHeight));

            full_object_detection faceLandmark = landmarkDetector(fulldlibImage, rect);

            auto eyesPerimeter = getEyesPerimeter(faceLandmark);

            auto leftEyeFuturepoint = std::async(std::launch::async, findEyeCenter, std::ref(frame), eyesPerimeter.first);
            auto rightEyeFuturepoint = std::async(std::launch::async, findEyeCenter,std::ref(frame), eyesPerimeter.second);
            auto faceCenterPoint = getFaceCenter(faceLandmark);
            auto blinkPair = verifyBlink(faceLandmark);
            double mouthOpening = getMouthOpening(faceLandmark);
            cout << mouthOpening << " ";
            circle(frame, cv::Point(faceLandmark.part(62).x(), faceLandmark.part(62).y()), 
                3, Scalar(0, 255, 255), -1, 8, 0);
            circle(frame, cv::Point(faceLandmark.part(66).x(), faceLandmark.part(66).y()+ 
                (faceLandmark.part(66).y() - faceLandmark.part(62).y())),
                3, Scalar(255, 0, 255), -1, 8, 0);

            circle(frame, faceCenterPoint, 3, Scalar(255, 255, 255), -1, 8, 0);
            faceCenterPoint = cv::Point(faceCenterPoint.x * imageToScreenFactorX,
                faceCenterPoint.y * imageToScreenFactorY);
            userPositionCallback(faceCenterPoint.x, faceCenterPoint.y);

            auto leftEyepoint = convertEyePointToFacePoint(leftEyeFuturepoint.get(), eyesPerimeter.first);
            auto rightEyepoint = convertEyePointToFacePoint(rightEyeFuturepoint.get(), eyesPerimeter.second);

            circle(frame, leftEyepoint, 3, Scalar(255, 255, 0), -1, 8, 0);
            circle(frame, rightEyepoint, 3, Scalar(255, 255, 0), -1, 8, 0);

            leftEyepoint = cv::Point(leftEyepoint.x * imageToScreenFactorX,
                leftEyepoint.y * imageToScreenFactorY);
            rightEyepoint = cv::Point(rightEyepoint.x * imageToScreenFactorX,
                rightEyepoint.y * imageToScreenFactorY);


            userBlinkCallback(blinkPair.first, blinkPair.second);

            userMouthCallback(mouthOpening);


            sample_type rightX, rightY, leftX, leftY;
            rightX(0) = (rightEyepoint.x - faceCenterPoint.x);
            rightY(0) = (rightEyepoint.y - faceCenterPoint.y);

            leftX(0) = (leftEyepoint.x - faceCenterPoint.x);
            leftY(0) = (leftEyepoint.y - faceCenterPoint.y);

          //  SetCursorPos((rightEyeRegX(rightX) + leftEyeRegX(leftX)) / 2,
            //    (rightEyeRegY(rightY) + leftEyeRegY(leftY)) / 2);

            userGazeCallback((rightEyeRegX(rightX) + leftEyeRegX(leftX)) / 2,
                (rightEyeRegY(rightY) + leftEyeRegY(leftY)) / 2);

            //cv::rectangle(frame, eyesPerimeter.first, cv::Scalar(0, 255, 0));
            //cv::rectangle(frame, eyesPerimeter.second, cv::Scalar(0, 255, 0));
            flip(frame, frame, 1);
            imshow("eyeTracker", frame);
            //stop2 = high_resolution_clock::now();
            //auto duration2 = duration_cast<milliseconds>(stop2 - start2);
            //cout << "Time taken for eyes detection: "
            //    << duration2.count() << " milliseconds" << endl;
        }
        else
        {
            cout << "No faces detected\n";
        }

        // imshow("eyeTracker", frame);
        if (waitKey(1) == 27)
        {
            break;
        }
    }
    cam.release();
    destroyAllWindows();
    return;
}