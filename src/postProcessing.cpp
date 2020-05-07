#include <opencv2/opencv.hpp>
#include "include/const.h"
#include "include/postProcessing.h"

namespace postprocess
{

// visualize the ETF
cv::Mat visualizeETF(const cv::Mat &flowfield)
{
    cv::Mat noise = cv::Mat::zeros(cv::Size(flowfield.cols / 2, flowfield.rows / 2), CV_32F);
    cv::Mat dst   = cv::Mat::zeros(flowfield.size(), CV_32F);
    cv::randu(noise, 0, 1.0f);
    cv::resize(noise, noise, flowfield.size(), 0, 0, cv::INTER_NEAREST);

    constexpr int s       = 10;
    constexpr float sigma = 2 * s * s;
    const int nRows       = noise.rows;
    const int nCols       = noise.cols;


#pragma omp parallel for
    for (int i = 0; i < nRows; ++i) {
        for (int j = 0; j < nCols; ++j) {
            float w_sum = 0.0;
            float x     = i;
            float y     = j;
            for (int k = 0; k < s; ++k) {
                cv::Vec3f v = cv::normalize(flowfield.at<cv::Vec3f>(int(x + nRows) % nRows, int(y + nCols) % nCols));
                if (v[0] != 0) x = x + (abs(v[0]) / float(abs(v[0]) + abs(v[1]))) * (abs(v[0]) / v[0]);
                if (v[1] != 0) y = y + (abs(v[1]) / float(abs(v[0]) + abs(v[1]))) * (abs(v[1]) / v[1]);
                const float r2 = k * k;
                const float w  = (1 / (constant::PI * sigma)) * exp(-(r2) / sigma);
                int xx         = (int(x) + nRows) % nRows;
                int yy         = (int(y) + nCols) % nCols;
                dst.at<float>(i, j) += w * noise.at<float>(xx, yy);
                w_sum += w;
            }

            x = i;
            y = j;
            for (int k = 0; k < s; ++k) {
                cv::Vec3f v = -cv::normalize(flowfield.at<cv::Vec3f>(int(x + nRows) % nRows, int(y + nCols) % nCols));
                if (v[0] != 0) x = x + (abs(v[0]) / float(abs(v[0]) + abs(v[1]))) * (abs(v[0]) / v[0]);
                if (v[1] != 0) y = y + (abs(v[1]) / float(abs(v[0]) + abs(v[1]))) * (abs(v[1]) / v[1]);

                const float r2 = k * k;
                const float w  = (1 / (constant::PI * sigma)) * exp(-(r2) / sigma);
                dst.at<float>(i, j) += w * noise.at<float>(int(x + nRows) % nRows, int(y + nCols) % nCols);
                w_sum += w;
            }
            dst.at<float>(i, j) /= w_sum;
        }
    }

    return dst;
}

// visualize ETF by drawing red arrowline
cv::Mat visualizeFlowfield(const cv::Mat &flowfield)
{
    constexpr int resolution = 10;
    cv::Mat dst{flowfield.size(), CV_32FC3, cv::Scalar(1, 1, 1)};

    for (int i = 0; i < dst.rows; i += resolution) {
        for (int j = 0; j < dst.cols; j += resolution) {
            const cv::Vec3f v = flowfield.at<cv::Vec3f>(i, j);
            const cv::Point2f p1(j, i);
            const cv::Point2f p2(j + v[1] * 5, i + v[0] * 5);

            cv::arrowedLine(dst, p1, p2, cv::Scalar(1, 0, 0), 1.5, 8, 0, 0.3);
        }
    }

    return dst;
}

cv::Mat antiAlias(const cv::Mat &src)
{
    constexpr int BLUR_SIZE = 3;
    cv::Mat dst{cv::Mat::zeros(src.size(), CV_32F)};

    cv::normalize(src, dst, 60, 255, cv::NORM_MINMAX);
    cv::GaussianBlur(dst, dst, cv::Size(BLUR_SIZE, BLUR_SIZE), 0, 0);
    return dst;
}

} // namespace postprocess