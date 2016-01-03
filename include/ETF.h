#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class ETF {
public:
	ETF();
	ETF(Size);
	void Init(Size);
	//void operator=(const ETF &in);
	//void ReadSrc(string);
	void ReadFlow(string, Size);
	void gen_ETF(string, Size);
	//void GVF();
	void RotateFlow(float theta);

	Mat flowField;
	Mat RotationMat;

	int halfw;
	int smoothPasses;

};