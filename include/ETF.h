#ifndef ETF_H_
#define ETF_H_

#include <opencv2/opencv.hpp>

class ETF
{
public:
    ETF();
    ETF(const cv::Size);
    void initial_ETF(const std::string, const cv::Size);
    void refine_ETF(const int kernel);

    cv::Size s;
    cv::Mat gradientMag; // Normalized gradient magnitude
    cv::Mat flowField;   // edge tangent flow
    cv::Mat refinedETF;  // ETF after refinement

private:
    cv::Mat rotate(const cv::Mat &src, const double degree);
    cv::Vec3f computeNewVector(const int x, const int y, const int kernel);
    float computePhi(const cv::Vec3f &x, const cv::Vec3f &y);
    float computeWs(const cv::Point2f &x, const cv::Point2f &y, const int r);
    float computeWm(const float gradmag_x, const float gradmag_y);
    float computeWd(const cv::Vec3f &x, const cv::Vec3f &y);
};

#endif // ETF_H_