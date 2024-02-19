#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>

#include "constants.hpp"

cv::Mat floodKillEdges(cv::Mat& mat);

#pragma mark Visualization

#pragma mark Helpers
bool inMat(cv::Point p, int rows, int cols) {
    return p.x >= 0 && p.x < cols&& p.y >= 0 && p.y < rows;
}

double computeDynamicThreshold(const cv::Mat& mat, double stdDevFactor) {
    cv::Scalar stdMagnGrad, meanMagnGrad;
    cv::meanStdDev(mat, meanMagnGrad, stdMagnGrad);
    double stdDev = stdMagnGrad[0] / sqrt(mat.rows * mat.cols);
    return stdDevFactor * stdDev + meanMagnGrad[0];
}

cv::Mat matrixMagnitude(const cv::Mat& matX, const cv::Mat& matY) {
    cv::Mat mags(matX.rows, matX.cols, CV_64F);
    for (int y = 0; y < matX.rows; ++y) {
        const double* Xr = matX.ptr<double>(y), * Yr = matY.ptr<double>(y);
        double* Mr = mags.ptr<double>(y);
        for (int x = 0; x < matX.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));
            Mr[x] = magnitude;
        }
    }
    return mags;
}

cv::Point unscalePoint(cv::Point p, cv::Rect origSize) {
    float ratio = (((float)kFastEyeWidth) / origSize.width);
    int x = round(p.x / ratio);
    int y = round(p.y / ratio);
    return cv::Point(x, y);
}

void scaleToFastSize(const cv::Mat& src, cv::Mat& dst) {
    cv::resize(src, dst, cv::Size(kFastEyeWidth, (((float)kFastEyeWidth) / src.cols) * src.rows));
}

cv::Mat Gradient(const cv::Mat& mat) {
    cv::Mat out(mat.rows, mat.cols, CV_64F);

    for (int y = 0; y < mat.rows; ++y) {
        const uchar* Mr = mat.ptr<uchar>(y);
        double* Or = out.ptr<double>(y);

        Or[0] = Mr[1] - Mr[0];
        for (int x = 1; x < mat.cols - 1; ++x) {
            Or[x] = (Mr[x + 1] - Mr[x - 1]) / 2.0;
        }
        Or[mat.cols - 1] = Mr[mat.cols - 1] - Mr[mat.cols - 2];
    }

    return out;
}


void testPossibleCentersFormula(int x, int y, const cv::Mat& weight, double gx, double gy, cv::Mat& out) {
    // for all possible centers
    for (int cy = 0; cy < out.rows; ++cy) {
        double* Or = out.ptr<double>(cy);
        const unsigned char* Wr = weight.ptr<unsigned char>(cy);
        for (int cx = 0; cx < out.cols; ++cx) {
            if (x == cx && y == cy) {
                continue;
            }
            // create a vector from the possible center to the gradient origin
            double dx = x - cx;
            double dy = y - cy;
            // normalize d
            double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            double dotProduct = dx * gx + dy * gy;
            dotProduct = std::max(0.0, dotProduct);
            // square and multiply by the weight
            Or[cx] += dotProduct * dotProduct * Wr[cx];
        }
    }
}

cv::Point findEyeCenter(cv::Mat& face, cv::Rect eye) {
    cv::Mat eyes = face(eye);

    cv::cvtColor(eyes, eyes, cv::COLOR_BGR2GRAY);

    cv::Mat gradientX = Gradient(eyes);
    cv::Mat gradientY = Gradient(eyes.t()).t();
    //-- Normalize and threshold the gradient
    // compute all the magnitudes
    cv::Mat mags = matrixMagnitude(gradientX, gradientY);
    //compute the threshold
    double gradientThresh = computeDynamicThreshold(mags, kGradientThreshold);
    //double gradientThresh = kGradientThreshold;
    //double gradientThresh = 0;
    //normalize
    for (int y = 0; y < eyes.rows; ++y) {
        double* Xr = gradientX.ptr<double>(y), * Yr = gradientY.ptr<double>(y);
        const double* Mr = mags.ptr<double>(y);
        for (int x = 0; x < eyes.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = Mr[x];
            if (magnitude > gradientThresh) {
                Xr[x] = gX / magnitude;
                Yr[x] = gY / magnitude;
            }
            else {
                Xr[x] = 0.0;
                Yr[x] = 0.0;
            }
        }
    }
    //-- Create a blurred and inverted image for weighting
    cv::Mat weight;
    GaussianBlur(eyes, weight, cv::Size(kWeightBlurSize, kWeightBlurSize), 0, 0);
    for (int y = 0; y < weight.rows; ++y) {
        unsigned char* row = weight.ptr<unsigned char>(y);
        for (int x = 0; x < weight.cols; ++x) {
            row[x] = (255 - row[x]);
        }
    }
    //imshow(debugWindow,weight);
    //-- Run the algorithm!
    cv::Mat outSum = cv::Mat::zeros(eyes.rows, eyes.cols, CV_64F);
    // for each possible gradient location
    // Note: these loops are reversed from the way the paper does them
    // it evaluates every possible center for each gradient location instead of
    // every possible gradient location for every center.
    for (int y = 0; y < weight.rows; ++y) {
        const double* Xr = gradientX.ptr<double>(y), * Yr = gradientY.ptr<double>(y);
        for (int x = 0; x < weight.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            if (gX == 0.0 && gY == 0.0) {
                continue;
            }
            testPossibleCentersFormula(x, y, weight, gX, gY, outSum);
        }
    }
    // scale all the values down, basically averaging them
    double numGradients = (weight.rows * weight.cols);
    cv::Mat out;
    outSum.convertTo(out, CV_32F, 1.0 / numGradients);
    //imshow(debugWindow,out);
    //-- Find the maximum point
    cv::Point maxP;
    double maxVal;
    cv::minMaxLoc(out, NULL, &maxVal, NULL, &maxP);
    //-- Flood fill the edges
    if (kEnablePostProcess) {
        cv::Mat floodClone;
        //double floodThresh = computeDynamicThreshold(out, 1.5);
        double floodThresh = maxVal * kPostProcessThreshold;
        cv::threshold(out, floodClone, floodThresh, 0.0f, cv::THRESH_TOZERO);

        cv::Mat mask = floodKillEdges(floodClone);
        //imshow(debugWindow + " Mask",mask);
        //imshow(debugWindow,out);
        // redo max
        cv::minMaxLoc(out, NULL, &maxVal, NULL, &maxP, mask);
    }
    return maxP;
}

#pragma mark Postprocessing

bool floodShouldPushPoint(const cv::Point& np, const cv::Mat& mat) {
    return inMat(np, mat.rows, mat.cols);
}

// returns a mask
cv::Mat floodKillEdges(cv::Mat& mat) {
    rectangle(mat, cv::Rect(0, 0, mat.cols, mat.rows), 255);

    cv::Mat mask(mat.rows, mat.cols, CV_8U, 255);
    std::queue<cv::Point> toDo;
    toDo.push(cv::Point(0, 0));
    while (!toDo.empty()) {
        cv::Point p = toDo.front();
        toDo.pop();
        if (mat.at<float>(p) == 0.0f) {
            continue;
        }
        // add in every direction
        cv::Point np(p.x + 1, p.y); // right
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x - 1; np.y = p.y; // left
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x; np.y = p.y + 1; // down
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x; np.y = p.y - 1; // up
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        // kill it
        mat.at<float>(p) = 0.0f;
        mask.at<uchar>(p) = 0;
    }
    return mask;
}