#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class PP
{
public:
	PP(Size);
	void	CLAHE(Mat &src);
	void	motionIllu(Mat &src, Mat &flowfield, Mat &dis);
	void	Texture(Mat &src, Mat &dis);
	void	dirTexture(Mat &src, Mat &flowfield, Mat &dis);
	void	dirTexture_Polar(Mat &src, Mat &flowfield, Mat &dis);
	void	adaThresholding(Mat &src, Mat &mask, Mat &dis);
	void	ETF(Mat &flowfield, Mat &dis);
	void	Thresholding(Mat &src, Mat &dis);
	//void	Background(Mat &src, Mat &dis, Mat &oriImg);
	void	Colormapping(Mat &src, Mat &mask, Mat &oriImg, Mat &dis, int mode, bool isAdaThresholding);
	Mat		reduceGrayScale(Mat &src, int level);
	void	ShowColorMask(Mat &src);
	void	ReadTexture(string);

	Mat		texture;
	float	alpha;
	float	beta;
	bool	TextureLoaded;

private:
	void	SeedGradient(Mat &src, Mat &Gradient);
};