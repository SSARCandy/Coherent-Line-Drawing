#include "postProcessing.h"

PP::PP(Size s)
{
	texture = Mat::ones(s, CV_32FC3);  //why

	alpha = 0.2;
	beta = 0.5;

	float m_alpha = 0.0; //why
	float m_beta = 0.0;
	float t_alpha = 0.0;
	float t_beta = 0.0;

	TextureLoaded = false;
}

void PP::SeedGradient(Mat &src, Mat &Gradient)
{
	Mat sobel_x, sobel_y;
	Sobel(src, sobel_x, CV_32F, 1, 0, 3, 1, 0, 1);
	Sobel(src, sobel_y, CV_32F, 0, 1, 3, 1, 0, 1);
	vector<Mat> channel;
	channel.push_back(sobel_y);
	channel.push_back(sobel_x);
	channel.push_back(Mat::zeros(src.size(), CV_32F));
	merge(channel, Gradient);
}

void PP::ReadTexture(string file)
{
	texture = imread(file, 1);
	TextureLoaded = true;
}

void PP::ETF(Mat &flowfield, Mat &dis)
{
	const float M_PI = 3.14159265358979323846;
	Mat noise = Mat::zeros(cv::Size(flowfield.cols / 2, flowfield.rows / 2), CV_32F);
	dis = Mat::zeros(flowfield.size(), CV_32F);
	randu(noise, 0, 1.0f);
	resize(noise, noise, flowfield.size(), 0, 0, INTER_NEAREST);

	int s = 10;
	int nRows = noise.rows;
	int nCols = noise.cols;
	float sigma = 2 * s*s;
#pragma omp parallel for
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			float w_sum = 0.0;
			float x = i;
			float y = j;
			for (int k = 0; k < s; k++)
			{
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
			for (int k = 0; k < s; k++)
			{
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

//Contrast Limited Adaptive Histogram Equalization
void PP::CLAHE(Mat &src)
{
	Mat AHE;

	Ptr<cv::CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(2);
	clahe->setTilesGridSize(Size(8, 8));

	src.convertTo(src, CV_8UC1, 255);
	clahe->apply(src, AHE);
	AHE.convertTo(src, CV_32FC1, 1.0f / 255);
}

void PP::motionIllu(Mat &src, Mat &flowfield, Mat &dis)
{
	vector<Mat> channels;
	Mat Gradient = Mat::zeros(src.size(), CV_32FC3);
	SeedGradient(src, Gradient);
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

	// Create binary image from source image,
	cv::Mat bw;

	src.convertTo(bw, CV_8UC1, 255);
	cv::threshold(bw, bw, 255 * beta, 255, CV_THRESH_BINARY);
	//outward distance
	cv::Mat dist_outward;
	cv::distanceTransform(bw, dist_outward, CV_DIST_L2, 3);


	src.convertTo(bw, CV_8UC1, 255);
	cv::threshold(bw, bw, 255 * beta, 255, CV_THRESH_BINARY_INV);
	//inward distance
	cv::Mat dist_inward;
	cv::distanceTransform(bw, dist_inward, CV_DIST_L2, 3);

	// display distance field
	//cv::normalize(dist_outward, dist_outward, 0, 1., cv::NORM_MINMAX);
	//cv::normalize(dist_inward, dist_inward, 0, 1., cv::NORM_MINMAX);
	//for (int i = 0; i < src.rows; i++){
	//	for (int j = 0; j < src.cols; j++){
	//		float range = alpha / 2.0;
	//		float center = beta;
	//		//r.at<float>(i, j) = bw.at<uchar>(i, j);
	//		//g.at<float>(i, j) = bw.at<uchar>(i, j);
	//		//b.at<float>(i, j) = bw.at<uchar>(i, j);

	//		r.at<float>(i, j) = dist_outward.at<float>(i, j);
	//		g.at<float>(i, j) = 0;
	//		b.at<float>(i, j) = dist_inward.at<float>(i, j);

	//	}
	//}

#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float cos_theta = normalize(Gradient.at<Vec3f>(i, j)).dot(normalize(flowfield.at<Vec3f>(i, j)));
			float range = alpha / 2.0;
			float center = beta;

			if (src.at<float>(i, j) > center)
			{
				if (dist_outward.at<float>(i, j) < 20 * range)
				{
					if (cos_theta > 0)
					{
						r.at<float>(i, j) = 0.0;
						g.at<float>(i, j) = 0.0;
						b.at<float>(i, j) = 0.0;
					}
					else
					{
						r.at<float>(i, j) = 1.0;
						g.at<float>(i, j) = 1.0;
						b.at<float>(i, j) = 1.0;
					}
				}
				else
				{
					r.at<float>(i, j) = 1.0;
					g.at<float>(i, j) = 1.0;
					b.at<float>(i, j) = 0.0;
				}
			}
			else if (src.at<float>(i, j) <= center)
			{
				r.at<float>(i, j) = 0.0;
				g.at<float>(i, j) = 0.0;
				b.at<float>(i, j) = 1.0;
			}
		}
	}

	//#pragma omp parallel for
	//	for (int i = 0; i < src.rows; i++){
	//		for (int j = 0; j < src.cols; j++){
	//			float cos_theta = normalize(Gradient.at<Vec3f>(i, j)).dot(normalize(flowfield.at<Vec3f>(i, j)));
	//			float range = alpha / 2.0;
	//			float center = beta;
	//			if (src.at<float>(i, j) > center + range){
	//				r.at<float>(i, j) = 1.0;
	//				g.at<float>(i, j) = 1.0;
	//				b.at<float>(i, j) = 0.0;
	//			}
	//			else if (src.at<float>(i, j) < center - range){
	//				r.at<float>(i, j) = 0.0;
	//				g.at<float>(i, j) = 0.0;
	//				b.at<float>(i, j) = 1.0;
	//			}
	//			else if (cos_theta > 0){
	//				r.at<float>(i, j) = 0.0;
	//				g.at<float>(i, j) = 0.0;
	//				b.at<float>(i, j) = 0.0;
	//			}
	//			else {
	//				r.at<float>(i, j) = 1.0;
	//				g.at<float>(i, j) = 1.0;
	//				b.at<float>(i, j) = 1.0;
	//			}
	//		}
	//	}



	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

// Map density to color
// Y-aixs: density
void PP::Texture(Mat &src, Mat &dis)
{
	const float M_PI = 3.14159265358979323846;

	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float y = 0.5f;
			float x = src.at<float>(i, j);	  //density
			x = (alpha+0.5f)*(x+(beta-0.5f));
			x = max(min(x, 1.0f), 0.0f);

			r.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[2] / 255.0;
			b.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[0] / 255.0;
			g.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[1] / 255.0;
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

// X-aixs: angle between grident and flowvector
// Y-aixs: density
void PP::dirTexture(Mat &src, Mat &flowfield, Mat &dis)
{
	const float M_PI = 3.14159265358979323846;

	vector<Mat> channels;
	Mat Gradient = Mat::zeros(src.size(), CV_32FC3);
	SeedGradient(src, Gradient);
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float cos_theta = normalize(Gradient.at<Vec3f>(i, j)).dot(normalize(flowfield.at<Vec3f>(i, j)));

			float x = acos(cos_theta) / M_PI; //cos(theta) [-1, 1] map to [0, pi] to [0, 1]
			float y = src.at<float>(i, j);	  //density

			x = max(min(x, 1.0f), 0.0f);
			y = max(min(y, 1.0f), 0.0f);

			r.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[2] / 255.0;
			b.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[0] / 255.0;
			g.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[1] / 255.0;
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

// Polar coordinate (Angle, Radius)
void PP::dirTexture_Polar(Mat &src, Mat &flowfield, Mat &dis)
{
	const float M_PI = 3.14159265358979323846;

	vector<Mat> channels;
	Mat Gradient = Mat::zeros(src.size(), CV_32FC3);
	SeedGradient(src, Gradient);
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			//float shift_angle = theta0 / 360 * 2 * M_PI;
			float cos_theta = normalize(Gradient.at<Vec3f>(i, j)).dot(normalize(flowfield.at<Vec3f>(i, j)));
			float Radius = 1 - src.at<float>(i, j);	//density

			//use alpha, beta to adjust
			float x = 0.5 + alpha + beta*Radius*cos_theta;
			float y = 0.5 + alpha + beta*Radius*sin(acos(cos_theta));

			x = max(min(x, 1.0f), 0.0f);
			y = max(min(y, 1.0f), 0.0f);

			r.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[2] / 255.0;
			b.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[0] / 255.0;
			g.at<float>(i, j) = texture.at<Vec3b>(y*(texture.rows - 1), x*(texture.cols - 1))[1] / 255.0;
		}
	}

	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

void PP::Thresholding(Mat &src, Mat &dis)
{
	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);
#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float range = alpha / 2;
			float center = beta;
			if (src.at<float>(i, j) > center + range)
			{
				r.at<float>(i, j) = 1.0;
				g.at<float>(i, j) = 1.0;
				b.at<float>(i, j) = 1.0;
			}
			else if (src.at<float>(i, j) < center - range)
			{
				r.at<float>(i, j) = 0.0;
				g.at<float>(i, j) = 0.0;
				b.at<float>(i, j) = 0.0;
			}
			else
			{
				r.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
				g.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
				b.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
			}
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

void PP::adaThresholding(Mat &src, Mat &mask, Mat &dis)
{
	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);
#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float center = ((1 - (mask.at<float>(i, j)))*beta + (mask.at<float>(i, j))*alpha);
			float range = 0.05;
			if (src.at<float>(i, j) > center+range)
			{
				r.at<float>(i, j) = 1.0;
				g.at<float>(i, j) = 1.0;
				b.at<float>(i, j) = 1.0;
			}
			else if (src.at<float>(i, j) < center-range)
			{
				r.at<float>(i, j) = 0.0;
				g.at<float>(i, j) = 0.0;
				b.at<float>(i, j) = 0.0;
			}
			else
			{
				r.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
				g.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
				b.at<float>(i, j) = (src.at<float>(i, j) - (center - range)) / (range * 2);
			}
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};


void PP::Colormapping(Mat &src, Mat &mask, Mat &oriImg, Mat &dis, int mode, bool isAdaThresholding)
{
	//Mode :  1-half color bg, 2-shining, 3-inverse shining 
	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);
	//Mat a = Mat::zeros(src.size(), CV_32F);

	float a = 0.5;
#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float center, range;
			if (isAdaThresholding)
			{
				center = ((1 - (mask.at<float>(i, j)))*beta + (mask.at<float>(i, j))*alpha);
				range = 0;
			}
			else
			{
				range = alpha / 2;
				center = beta;
			}


			if (src.at<float>(i, j) > center + range)
			{
				if (mode == 1)//half color bg
				{
					// rgba to rgb  =  (1 - a) * bgcolor + a * fgcolor
					b.at<float>(i, j) = 0.5 + 0.5*(float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
					g.at<float>(i, j) = 0.5 + 0.5*(float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					r.at<float>(i, j) = 0.5 + 0.5*(float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
				}
				else if (mode == 2) //shining,
				{
					r.at<float>(i, j) = 1.0;
					g.at<float>(i, j) = 1.0;
					b.at<float>(i, j) = 1.0;
				}
				else if (mode == 3) // inverse shining
				{
					b.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
					g.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					r.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
				}
				else if (mode == 4) //Background
				{
					b.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
					g.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					r.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
				}
			}
			else if (src.at<float>(i, j) > center - range)
			{
				double a = 1 - (src.at<float>(i, j) - (center - range)) / (range * 2);

				if (mode == 1)
				{
					a = max(a, 0.5);
					r.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
					g.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					b.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
				}
				else if (mode==2)
				{
					r.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
					g.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					b.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
				}
				else if (mode == 3)
				{
					a = 1 - a;
					r.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
					g.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					b.at<float>(i, j) = (1 - a) + a*(float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
				}
				else if (mode == 4)
				{
					r.at<float>(i, j) = 0.0;
					g.at<float>(i, j) = 0.0;
					b.at<float>(i, j) = 0.0;
				}

			}
			else
			{
				if (mode == 1 || mode == 2)
				{
					b.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[0] / 255.0;
					g.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[1] / 255.0;
					r.at<float>(i, j) = (float)oriImg.at<cv::Vec3b>(i, j)[2] / 255.0;
				}
				else if (mode == 3)
				{
					r.at<float>(i, j) = 1.0;
					g.at<float>(i, j) = 1.0;
					b.at<float>(i, j) = 1.0;
				}
				else if (mode == 4)
				{
					r.at<float>(i, j) = 0.0;
					g.at<float>(i, j) = 0.0;
					b.at<float>(i, j) = 0.0;

				}
			}
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, dis);
};

void PP::ShowColorMask(Mat &src)
{
	const float M_PI = 3.14159265358979323846;
	Mat tmp = Mat::zeros(src.size(), CV_32F);

	vector<Mat> channels;
	Mat r = Mat::zeros(src.size(), CV_32F);
	Mat b = Mat::zeros(src.size(), CV_32F);
	Mat g = Mat::zeros(src.size(), CV_32F);

#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			float y = 0.5f;
			float x = src.at<float>(i, j);	  //density
			x = (alpha + 0.5f)*(x + (beta - 0.5f));
			x = max(min(x, 1.0f), 0.0f);

			r.at<float>(i, j) = x;
			b.at<float>(i, j) = x;
			g.at<float>(i, j) = x;
		}
	}
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, tmp);

	namedWindow("Size Mask", CV_WINDOW_AUTOSIZE);
	imshow("Size Mask", tmp);
};

//Unnfinish
Mat PP::reduceGrayScale(Mat &src, int level)
{
	src.convertTo(src, CV_8UC1);

	int thresholding = 256 / level;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			src.at<float>(i, j) = (float)(((int)(src.at<float>(i, j))*255 / thresholding)*thresholding)/255.0;
		}
	}
	imshow("kol", src);
	return src;
}
