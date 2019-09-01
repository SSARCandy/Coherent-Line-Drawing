#ifndef POST_PROCESSING_H_
#define POST_PROCESSING_H_

#include <stdio.h>
#include <opencv2/opencv.hpp>

#define M_PI 3.14159265358979323846

class PP
{
public:
    PP(cv::Size);
    void ETF(cv::Mat &flowfield, cv::Mat &dis);
    void FlowField(cv::Mat &flowfield, cv::Mat &dis);
    void AntiAlias(cv::Mat &src, cv::Mat &dis);
};

#endif // POST_PROCESSING_H_
