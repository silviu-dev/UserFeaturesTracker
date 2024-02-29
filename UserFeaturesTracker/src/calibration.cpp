#include<dlib/image_processing.h>
#include<dlib/image_processing/frontal_face_detector.h>
#include<iostream>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <chrono>
#include <tuple>
#include "calibration.hpp"
#include "helpers.hpp"
#include "pupilDetector.hpp"
#include <Windows.h>
#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>


using namespace std;
using namespace cv;
using namespace dlib;
using namespace chrono;

int screenWidth, screenHeight;
double imageToScreenFactorX, imageToScreenFactorY;
std::atomic<bool> processingFinished(false);
bool processingThredOngoing=false;
frontal_face_detector faceDetector;
full_object_detection faceLandmark;
float resizedHeight = 480;
shape_predictor landmarkDetector;
std::atomic<bool> leftClickPressed(false);
std::pair<cv::Point, cv::Point> calculateGaze();

void onClickCallback(int event, int x, int y, int flags, void*)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        leftClickPressed=true;
    }
}
void calibration()
{
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
    { {20,20},{20 + windowWidth / 3,20}};//{20 + windowWidth / 3 * 2,20}, {20 + windowWidth,20},
    // {20,20 + windowHeight / 3 },{20 + windowWidth / 3,20 + windowHeight / 3},{20 + windowWidth / 3 * 2,20 + windowHeight / 3}, {20 + windowWidth,20 + windowHeight / 3},
    // {20,20 + windowHeight / 3 * 2},{20 + windowWidth / 3,20 + windowHeight / 3 * 2},{20 + windowWidth / 3 * 2,20 + windowHeight / 3 * 2}, {20 + windowWidth,20 + windowHeight / 3 * 2},
    // {20,20 + windowHeight},{20 + windowWidth / 3,20 + windowHeight},{20 + windowWidth / 3 * 2,20 + windowHeight}, {20 + windowWidth,20 + windowHeight} };

    auto calibrationPointsIt = calibrationPoints.begin();
    auto detecteEyePointsIt = std::inserter<std::vector<std::pair<
        cv::Point, cv::Point>>>(detecteEyePoints, detecteEyePoints.begin());

    setMouseCallback("CalibrationWindow", onClickCallback);
    while (calibrationPointsIt != calibrationPoints.end())
    {
        //draw circle on the screen
        circle(img, Point((*calibrationPointsIt).x, (*calibrationPointsIt).y), 20, Scalar(0, 0, 255), -1, 1, 0);
        imshow("CalibrationWindow", img);
        if(leftClickPressed==true)
        {
            std::cout<<"click apasat"<<std::endl;
            leftClickPressed=false;
            detecteEyePoints.push_back(calculateGaze());
            calibrationPointsIt++;
        }
        waitKey(1);
    }
    cv::setMouseCallback("CalibrationWindow", NULL);

    std::vector<cv::Point> detectedRightEye;
    std::vector<cv::Point> detectedLeftEye;
    for (auto it = detecteEyePoints.begin(); it != detecteEyePoints.end(); it++)
    {
        std::cout<<"rightEye, leftEye "<<(*it).first<<" "<<(*it).second<<std::endl;
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

void processImage(const cv::Mat& inputImage) {
    dlib::cv_image<dlib::bgr_pixel> dlibImage(inputImage);

        // Detectam fetele in frame
    auto faces = faceDetector(dlibImage);
    if(!faces.empty())
    {
        // Calculul coordonatelor și dimensiunilor dreptunghiului de decupare
        int x = int(faces[0].left() * inputImage.cols / resizedHeight);
        int y = int(faces[0].top() * inputImage.rows / resizedHeight);
        int width = int((faces[0].right() - faces[0].left()) * inputImage.cols / resizedHeight);
        int height = int((faces[0].bottom() - faces[0].top()) * inputImage.rows / resizedHeight);

        // Asigurarea că dreptunghiul de decupare rămâne în limitele imaginii
        x = std::max(0, x);
        y = std::max(0, y);
        width = std::min(inputImage.cols - x, width);
        height = std::min(inputImage.rows - y, height);

        // Crearea dreptunghiului de decupare
        dlib::rectangle face(x, y, x + width, y + height);

        faceLandmark = landmarkDetector(dlibImage, face);
        cout<<"faceLandmark.nr "<<faceLandmark.num_parts();
    }
    else
    {
         faceLandmark = dlib::full_object_detection();
         cout<<"fata NEdetectata"<<std::endl;
    }
    processingFinished = true;

}


std::pair<cv::Point, cv::Point> calculateGaze()
{
    std::cerr << "silviu intrare in calculateGaze"<<std::endl;
    cv::setMouseCallback("CalibrationWindow", NULL);
    faceDetector = get_frontal_face_detector();

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Eroare la deschiderea camerei web." << std::endl;
        setMouseCallback("CalibrationWindow", onClickCallback);
        return std::make_pair(cv::Point(-1, -1), cv::Point(-1, -1));
    }

    cv::Mat frame, croppedFrame;

    while (true) 
    {
    cap.read(frame);
    if (frame.empty()) {
        std::cerr << "Frame gol sau nu a putut fi citit de la camera web." << std::endl;
        break;
    }
    cv::imwrite("silviu.jpg", frame);
    imageToScreenFactorX = (double)screenWidth / (frame.cols);
    imageToScreenFactorY = (double)screenHeight / (frame.rows);
    resize(frame, croppedFrame, Size(), frame.rows / resizedHeight, frame.rows / resizedHeight, INTER_CUBIC);
    // Afișarea feed-ului video în fereastra "Camera Web" în firul de execuție principal
    // Crearea unui fir de execuție secundar pentru procesarea imaginii
    if(processingThredOngoing == false)
    {
        processingThredOngoing = true;
        std::thread processingThread(processImage, croppedFrame.clone());
        processingThread.detach(); // Detasam firul de execuție pentru a rula independent
    }
    // Aici puteti continua executia fara a astepta dupa firul secundar

    // Așteptăm un scurt interval de timp pentru a evita prea multa solicitare a procesorului
    cv::waitKey(1);
    // Verificăm dacă operația costisitoare este finalizată în mod asincron
    if (processingFinished == true) 
    {
        processingFinished = false;
        processingThredOngoing=false;
        std::cout<<faceLandmark.num_parts()<<std::endl;
        if (faceLandmark.num_parts() == 68)
        {
            std::cout<<"FATADETECTATA"<<std::endl;
            auto eyesPerimeter = getEyesPerimeter(faceLandmark);
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
            leftEyepoint = cv::Point(leftEyepoint.x * imageToScreenFactorX,
                leftEyepoint.y * imageToScreenFactorY);
            rightEyepoint = cv::Point(rightEyepoint.x * imageToScreenFactorX,
                rightEyepoint.y * imageToScreenFactorY);
            faceCenterPoint = cv::Point(faceCenterPoint.x * imageToScreenFactorX,
                faceCenterPoint.y * imageToScreenFactorY);
            if (leftEyepoint != Point(-1, -1) && rightEyepoint != Point(-1, -1))
            {
                cap.release();
                setMouseCallback("CalibrationWindow", onClickCallback);
                return std::make_pair(cv::Point(rightEyepoint.x - faceCenterPoint.x,
                    rightEyepoint.y - faceCenterPoint.y),
                    cv::Point(leftEyepoint.x - faceCenterPoint.x, leftEyepoint.y - faceCenterPoint.y));

            }
        }
        else
        {
            std::cout<<"FATANEDETECTATA"<<std::endl;
        }
    }
    cv::imshow("eyeTracker", frame);
    // Așteptare pentru apăsarea tastei 'ESC' pentru a ieși din buclă
    if (cv::waitKey(1) == 27)
        break;
    }
}
