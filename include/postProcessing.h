#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;

#define M_PI 3.14159265358979323846

class PP {
public:
	PP(cv::Size);
	void ETF(cv::Mat &flowfield, cv::Mat &dis);
	void FlowField(cv::Mat &flowfield, cv::Mat &dis);
	void AntiAlias(cv::Mat &src, cv::Mat &dis);
};