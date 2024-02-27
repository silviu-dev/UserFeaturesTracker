#include<opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "helpers.hpp"
using namespace cv;

cv::Rect&& getEyePerimeter(dlib::full_object_detection landmarks, int start, int end) 
{
    cv::Point minPoint(10000, 10000);
    cv::Point maxPoint(0, 0);
    int padding = 5;
    for (int i = start; i <= end; i++) {
        if (landmarks.part(i).x() - padding < minPoint.x)
        {
            minPoint.x = landmarks.part(i).x() - padding;
        }
        if (landmarks.part(i).y() - padding < minPoint.y)
        {
            minPoint.y = landmarks.part(i).y() - padding;
        }
        if (landmarks.part(i).x() + padding > maxPoint.x)
        {
            maxPoint.x = landmarks.part(i).x() + padding;
        }
        if (landmarks.part(i).y() + padding > maxPoint.y)
        {
            maxPoint.y = landmarks.part(i).y() + padding;
        }
    }
    return std::move(cv::Rect(minPoint, maxPoint));
}

std::pair<cv::Rect, cv::Rect> getEyesPerimeter(dlib::full_object_detection landmarks) {

    cv::Rect rightEyePerimter = getEyePerimeter(landmarks, 36, 41);
    cv::Rect leftEyePerimter = getEyePerimeter(landmarks, 42, 47);

    return std::make_pair(leftEyePerimter, rightEyePerimter);
}
cv::Point getFaceCenter(dlib::full_object_detection landmarks)
{
    return cv::Point(landmarks.part(27).x(), landmarks.part(27).y());
}

double euclidDistance(dlib::point point1, dlib::point point2)
{
    return sqrt(std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2));
}
std::pair<double, double> calculateEAR(dlib::full_object_detection landmarks)
{
    double leftEyeEAR = (euclidDistance(landmarks.part(43), landmarks.part(47)) +
        (euclidDistance(landmarks.part(44), landmarks.part(46))))
        / (2 * (euclidDistance(landmarks.part(42), landmarks.part(45))));
    double rightEyeEAR = (euclidDistance(landmarks.part(37), landmarks.part(41)) +
        (euclidDistance(landmarks.part(38), landmarks.part(40))))
        / (2 * (euclidDistance(landmarks.part(39), landmarks.part(36))));

    return std::make_pair(rightEyeEAR, leftEyeEAR);
}

std::pair<bool, bool> verifyBlink(dlib::full_object_detection landmarks)
{
    auto pairEar = calculateEAR(landmarks);
    auto pairBlink = std::make_pair(false, false);
    if (pairEar.first < 0.22)
        pairBlink.first = true;
    if (pairEar.second < 0.22)
        pairBlink.second = true;
    return pairBlink;
}

double getMouthOpening(dlib::full_object_detection landmarks)
{
    return euclidDistance(landmarks.part(62), landmarks.part(66));
}

cv::Point convertEyePointToFacePoint(cv::Point eyePoint, cv::Rect eyeRegion)
{
    cv::Point convertedPoint;
    convertedPoint.x = eyeRegion.x + eyePoint.x;
    convertedPoint.y = eyeRegion.y + eyePoint.y;
    return convertedPoint;
}