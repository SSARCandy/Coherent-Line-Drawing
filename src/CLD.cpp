#include "CLD.h"


inline double gauss(double x, double mean, double sigma) {
	return (exp((-(x - mean)*(x - mean)) / (2 * sigma*sigma)) / sqrt(M_PI * 2.0 * sigma * sigma));
}

void MakeGaussianVector(double sigma, Vector<double>& GAU) {
	int i, j;

	double threshold = 0.001;

	i = 0;
	while (1) {
		i++;
		if (gauss((double)i, 0.0, sigma) < threshold)
			break;
	}
	GAU.clear();
	GAU.resize(i + 1);

	GAU[0] = gauss((double)0.0, 0.0, sigma);
	for (j = 1; j < GAU.size(); j++) {
		GAU[j] = gauss((double)j, 0.0, sigma);
	}
}

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

	sigma1 = .4;
	sigma2 = 3;
	tau = .99;
	black = 0;
}

void CLD::readSrc(string file) {
	originalImg = imread(file, CV_LOAD_IMAGE_GRAYSCALE);
	//originalImg.convertTo(originalImg, CV_32FC1, 1.0);
	imshow("lo;", originalImg);

	result = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_8UC1);
	etf.gen_ETF(file, originalImg.size());
}

void CLD::genCLD() {
	//GaussianBlur(originalImg, result, Size(91, 91), 0,0);

	Vector<double> g1, g2, g3;
	MakeGaussianVector(sigma1, g1);
	MakeGaussianVector(sigma2*SIGMA_RATIO, g2);

	int half_w1, half_w2, half_l;
	half_w1 = g1.size() - 1;
	half_w2 = g2.size() - 1;

	MakeGaussianVector(sigma2, g3);
	half_l = g3.size() - 1;

	Mat tmp(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	for (int r = 0; r < tmp.rows; r++) {
		for (int c = 0; c < tmp.cols; c++) {
			float v = originalImg.at<uchar>(r, c)/255.0;
			tmp.at<float>(r, c) = v;
		}
	}

	Mat tmp2(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	Mat dog(Size(originalImg.cols, originalImg.rows), CV_32FC1);

	genDDoG(tmp, dog, g1, g2);
	genFDoG(dog, tmp2, g3);

	for (int i = 0; i < originalImg.rows; i++) {
		for (int j = 0; j < originalImg.cols; j++) {
			float val = tmp2.at<float>(i, j);
			int val2 = val * 255;
			result.at<uchar>(i, j) = val2 < 255 - BINARIZATION_THRESHOLDING ? 0 : 255;
		}
	}
}




/**
 * Private Functions
 */

void CLD::genFDoG(Mat& dog, Mat& tmp, Vector<double>& g3) {
	Vector<double> vt(2, 0);
	double x, y, d_x, d_y;
	double weight1, w_sum1, sum1;

	int i_x, i_y, k;
	int x1, y1;
	double val;
	int i, j;

	int image_x = dog.rows;
	int image_y = dog.cols;

	int half_l;
	half_l = g3.size() -1;

	int flow_DOG_sign = 0;

	double step_size = 1.0;

	for (i = 0; i < image_x; i++) {
		for (j = 0; j < image_y; j++) {
			sum1 = 0.0;
			w_sum1 = 0.0;
			weight1 = 0.0;
			/////////////////////////////////
			val = dog.at<float>(i, j);
			weight1 = g3[0];
			sum1 = val * weight1;
			w_sum1 += weight1;
			////////////////////////////////////////////////
			d_x = (double)i; d_y = (double)j;
			i_x = i; i_y = j;
			////////////////////////////
			for (k = 0; k < half_l; k++) {
				vt[0] = etf.flowField.at<Vec3f>(i_x, i_y)[0];
				vt[1] = etf.flowField.at<Vec3f>(i_x, i_y)[1];
				if (vt[0] == 0.0 && vt[1] == 0.0) {
					break;
				}
				x = d_x;
				y = d_y;
				/////////////////////////////////////////////////////
				if (x >(double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0)
					break;
				x1 = round(x);	if (x1 < 0) x1 = 0; if (x1 > image_x - 1) x1 = image_x - 1;
				y1 = round(y);	if (y1 < 0) y1 = 0; if (y1 > image_y - 1) y1 = image_y - 1;
				val = dog.at<float>(x1, y1);
				//////////////////////////////
				weight1 = g3[k];
				////////////////////
				sum1 += val * weight1;
				w_sum1 += weight1;
				/////////////////////////////////////////
				d_x += vt[0] * step_size;
				d_y += vt[1] * step_size;
				/////////////////////////////////////////
				i_x = round(d_x);
				i_y = round(d_y);
				if (d_x < 0 || d_x > image_x - 1 || d_y < 0 || d_y > image_y - 1) break;
				/////////////////////////
			}
			////////////////////////////////////////////////
			d_x = (double)i; d_y = (double)j;
			i_x = i; i_y = j;
			for (k = 0; k < half_l; k++) {
				vt[0] = -etf.flowField.at<Vec3f>(i_x, i_y)[0];
				vt[1] = -etf.flowField.at<Vec3f>(i_x, i_y)[1];
				if (vt[0] == 0.0 && vt[1] == 0.0) {
					break;
				}
				x = d_x;
				y = d_y;
				/////////////////////////////////////////////////////
				if (x >(double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0)
					break;
				x1 = round(x);	if (x1 < 0) x1 = 0; if (x1 > image_x - 1) x1 = image_x - 1;
				y1 = round(y);	if (y1 < 0) y1 = 0; if (y1 > image_y - 1) y1 = image_y - 1;
				val = dog.at<float>(x1, y1);
				//////////////////////////////
				weight1 = g3[k];
				////////////////////
				sum1 += val * weight1;
				w_sum1 += weight1;
				/////////////////////////////////////////
				d_x += vt[0] * step_size;
				d_y += vt[1] * step_size;
				/////////////////////////////////////////
				i_x = round(d_x);
				i_y = round(d_y);
				if (d_x < 0 || d_x > image_x - 1 || d_y < 0 || d_y > image_y - 1) break;
				/////////////////////////
			}
			////////////////////////////////////////
			sum1 /= w_sum1;
			//////////////////////////////////////
			if (sum1 > 0) tmp.at<float>(i, j) = 1.0;
			else tmp.at<float>(i, j) = 1.0 + tanh(sum1);
		}
	}
}

void CLD::genDDoG(Mat& image, Mat& dog, Vector<double>& g1, Vector<double>& g2) {
	Vector<double> vn(2, 0);
	double x, y, d_x, d_y;
	double weight1, weight2, w_sum1, sum1, sum2, w_sum2;

	int s;
	int x1, y1;
	int i, j;
	int dd;
	double val;

	int half_w1, half_w2;

	half_w1 = g1.size() - 1;
	half_w2 = g2.size() - 1;

	int image_x, image_y;

	image_x = originalImg.rows;
	image_y = originalImg.cols;

	for (i = 0; i < image_x; i++) {
		for (j = 0; j < image_y; j++) {
			if (j> 213) {
				vn[0] = vn[0]; 
			}
			sum1 = sum2 = 0.0;
			w_sum1 = w_sum2 = 0.0;
			weight1 = weight2 = 0.0;

			vn[0] = -etf.flowField.at<Vec3f>(i, j)[1];
			vn[1] = etf.flowField.at<Vec3f>(i, j)[0];

			if (vn[0] == 0.0 && vn[1] == 0.0) {
				sum1 = 255.0;
				sum2 = 255.0;
				dog.at<double>(j, i) = sum1 - tau * sum2;
				continue;
			}
			d_x = i; d_y = j;
			////////////////////////////////////////
			for (s = -half_w2; s <= half_w2; s++) {
				////////////////////////
				x = d_x + vn[0] * s;
				y = d_y + vn[1] * s;
				/////////////////////////////////////////////////////
				if (x > (double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0)
					continue;
				x1 = round(x);	if (x1 < 0) x1 = 0; if (x1 > image_x - 1) x1 = image_x - 1;
				y1 = round(y);	if (y1 < 0) y1 = 0; if (y1 > image_y - 1) y1 = image_y - 1;
				//imshow("hyj",image);
				val = image.at<float>(x1, y1);
				/////////////////////////////////////////////////////////
				dd = abs(s);
				if (dd > half_w1) weight1 = 0.0;
				else weight1 = g1[dd];
				//////////////////////////////////
				sum1 += val * weight1;
				w_sum1 += weight1;
				/////////////////////////////////////////////////////
				weight2 = g2[dd];
				sum2 += val * weight2;
				w_sum2 += weight2;
			}
			/////////////////////////
			sum1 /= w_sum1;
			sum2 /= w_sum2;
			//////////////////////////////////////
			dog.at<float>(i, j) = sum1 - tau * sum2;
		}
	}

}
