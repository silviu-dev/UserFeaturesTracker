#include<dlib/opencv.h>
#include<dlib/image_processing.h>
#include<dlib/image_processing/frontal_face_detector.h>
#include<iostream>
#include<opencv2/imgproc.hpp>


#include "helpers.hpp"
#include "regression.hpp"

using namespace std;
using namespace dlib;
using namespace cv;

void calculateCoeff(std::vector<cv::Point>& X, std::vector<cv::Point>& Y, bool rightEye)
{
    sample_type m;
    std::vector<sample_type> samplesX;
    std::vector<double> labelsX;
    std::vector<sample_type> samplesY;
    std::vector<double> labelsY;
    for (auto& it : X)
    {
        m(0) = it.x;
        samplesX.push_back(m);
        m(0) = it.y;
        samplesY.push_back(m);
    }
    for (auto& it : Y)
    {
        labelsX.push_back(it.x);
        labelsY.push_back(it.y);
    }
    rvm_regression_trainer<kernel_type> trainer;

    const double gamma = 2.0 / compute_mean_squared_distance(samplesX);
    trainer.set_kernel(kernel_type(gamma));
    trainer.set_epsilon(0.001);

    decision_function<kernel_type> regX = trainer.train(samplesX, labelsX);
    decision_function<kernel_type> regY = trainer.train(samplesY, labelsY);

    if (rightEye)
    {
        serialize("rightEyeRegX.dat") << regX;
        serialize("rightEyeRegY.dat") << regY;
    }
    else
    {
        serialize("leftEyeRegX.dat") << regX;
        serialize("leftEyeRegY.dat") << regY;
    }
}