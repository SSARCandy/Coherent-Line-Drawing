#include "ETF.h"
# define M_PI 3.14159265358979323846

ETF::ETF() {
	Size s(300, 300);

	Init(s);
}

ETF::ETF(Size s) {
	Init(s);
}

void ETF::Init(Size s) {
	flowField = Mat::zeros(s, CV_32FC3);

	halfw = 4;
	smoothPasses = 2;
}

void ETF::ReadFlow(string file, Size s) {
	FILE *stream = fopen(file.c_str(), "rb");
	if (!stream) {
		//std::cout<<"ERROR!! Can't read "<<featurefile<<'\n';
		return;
	}

	int vf_w, vf_h;
	int sint = sizeof(int);

	fread((void *)&(vf_w), sint, 1, stream);
	fread((void *)&(vf_h), sint, 1, stream);

	int sfloat = sizeof(float);
	float *data = new float[vf_w*vf_h * 2];
	fread((void *)(data), sfloat, vf_w*vf_h * 2, stream);

	resize(flowField, flowField, Size(vf_w, vf_h), 0, 0, CV_INTER_LINEAR);


	for (int j = 0; j < vf_h; j++) {
		for (int i = 0; i < vf_w; i++) {
			int index = j*vf_w + i;
			float dx = data[index * 2 + 1];
			float dy = data[index * 2];
			flowField.at<Vec3f>(vf_h - j - 1, i) = Vec3f(dx, -dy, 0.0); //x,y swap??

		}
	}
	resize(flowField, flowField, s, 0, 0, CV_INTER_LINEAR);
}

//Generate ETF of input image as flowfield
void ETF::gen_ETF(string file, Size s) {
	Mat src = imread(file, 1);
	Mat src_n;
	Mat grad;
	normalize(src, src_n, 0.0, 1.0, NORM_MINMAX, CV_32FC3);
	GaussianBlur(src_n, src_n, Size(71, 71), 0, 0);

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	/// Gradient X
	Sobel(src_n, grad_x, CV_32F, 1, 0, 3, 1, 0, 1);
	/// Gradient Y
	Sobel(src_n, grad_y, CV_32F, 0, 1, 3, 1, 0, 1);

	flowField = Mat::zeros(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			Vec3f u = grad_x.at<cv::Vec3f>(i, j) / 255.0; //-255~255
			Vec3f v = grad_y.at<cv::Vec3f>(i, j) / 255.0;

			float x = u.dot(u);
			float y = v.dot(v);
			float z = v.dot(u);
			float temp = y*y - 2.0*x*y + x*x + 4.0*z*z;
			float lambda1 = 0;
			lambda1 = 0.5 * (y + x + sqrt(temp));
			flowField.at<cv::Vec3f>(i, j) = normalize(Vec3f(z, x - lambda1, 0.0));
			//dis.at<cv::Vec3f>(i,j) = normalize( Vec3f(lambda1-x,z,0.0) );

			if (flowField.at<cv::Vec3f>(i, j) == Vec3f(0.0, 0.0, 0.0)) {
				flowField.at<cv::Vec3f>(i, j) = Vec3f(0.0, 1.0, 0.0);
			}
		}
	}

	resize(flowField, flowField, Size(src.cols, src.rows), 0, 0, CV_INTER_LINEAR);
}

//void ETF::GVF()
//{
//	//Mat tmp_n = Mat::zeros(Size(256, 256), CV_32FC3);
//
//	////Addition_B is CV_32F ,but we need CV_32FC3 to store vec3f
//	////create 3 same CV_32F channel and merge to create CV_32FC3 Mat
//	//vector<Mat> channels;
//	//Mat c = Mat::zeros(tmp_n.size(), CV_32F);
//	//Addition_B.convertTo(c, CV_32F, 255);
//	//channels.push_back(c);
//	//channels.push_back(c);
//	//channels.push_back(c);
//	//merge(channels, tmp_n);
//
//	//normalize(tmp_n, tmp_n, 0.0, 1.0, NORM_MINMAX, CV_32FC3);
//	//GaussianBlur(tmp_n, tmp_n, Size(91, 91), 0, 0);
//
//	///// Generate grad_x and grad_y
//	//Mat dX, dY;
//	//Sobel(tmp_n, dX, CV_32F, 1, 0, 3, 1, 0, 1);
//	//Sobel(tmp_n, dY, CV_32F, 0, 1, 3, 1, 0, 1);
//
//	//gvf = Mat::zeros(tmp_n.size(), CV_32FC3);
//	//for (int i = 0; i < tmp_n.rows; i++)
//	//{
//	//	for (int j = 0; j < tmp_n.cols; j++)
//	//	{
//	//		Vec3f u = dX.at<cv::Vec3f>(i, j) / 255.0; //-255~255
//	//		Vec3f v = dY.at<cv::Vec3f>(i, j) / 255.0;
//
//	//		float x = u.dot(u);
//	//		float y = v.dot(v);
//	//		float z = v.dot(u);
//	//		float temp = y*y - 2.0*x*y + x*x + 4.0*z*z;
//	//		float lambda1 = 0;
//	//		lambda1 = 0.5 * (y + x + sqrt(temp));
//	//		gvf.at<cv::Vec3f>(i, j) = normalize(Vec3f(z,x - lambda1, 0.0));
//
//	//		if (gvf.at<cv::Vec3f>(i, j) == Vec3f(0.0, 0.0, 0.0))
//	//		{
//	//			gvf.at<cv::Vec3f>(i, j) = Vec3f(0.0, 1.0, 0.0);
//	//		}
//	//	}
//	//}
//
//	//for (int i = 0; i < gvf.rows; i++)
//	//{
//	//	for (int j = 0; j < gvf.cols; j++)
//	//	{
//	//		Vec3f v = gvf.at<cv::Vec3f>(i, j);
//	//		gvf.at<cv::Vec3f>(i, j) = Vec3f(-v[1], v[0], 0.0);
//	//	}
//	//}
//	//resize(gvf, gvf, Mask.size(), 0, 0, CV_INTER_LINEAR);
//	//flowField = gvf.clone();
//
//}

void ETF::RotateFlow(float theta) {
	theta = theta / 180.0 * M_PI;
	for (int i = 0; i < flowField.rows; i++) {
		for (int j = 0; j < flowField.cols; j++) {
			Vec3f v = flowField.at<cv::Vec3f>(i, j);
			// x' = x*cos(Theta) - y*sin(Theta)
			// y' = y*cos(Theta) + x*sin(Theta)
			float rx = v[0] * cos(theta) - v[1] * sin(theta);
			float ry = v[1] * cos(theta) + v[0] * sin(theta);
			flowField.at<cv::Vec3f>(i, j) = Vec3f(rx, ry, 0.0);
		}
	}

}

