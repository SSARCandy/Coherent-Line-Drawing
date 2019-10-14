#ifndef POST_PROCESSING_H_
#define POST_PROCESSING_H_

#define _USE_MATH_DEFINES

#include <cmath>
#include <opencv2/opencv.hpp>

class PP
{
public:
    PP();
    void visualizeFlowfield(const cv::Mat &flowfield, cv::Mat &dst);
    cv::Mat visualizeETF(const cv::Mat &flowfield);
    cv::Mat antiAlias(const cv::Mat &src);
};

#endif // POST_PROCESSING_H_
