#define _USE_MATH_DEFINES

#include <opencv2/opencv.hpp>
#include "include/const.h"
#include "include/ETF.h"


ETF::ETF()
    : s{cv::Size(300, 300)}
    , flowField{cv::Mat::zeros(s, CV_32FC3)}
    , refinedETF{cv::Mat::zeros(s, CV_32FC3)}
    , gradientMag{cv::Mat::zeros(s, CV_32FC3)}
{
    ;
}

ETF::ETF(const cv::Size s)
    : s{s}
    , flowField{cv::Mat::zeros(s, CV_32FC3)}
    , refinedETF{cv::Mat::zeros(s, CV_32FC3)}
    , gradientMag{cv::Mat::zeros(s, CV_32FC3)}
{
    ;
}

/**
 * Generate initial ETF 
 * by taking perpendicular vectors(counter-clockwise) from gradient map
 */
void ETF::initial_ETF(const std::string file, const cv::Size s)
{
    // Resizing Mat
    cv::resize(flowField, flowField, s, 0, 0, CV_INTER_LINEAR);
    cv::resize(refinedETF, refinedETF, s, 0, 0, CV_INTER_LINEAR);
    cv::resize(gradientMag, gradientMag, s, 0, 0, CV_INTER_LINEAR);

    cv::Mat src = cv::imread(file, 1);
    cv::Mat src_n;
    cv::Mat grad;
    normalize(src, src_n, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC1);
    //GaussianBlur(src_n, src_n, cv::Size(51, 51), 0, 0);

    // Generate grad_x and grad_y
    cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
    Sobel(src_n, grad_x, CV_32FC1, 1, 0, 5);
    Sobel(src_n, grad_y, CV_32FC1, 0, 1, 5);

    //Compute gradient
    magnitude(grad_x, grad_y, gradientMag);
    normalize(gradientMag, gradientMag, 0.0, 1.0, cv::NORM_MINMAX);

    flowField = cv::Mat::zeros(src.size(), CV_32FC3);

#pragma omp parallel for
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            cv::Vec3f u = grad_x.at<cv::Vec3f>(i, j);
            cv::Vec3f v = grad_y.at<cv::Vec3f>(i, j);

            flowField.at<cv::Vec3f>(i, j) = normalize(cv::Vec3f(v.val[0], u.val[0], 0));
        }
    }

    flowField = rotate(flowField, 90);
}


void ETF::refine_ETF(int kernel)
{
#pragma omp parallel for
    for (int r = 0; r < flowField.rows; ++r) {
        for (int c = 0; c < flowField.cols; ++c) {
            refinedETF.at<cv::Vec3f>(r, c) = computeNewVector(c, r, kernel);
        }
    }

    flowField = refinedETF.clone();
}


cv::Mat ETF::rotate(const cv::Mat &src, const double degree)
{
    const double theta{degree / 180.0 * constant::PI};
    cv::Mat dst{cv::Mat::zeros(src.size(), CV_32FC3)};

    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            cv::Vec3f v    = src.at<cv::Vec3f>(i, j);
            const float rx = v[0] * cos(theta) - v[1] * sin(theta);
            const float ry = v[1] * cos(theta) + v[0] * sin(theta);

            dst.at<cv::Vec3f>(i, j) = cv::Vec3f(rx, ry, 0.0);
        }
    }

    return dst;
}

/*
 * Paper's Eq(1)
 */
cv::Vec3f ETF::computeNewVector(const int x, const int y, const int kernel)
{
    const cv::Vec3f t_cur_x = flowField.at<cv::Vec3f>(y, x);
    cv::Vec3f t_new         = cv::Vec3f(0, 0, 0);

    for (int r = y - kernel; r <= y + kernel; ++r) {
        for (int c = x - kernel; c <= x + kernel; ++c) {
            if (r < 0 || r >= refinedETF.rows || c < 0 || c >= refinedETF.cols) continue;

            const cv::Vec3f t_cur_y = flowField.at<cv::Vec3f>(r, c);

            const float phi = computePhi(t_cur_x, t_cur_y);
            const float w_s = computeWs(cv::Point2f(x, y), cv::Point2f(c, r), kernel);
            const float w_m =
                computeWm(cv::norm(gradientMag.at<cv::Vec3f>(y, x)), cv::norm(gradientMag.at<float>(r, c)));
            const float w_d = computeWd(t_cur_x, t_cur_y);
            t_new += phi * t_cur_y * w_s * w_m * w_d;
        }
    }
    return cv::normalize(t_new);
}

/*
 * Paper's Eq(5)
 */
inline float ETF::computePhi(const cv::Vec3f &x, const cv::Vec3f &y) { return x.dot(y) > 0 ? 1 : -1; }

/*
 * Paper's Eq(2)
 */
inline float ETF::computeWs(const cv::Point2f &x, const cv::Point2f &y, const int r) { return norm(x - y) < r ? 1 : 0; }

/*
 * Paper's Eq(3)
 */
inline float ETF::computeWm(const float gradmag_x, const float gradmag_y)
{
    const float wm = (1 + tanh(gradmag_y - gradmag_x)) / 2;
    return wm;
}

/*
 * Paper's Eq(4)
 */
inline float ETF::computeWd(const cv::Vec3f &x, const cv::Vec3f &y) { return abs(x.dot(y)); }
