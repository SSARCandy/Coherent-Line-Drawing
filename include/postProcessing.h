#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;

class PP
{
public:
	PP(cv::Size);
	void	ETF(cv::Mat &flowfield, cv::Mat &dis);
	void	Thresholding(cv::Mat &src, cv::Mat &dis);

	float	thresholding;
};