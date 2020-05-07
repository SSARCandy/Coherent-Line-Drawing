#ifndef CLD_H_
#define CLD_H_

#include <opencv2/opencv.hpp>
#include "ETF.h"

#define SIGMA_RATIO 1.6

class CLD
{
public:
    CLD();
    CLD(const cv::Size);
    void init(const cv::Size);
    void readSrc(const std::string);
    void genCLD();
    void combineImage();
    // eq.(10)
    void binaryThresholding(const cv::Mat &src, cv::Mat &dst, const double tau);

    cv::Mat originalImg;
    cv::Mat DoG;
    cv::Mat FDoG;
    cv::Mat result;
    ETF etf;

    double sigma_c;
    double sigma_m;
    double rho;
    double tau;

private:
    // Perform eq.(6) on each pixel
    void gradientDoG(const cv::Mat &src, cv::Mat &dst, const double rho, const double sigma_c);
    // Perform eq.(9) on each pixel
    void flowDoG(const cv::Mat &src, cv::Mat &dst, const double sigma_m);
};

#endif // !CLD_H_
