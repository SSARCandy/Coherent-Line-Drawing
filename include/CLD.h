#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "ETF.h"


using namespace cv;
using namespace std;

class CLD
{
public:
	CLD();
	CLD(Size);
	void init(Size);
	void readSrc(string);
	void genFDOG();

	// missing setter/getter functions


	Mat originalImg;
	Mat result;
	ETF etf;

	int halfw;
	int smoothPasses;
	double sigma1;
	double sigma2;
	double tau;
	int black;
};