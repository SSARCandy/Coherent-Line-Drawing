#include "CLD.h"

CLD::CLD() {
	Size s(300, 300);

	init(s);
}

CLD::CLD(Size s) {
	init(s);
}

void CLD::init(Size s) {
	originalImg = Mat::zeros(s, CV_8UC1);
	result = Mat::zeros(s, CV_8UC1);
	etf.Init(s);

	halfw = 4;
	smoothPasses = 2;
	sigma1 = .4;
	sigma2 = 3;
	tau = .99;
	black = 0;
}

void CLD::readSrc(string file) {
	originalImg = imread(file, CV_LOAD_IMAGE_GRAYSCALE);
	//imshow("lo;", originalImg);
	etf.gen_ETF(file, originalImg.size());
}

void CLD::genFDOG() {

}




/**
 * Utility Functions
 */
