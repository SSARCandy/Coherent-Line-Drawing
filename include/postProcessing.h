#ifndef POST_PROCESSING_H_
#define POST_PROCESSING_H_

#define _USE_MATH_DEFINES

#include <cmath>
#include <opencv2/opencv.hpp>

class PP
{
public:
    PP(cv::Size);
    void ETF(cv::Mat &flowfield, cv::Mat &dis);
    void FlowField(cv::Mat &flowfield, cv::Mat &dis);
    void AntiAlias(cv::Mat &src, cv::Mat &dis);
};

#endif // POST_PROCESSING_H_
