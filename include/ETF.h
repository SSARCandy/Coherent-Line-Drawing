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
	void refine_ETF(int kernel);
	//void GVF();
	void rotateFlow(cv::Mat& src, cv::Mat& dst, float theta);

	cv::Mat GVF;         // gradient vector flow
	cv::Mat gradientMag; // Normalized gradient magnitude
	cv::Mat flowField;   // edge tangent flow
	cv::Mat refinedETF;  // ETF after refinement 
	cv::Mat RotationMat;

	int halfw;
	int smoothPasses;

private:
	void resizeMat(cv::Size);
	void computeNewVector(int x, int y, const int kernel);
	float computePhi(cv::Vec3f x, cv::Vec3f y);
	float computeWs(cv::Point2f x, cv::Point2f y, int r);
	float computeWm(float gradmag_x, float gradmag_y);
	float computeWd(cv::Vec3f x, cv::Vec3f y);

};