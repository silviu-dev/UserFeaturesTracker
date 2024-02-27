#include <opencv2/features2d.hpp>

typedef dlib::matrix<double, 1, 1> sample_type;
typedef dlib::radial_basis_kernel<sample_type> kernel_type;

void calculateCoeff(std::vector<cv::Point>& X, std::vector<cv::Point>& Y, bool rightEye);