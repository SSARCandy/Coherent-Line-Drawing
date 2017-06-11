#include "CLD.h"

using namespace cv;

inline double gauss(double x, double mean, double sigma) {
	return (exp((-(x - mean)*(x - mean)) / (2 * sigma*sigma)) / sqrt(M_PI * 2.0 * sigma * sigma));
}

void MakeGaussianVector(double sigma, vector<double>& GAU) {
	const double threshold = 0.001;

	int i = 0;
	while (1) {
		i++;
		if (gauss((double)i, 0.0, sigma) < threshold)
			break;
	}
	GAU.clear();
	GAU.resize(i + 1);

	GAU[0] = gauss(0.0, 0.0, sigma);
	for (int j = 1; j < GAU.size(); j++) {
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
	rho = .99;
}

void CLD::readSrc(string file) {
	originalImg = imread(file, CV_LOAD_IMAGE_GRAYSCALE);
	//originalImg.convertTo(originalImg, CV_32FC1, 1.0);
	//imshow("lo;", originalImg);

	result = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_8UC1);
	etf.gen_ETF(file, originalImg.size());
}

void CLD::genCLD() {
	vector<double> g1, g2, g3;
	MakeGaussianVector(sigma1, g1);
	MakeGaussianVector(sigma2*SIGMA_RATIO, g2);

	int half_w1, half_w2, half_l;
	half_w1 = g1.size() - 1;
	half_w2 = g2.size() - 1;

	MakeGaussianVector(sigma2, g3);
	half_l = g3.size() - 1;

	Mat original_converted(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	for (int r = 0; r < originalImg.rows; r++) {
		for (int c = 0; c < originalImg.cols; c++) {
			float v = originalImg.at<uchar>(r, c) / 255.0;
			original_converted.at<float>(r, c) = v;
		}
	}

	Mat tmp(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	Mat dog(Size(originalImg.cols, originalImg.rows), CV_32FC1);

	genDDoG(original_converted, dog, g1, g2);
	genFDoG(dog, tmp, g3);

	for (int i = 0; i < originalImg.rows; i++) {
		for (int j = 0; j < originalImg.cols; j++) {
			float val = tmp.at<float>(i, j);
			int val2 = val * 255 - BIAS;
			result.at<uchar>(i, j) = max(val2, 0);// > 0 ? val2 - BIAS : 0;//< 255 - BINARIZATION_THRESHOLDING ? 0 : 255;
		}
	}
	normalize(result, result, 0, 255, NORM_MINMAX, CV_8UC1);

	//for (int i = 0; i < originalImg.rows; i++) {
	//	for (int j = 0; j < originalImg.cols; j++) {
	//		int val = result.at<uchar>(i, j);
	//		result.at<uchar>(i, j) = val > BINARIZATION_THRESHOLDING ? 255 : 0;// > 0 ? val2 - BIAS : 0;//< 255 - BINARIZATION_THRESHOLDING ? 0 : 255;
	//	}
	//}
}



/**
 * Private Functions
 */
void CLD::genFDoG(Mat& dog, Mat& tmp, vector<double>& g3) {
	vector<double> vt(2, 0);
	double x, y, d_x, d_y;
	double weight1, w_sum1, sum1;

	int i_x, i_y;
	int x1, y1;
	double val;

	int image_x = dog.rows;
	int image_y = dog.cols;

	int half_l = g3.size() - 1;

	int flow_DOG_sign = 0;

	for (int i = 0; i < image_x; i++) {
		for (int j = 0; j < image_y; j++) {
			sum1 = w_sum1 = weight1 = 0.0;

			val = dog.at<float>(i, j);
			weight1 = g3[0];
			sum1 = val * weight1;
			w_sum1 += weight1;

			d_x = (double)i;
			d_y = (double)j;
			i_x = i; 
			i_y = j;
			for (int k = 0; k < half_l; k++) {
				vt[0] = etf.flowField.at<Vec3f>(i_x, i_y)[0];
				vt[1] = etf.flowField.at<Vec3f>(i_x, i_y)[1];

				if (vt[0] == 0.0 && vt[1] == 0.0) break;

				x = d_x;
				y = d_y;

				if (x >(double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0)	break;

				x1 = min(max((int)round(x), 0), image_x - 1);
				y1 = min(max((int)round(y), 0), image_y - 1);

				val = dog.at<float>(x1, y1);

				weight1 = g3[k];

				sum1 += val * weight1;
				w_sum1 += weight1;

				d_x += vt[0] * STEPSIZE;
				d_y += vt[1] * STEPSIZE;

				i_x = round(d_x);
				i_y = round(d_y);

				if (d_x < 0 || d_x > image_x - 1 || d_y < 0 || d_y > image_y - 1) break;
			}

			d_x = (double)i;
			d_y = (double)j;
			i_x = i; 
			i_y = j;
			for (int k = 0; k < half_l; k++) {
				vt[0] = -etf.flowField.at<Vec3f>(i_x, i_y)[0];
				vt[1] = -etf.flowField.at<Vec3f>(i_x, i_y)[1];
				if (vt[0] == 0.0 && vt[1] == 0.0) break;

				x = d_x;
				y = d_y;

				if (x >(double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0)	break;

				x1 = min(max((int)round(x), 0), image_x - 1);
				y1 = min(max((int)round(y), 0), image_y - 1);

				val = dog.at<float>(x1, y1);

				weight1 = g3[k];

				sum1 += val * weight1;
				w_sum1 += weight1;

				d_x += vt[0] * STEPSIZE;
				d_y += vt[1] * STEPSIZE;

				i_x = round(d_x);
				i_y = round(d_y);

				if (d_x < 0 || d_x > image_x - 1 || d_y < 0 || d_y > image_y - 1) break;
			}

			sum1 /= w_sum1;


			tmp.at<float>(i, j) = (sum1 > 0) ? 1.0 : 1.0 + tanh(sum1);
		}
	}
}

void CLD::genDDoG(Mat& image, Mat& dog, vector<double>& g1, vector<double>& g2) {
	vector<double> vn(2, 0);
	int x1, y1;
	double x, y, d_x, d_y;
	double weight1, weight2, w_sum1, sum1, sum2, w_sum2;
	double val;

	int half_w1, half_w2;
	int image_x, image_y;

	half_w1 = g1.size() - 1;
	half_w2 = g2.size() - 1;

	image_x = originalImg.rows;
	image_y = originalImg.cols;

	for (int i = 0; i < image_x; i++) {
		for (int j = 0; j < image_y; j++) {
			sum1 = sum2 = 0.0;
			w_sum1 = w_sum2 = 0.0;
			weight1 = weight2 = 0.0;

			vn[0] = -etf.flowField.at<Vec3f>(i, j)[1];
			vn[1] = etf.flowField.at<Vec3f>(i, j)[0];

			if (vn[0] == 0.0 && vn[1] == 0.0) {
				sum1 = sum2 = 255.0;
				dog.at<double>(j, i) = sum1 - rho * sum2;
				continue;
			}

			d_x = i;
			d_y = j;
			for (int s = -half_w2; s <= half_w2; s++) {
				x = d_x + vn[0] * s;
				y = d_y + vn[1] * s;

				if (x > (double)image_x - 1 || x < 0.0 || y >(double)image_y - 1 || y < 0.0) continue;

				x1 = min(max((int)round(x), 0), image_x - 1);
				y1 = min(max((int)round(y), 0), image_y - 1);

				val = image.at<float>(x1, y1);

				int dd = abs(s);
				weight1 = (dd > half_w1) ? 0.0 : g1[dd];
				weight2 = g2[dd];

				sum1 += val * weight1;
				sum2 += val * weight2;
				w_sum1 += weight1;
				w_sum2 += weight2;
			}

			sum1 /= w_sum1;
			sum2 /= w_sum2;

			/**
			 * eq 7
			 */
			dog.at<float>(i, j) = sum1 - rho * sum2;
		}
	}

}
