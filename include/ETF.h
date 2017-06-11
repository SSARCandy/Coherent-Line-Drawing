#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;

class ETF {
public:
	ETF();
	ETF(cv::Size);
	void Init(cv::Size);
	//void operator=(const ETF &in);
	//void ReadSrc(string);
	void ReadFlow(string, cv::Size);
	void gen_ETF(string, cv::Size);
	//void GVF();
	void RotateFlow(float theta);

	cv::Mat flowField;
	cv::Mat RotationMat;

	int halfw;
	int smoothPasses;

};