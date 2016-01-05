#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "ETF.h"


using namespace cv;
using namespace std;

#define M_PI 3.14159265358979323846
#define SIGMA_RATIO 1.6 
#define BINARIZATION_THRESHOLDING 200
#define BIAS 200
#define STEPSIZE 1.0

class CLD
{
public:
	CLD();
	CLD(Size);
	void init(Size);
	void readSrc(string);
	void genCLD();
	//void genFDOG();



	Mat originalImg;
	Mat result;
	ETF etf;

	double sigma1;
	double sigma2;
	double rho;

private:
	void genFDoG(Mat&, Mat&, Vector<double>&);
	void genDDoG(Mat&, Mat&, Vector<double>&, Vector<double>&);
};