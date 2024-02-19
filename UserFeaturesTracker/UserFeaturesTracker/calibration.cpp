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

#include "calibration.hpp"
#include "helpers.hpp"
#include "pupilDetector.hpp"
#include <Windows.h>

using namespace std;
using namespace cv;
using namespace dlib;

void onClickCallback(int event, int x, int y, int flags, void* userdata);
int screenWidth, screenHeight;
double imageToScreenFactorX, imageToScreenFactorY;
void calibration()
{
    shape_predictor landmarkDetector;
    deserialize("shape_predictor_68_face_landmarks.dat") >> landmarkDetector;
    namedWindow("CalibrationWindow", WINDOW_NORMAL);
    setWindowProperty("CalibrationWindow", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    screenWidth = cv::getWindowImageRect("CalibrationWindow").width;
    screenHeight = cv::getWindowImageRect("CalibrationWindow").height;
    Mat img = imread("calibrationBG.png");
    int windowWidth = img.cols - 40;
    int windowHeight = img.rows - 40;

    std::vector<std::pair<cv::Point, cv::Point>> detecteEyePoints;
    std::vector<cv::Point> calibrationPoints
    { {20,20},{20 + windowWidth / 3,20},{20 + windowWidth / 3 * 2,20}, {20 + windowWidth,20},
    {20,20 + windowHeight / 3 },{20 + windowWidth / 3,20 + windowHeight / 3},{20 + windowWidth / 3 * 2,20 + windowHeight / 3}, {20 + windowWidth,20 + windowHeight / 3},
    {20,20 + windowHeight / 3 * 2},{20 + windowWidth / 3,20 + windowHeight / 3 * 2},{20 + windowWidth / 3 * 2,20 + windowHeight / 3 * 2}, {20 + windowWidth,20 + windowHeight / 3 * 2},
    {20,20 + windowHeight},{20 + windowWidth / 3,20 + windowHeight},{20 + windowWidth / 3 * 2,20 + windowHeight}, {20 + windowWidth,20 + windowHeight} };

    auto calibrationPointsIt = calibrationPoints.begin();
    auto detecteEyePointsIt = std::inserter<std::vector<std::pair<
        cv::Point, cv::Point>>>(detecteEyePoints, detecteEyePoints.begin());

    while (calibrationPointsIt != calibrationPoints.end())
    {
        //draw circle on the screen
        circle(img, Point((*calibrationPointsIt).x, (*calibrationPointsIt).y), 20, Scalar(0, 0, 255), -1, 1, 0);
        imshow("CalibrationWindow", img);
        std::tuple<std::reference_wrapper<std::insert_iterator<std::vector<
            std::pair<cv::Point, cv::Point> >>>,
            std::reference_wrapper<std::vector<cv::Point>::iterator>,
            std::reference_wrapper<shape_predictor>>
            context = { detecteEyePointsIt, calibrationPointsIt, landmarkDetector };
        setMouseCallback("CalibrationWindow", onClickCallback, &context);

        waitKey(1);
    }

    std::vector<cv::Point> detectedRightEye;
    std::vector<cv::Point> detectedLeftEye;
    for (auto it = detecteEyePoints.begin(); it != detecteEyePoints.end(); it++)
    {
        detectedRightEye.push_back((*it).first);
        detectedLeftEye.push_back((*it).second);
    }

    for (auto& it : calibrationPoints)
    {
        it = cv::Point(it.x * imageToScreenFactorX, it.y * imageToScreenFactorY);
    }

    calculateCoeff(detectedRightEye, calibrationPoints,true);

    calculateCoeff(detectedLeftEye, calibrationPoints, false);

    decision_function<kernel_type> rightEyeRegX, rightEyeRegY, leftEyeRegX, leftEyeRegY;
    deserialize("rightEyeRegX.dat") >> rightEyeRegX;
    deserialize("leftEyeRegY.dat") >> leftEyeRegY;
    for (auto it : detectedRightEye)
    {
        sample_type m;
        m(0) = it.x;
        cout << "input" << it.x << " output " << rightEyeRegX(m) << "\n";
    }

    destroyWindow("CalibrationWindow");
}

void onClickCallback(int event, int x, int y, int flags, void* userdata)
{
    //if left mouse click was press
    if (event == EVENT_LBUTTONDOWN)
    {
        float resizedHeight = 480;
        //std::vector<KeyPoint> rightEyekeypoints, leftEyekeypoints;
        frontal_face_detector faceDetector = get_frontal_face_detector();

        auto context = (std::tuple<std::reference_wrapper<std::insert_iterator<std::vector<
            std::pair<cv::Point, cv::Point> >>>, std::reference_wrapper<
            std::vector<cv::Point>::iterator>,
            std::reference_wrapper<shape_predictor>>*)userdata;
        auto& detectedPointsIt = std::get<0>(*context).get();
        auto& calibrationPointsIt = std::get<1>(*context).get();
        auto& landmarkDetector = std::get<2>(*context).get();
        Mat frame, croppedFrame, leftEye, rightEye;
        bool readEye = false;
        cv::VideoCapture cam;
        cam.open(0);
        while (!readEye)
        {
            cam >> frame;

            imageToScreenFactorX = (double)screenWidth / (frame.cols);
            imageToScreenFactorY = (double)screenHeight / (frame.rows);
            resize(frame, croppedFrame, Size(), frame.rows / resizedHeight, frame.rows / resizedHeight, INTER_CUBIC);
            cv_image<bgr_pixel> dlibImage(croppedFrame);
            auto faces = faceDetector(dlibImage);

            if (faces.size() != 0)
            {
                dlib::rectangle rect(int(faces[0].left() * frame.rows / resizedHeight),
                    int(faces[0].top() * frame.rows / resizedHeight),
                    int(faces[0].right() * frame.rows / resizedHeight),
                    int(faces[0].bottom() * frame.rows / resizedHeight));

                full_object_detection faceLandmark = landmarkDetector(dlibImage, rect);
                auto eyesPerimeter = getEyesPerimeter(faceLandmark);
                leftEye = frame(eyesPerimeter.first);
                rightEye = frame(eyesPerimeter.second);
                cv::rectangle(frame, eyesPerimeter.first, cv::Scalar(0, 255, 0));
                cv::rectangle(frame, eyesPerimeter.second, cv::Scalar(0, 255, 0));
                
                auto leftEyepoint = findEyeCenter(frame, eyesPerimeter.first);
                auto rightEyepoint = findEyeCenter(frame, eyesPerimeter.second);
                auto faceCenterPoint = getFaceCenter(faceLandmark);

                leftEyepoint = convertEyePointToFacePoint(leftEyepoint, eyesPerimeter.first);
                rightEyepoint = convertEyePointToFacePoint(rightEyepoint, eyesPerimeter.second);

                circle(frame, leftEyepoint, 2, Scalar(0, 0, 255), -1, 1, 0);
                circle(frame, rightEyepoint, 2, Scalar(0, 0, 255), -1, 1, 0);
                circle(frame, faceCenterPoint, 2, Scalar(0, 0, 255), -1, 1, 0);
                imshow("eyeTracker", frame);
                leftEyepoint = cv::Point(leftEyepoint.x * imageToScreenFactorX,
                    leftEyepoint.y * imageToScreenFactorY);
                rightEyepoint = cv::Point(rightEyepoint.x * imageToScreenFactorX,
                    rightEyepoint.y * imageToScreenFactorY);
                faceCenterPoint = cv::Point(faceCenterPoint.x * imageToScreenFactorX,
                    faceCenterPoint.y * imageToScreenFactorY);
                if (leftEyepoint != Point(-1, -1) && rightEyepoint != Point(-1, -1))
                {
                    readEye = true;
                    (*detectedPointsIt) = std::make_pair(cv::Point(rightEyepoint.x - faceCenterPoint.x,
                        rightEyepoint.y - faceCenterPoint.y),
                        cv::Point(leftEyepoint.x - faceCenterPoint.x, leftEyepoint.y - faceCenterPoint.y));

                }

            }
            else
            {
                cout << "No faces detected\n";
            }
            waitKey(1);

        }
        calibrationPointsIt++;
    }
}