#include "postProcessing.h"

PP::PP(Size s) {
	thresholding = 0.2;
	//beta = 0.5;
}

void PP::ETF(Mat &flowfield, Mat &dis) {
	const float M_PI = 3.14159265358979323846;
	Mat noise = Mat::zeros(cv::Size(flowfield.cols / 2, flowfield.rows / 2), CV_32F);
	dis = Mat::zeros(flowfield.size(), CV_32F);
	randu(noise, 0, 1.0f);
	resize(noise, noise, flowfield.size(), 0, 0, INTER_NEAREST);

	int s = 10;
	int nRows = noise.rows;
	int nCols = noise.cols;
	float sigma = 2 * s*s;

	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			float w_sum = 0.0;
			float x = i;
			float y = j;
			for (int k = 0; k < s; k++) {
				Vec3f v = normalize(flowfield.at<Vec3f>(int(x + nRows) % nRows, int(y + nCols) % nCols));
				if (v[0] != 0) x = x + (abs(v[0]) / float(abs(v[0]) + abs(v[1])))*(abs(v[0]) / v[0]);
				if (v[1] != 0) y = y + (abs(v[1]) / float(abs(v[0]) + abs(v[1])))*(abs(v[1]) / v[1]);
				float r2 = k*k;
				float w = (1 / (M_PI*sigma))*exp(-(r2) / sigma);
				int xx = (int(x) + nRows) % nRows;
				int yy = (int(y) + nCols) % nCols;
				dis.at<float>(i, j) += w*noise.at<float>(xx, yy);
				w_sum += w;
			}

			x = i;
			y = j;
			for (int k = 0; k < s; k++) {
				Vec3f v = -normalize(flowfield.at<Vec3f>(int(x + nRows) % nRows, int(y + nCols) % nCols));
				if (v[0] != 0) x = x + (abs(v[0]) / float(abs(v[0]) + abs(v[1])))*(abs(v[0]) / v[0]);
				if (v[1] != 0) y = y + (abs(v[1]) / float(abs(v[0]) + abs(v[1])))*(abs(v[1]) / v[1]);

				float r2 = k*k;
				float w = (1 / (M_PI*sigma))*exp(-(r2) / sigma);
				dis.at<float>(i, j) += w*noise.at<float>(int(x + nRows) % nRows, int(y + nCols) % nCols);
				w_sum += w;
			}
			dis.at<float>(i, j) /= w_sum;
		}
	}
}

void PP::Thresholding(Mat &src, Mat &dis) {
	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<uchar>(i, j) > thresholding * 255) {
				r.at<float>(i, j) = 1.0;
				g.at<float>(i, j) = 1.0;
				b.at<float>(i, j) = 1.0;
			}
			else {
				r.at<float>(i, j) = 0.0;
				g.at<float>(i, j) = 0.0;
				b.at<float>(i, j) = 0.0;
			}
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};