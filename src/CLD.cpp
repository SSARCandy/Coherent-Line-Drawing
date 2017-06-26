#include "CLD.h"

using namespace cv;

// Eq.(8)
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
	DoG = Mat::zeros(s, CV_32FC1);
	FDoG = Mat::zeros(s, CV_32FC1);

	etf.Init(s);

	sigma_m = 3.0;
	sigma_c = 1.0;
	rho = 0.997;
	tau = 0.8;
}

void CLD::readSrc(string file) {
	originalImg = imread(file, CV_LOAD_IMAGE_GRAYSCALE);

	result = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_8UC1);
	DoG = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	FDoG = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_32FC1);

	etf.initial_ETF(file, originalImg.size());
	//genCLD();
}



void CLD::genCLD() {
	Mat originalImg_32FC1 = Mat::zeros(Size(originalImg.cols, originalImg.rows), CV_32FC1);
	originalImg.convertTo(originalImg_32FC1, CV_32FC1, 1.0 / 255.0);

	gradientDoG(originalImg_32FC1, DoG, this->rho, this->sigma_c);
	flowDoG(DoG, FDoG, this->sigma_m);
	//flowDoG(DoG, FDoG, this->sigma_m);
	
	binaryThresholding(FDoG, result, this->tau);
}

/**
 * Flow-based DoG filtering
 */
void CLD::flowDoG(Mat & src, Mat & dst, const double sigma_m) {
	vector<double> gau_m;
	MakeGaussianVector(sigma_m, gau_m);

	const int img_w = src.cols;
	const int img_h = src.rows;
	const int kernel_half = gau_m.size() - 1;

#pragma omp parallel for
	for (int y = 0; y < img_h; y++) {
		for (int x = 0; x < img_w; x++) {
			double gau_m_acc = -gau_m[0] * src.at<float>(y, x);
			double gau_m_weight_acc = -gau_m[0];
				
			// Intergral alone ETF
			Point2f pos(x, y);
			for (int step = 0; step < kernel_half; step++) {
				Vec3f tmp = etf.flowField.at<Vec3f>((int)round(pos.y), (int)round(pos.x));
				Point2f direction = Point2f(tmp[1], tmp[0]);

				if (direction.x == 0 && direction.y == 0) break;
				if (pos.x > (double)img_w - 1 || pos.x < 0.0 || pos.y >(double)img_h - 1 || pos.y < 0.0) break;

				float value = src.at<float>((int)round(pos.y), (int)round(pos.x));
				float weight = gau_m[step];

				gau_m_acc += value*weight;
				gau_m_weight_acc += weight;

				// move alone ETF direction 
				pos += direction;
				
				if ((int)round(pos.x) < 0 || (int)round(pos.x) > img_w - 1 || (int)round(pos.y) < 0 || (int)round(pos.y) > img_h - 1) break;
			}

			// Intergral alone inverse ETF
			pos = Point2f(x, y);
			for (int step = 0; step < kernel_half; step++) {
				Vec3f tmp = -etf.flowField.at<Vec3f>((int)round(pos.y), (int)round(pos.x));
				Point2f direction = Point2f(tmp[1], tmp[0]);

				if (direction.x == 0 && direction.y == 0) break;
				if (pos.x >(double)img_w - 1 || pos.x < 0.0 || pos.y >(double)img_h - 1 || pos.y < 0.0) break;

				float value = src.at<float>((int)round(pos.y), (int)round(pos.x));
				float weight = gau_m[step];

				gau_m_acc += value*weight;
				gau_m_weight_acc += weight;

				// move alone ETF direction 
				pos += direction;

				if ((int)round(pos.x) < 0 || (int)round(pos.x) > img_w - 1 || (int)round(pos.y) < 0 || (int)round(pos.y) > img_h - 1) break;
			}

			
			dst.at<float>(y, x) = (gau_m_acc / gau_m_weight_acc) > 0 ? 1.0 : 1 + tanh(gau_m_acc / gau_m_weight_acc);
		}
	}

	normalize(dst, dst, 0, 1, NORM_MINMAX);
}

void CLD::gradientDoG(Mat & src, Mat & dst, const double rho, const double sigma_c) {
	const double sigma_s = SIGMA_RATIO*sigma_c;
	vector<double> gau_c, gau_s;
	MakeGaussianVector(sigma_c, gau_c);
	MakeGaussianVector(sigma_s, gau_s);

	const int kernel = gau_s.size() - 1;

#pragma omp parallel for
	for (int y = 0; y < dst.rows; y++) {
		for (int x = 0; x < dst.cols; x++) {
			double gau_c_acc = 0;
			double gau_s_acc = 0;
			double gau_c_weight_acc = 0;
			double gau_s_weight_acc = 0;
			Vec3f tmp = etf.flowField.at<Vec3f>(y, x);
			Point2f gradient = Point2f(-tmp[0], tmp[1]);

			if (gradient.x == 0 && gradient.y == 0) continue;
			
			for (int step = -kernel; step <= kernel; step++) {
				double row = y + gradient.y * step;
				double col = x + gradient.x * step;

				if (col > (double)dst.cols - 1 || col < 0.0 || row > (double)dst.rows - 1 || row < 0.0) continue;

				float value = src.at<float>((int)round(row), (int)round(col));

				int gau_idx = abs(step);
				double gau_c_weight = (gau_idx >= gau_c.size()) ? 0.0 : gau_c[gau_idx];
				double gau_s_weight = gau_s[gau_idx];

				gau_c_acc += value * gau_c_weight;
				gau_s_acc += value * gau_s_weight;
				gau_c_weight_acc += gau_c_weight;
				gau_s_weight_acc += gau_s_weight;
			}

			double v_c = gau_c_acc / gau_c_weight_acc;
			double v_s = gau_s_acc / gau_s_weight_acc;
			dst.at<float>(y, x) = v_c - rho*v_s;
		}
	}
}

void CLD::binaryThresholding(Mat & src, Mat & dst, const double tau) {
#pragma omp parallel for
	for (int y = 0; y < dst.rows; y++) {
		for (int x = 0; x < dst.cols; x++) {
			float H = src.at<float>(y, x);
			int v = H < tau ? 0 : 255;

			dst.at<uchar>(y, x) = v;
		}
	}
}

/**
 * re-initialize the filter input 
 * by superimposing the black edge pixels of the previous binary output upon the original image
 */
void CLD::combineImage() {
#pragma omp parallel for
	for (int y = 0; y < originalImg.rows; y++) {
		for (int x = 0; x < originalImg.cols; x++) {
			float H = result.at<uchar>(y, x);
			
			if (H == 0) {
				originalImg.at<uchar>(y, x) = 0;
			}
		}
	}

	// Blur a little-bit to let image more smooth
	GaussianBlur(originalImg, originalImg, Size(3, 3), 0, 0);
}
