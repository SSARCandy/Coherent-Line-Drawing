#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class PP
{
public:
	PP(Size);
	void	ETF(Mat &flowfield, Mat &dis);
	void	Thresholding(Mat &src, Mat &dis);

	float	thresholding;
};