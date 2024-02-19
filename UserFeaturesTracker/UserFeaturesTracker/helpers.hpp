#include<dlib/opencv.h>
#include<dlib/image_processing.h>


cv::Rect&& getEyePerimeter(dlib::full_object_detection landmarks,
	int start, int end);
std::pair<cv::Rect, cv::Rect> getEyesPerimeter(dlib::full_object_detection landmarks);
std::pair<bool, bool> verifyBlink(dlib::full_object_detection landmarks);
double getMouthOpening(dlib::full_object_detection landmarks);
cv::Point convertEyePointToFacePoint(cv::Point eyePoint, cv::Rect eyeRegion);
cv::Point getFaceCenter(dlib::full_object_detection landmarks);