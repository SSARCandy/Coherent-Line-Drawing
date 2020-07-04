#include <tuple>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include "include/const.h"
#include "include/CLD.h"

// Eq.(8)
inline double gauss(double x, double mean, double sigma)
{
    return (exp((-(x - mean) * (x - mean)) / (2 * sigma * sigma)) / sqrt(constant::PI * 2.0 * sigma * sigma));
}

void MakeGaussianVector(double sigma, std::vector<double> &GAU)
{
    constexpr double threshold = 0.001;

    int i = 0;
    while (gauss(static_cast<double>(i), 0.0, sigma) >= threshold) {
        ++i;
    }
    GAU.clear();
    GAU.resize(i + 1);

    GAU[0] = gauss(0.0, 0.0, sigma);
    for (int j = 1; j < GAU.size(); ++j) {
        GAU[j] = gauss(static_cast<double>(j), 0.0, sigma);
    }
}

CLD::CLD()
{
    cv::Size s(300, 300);
    init(s);
}

CLD::CLD(const cv::Size s) { init(s); }

void CLD::init(const cv::Size s)
{
    originalImg = cv::Mat::zeros(s, CV_8UC1);
    result      = cv::Mat::zeros(s, CV_8UC1);
    DoG         = cv::Mat::zeros(s, CV_32FC1);
    FDoG        = cv::Mat::zeros(s, CV_32FC1);
    etf         = ETF(s);

    sigma_m = 3.0;
    sigma_c = 1.0;
    rho     = 0.997;
    tau     = 0.8;
}

void CLD::readSrc(const std::string file)
{
    originalImg = cv::imread(file, CV_LOAD_IMAGE_GRAYSCALE);

    result = cv::Mat::zeros(cv::Size(originalImg.cols, originalImg.rows), CV_8UC1);
    DoG    = cv::Mat::zeros(cv::Size(originalImg.cols, originalImg.rows), CV_32FC1);
    FDoG   = cv::Mat::zeros(cv::Size(originalImg.cols, originalImg.rows), CV_32FC1);

    etf.initial_ETF(file, originalImg.size());
}


void CLD::genCLD()
{
    cv::Mat originalImg_32FC1 = cv::Mat::zeros(cv::Size(originalImg.cols, originalImg.rows), CV_32FC1);
    originalImg.convertTo(originalImg_32FC1, CV_32FC1, 1.0 / 255.0);

    gradientDoG(originalImg_32FC1, DoG, this->rho, this->sigma_c);
    flowDoG(DoG, FDoG, this->sigma_m);

    binaryThresholding(FDoG, result, this->tau);
}

/**
 * Flow-based DoG filtering
 */
void CLD::flowDoG(const cv::Mat &src, cv::Mat &dst, const double sigma_m)
{
    std::vector<double> gau_m;
    MakeGaussianVector(sigma_m, gau_m);

    const int img_w       = src.cols;
    const int img_h       = src.rows;
    const int kernel_half = gau_m.size() - 1;

#pragma omp parallel for
    for (int y = 0; y < img_h; ++y) {
        for (int x = 0; x < img_w; ++x) {
            double gau_m_acc        = -gau_m[0] * src.at<float>(y, x);
            double gau_m_weight_acc = -gau_m[0];

            // Intergral alone ETF
            for (auto [step, pos] = std::tuple{0, cv::Point2f(x, y)}; step < kernel_half; ++step) {
                if (pos.x < 0. || pos.x > static_cast<float>(img_w - 1) || pos.y < 0. ||
                    pos.y > static_cast<float>(img_h - 1))
                    break;

                const int px          = static_cast<int>(round(pos.x));
                const int py          = static_cast<int>(round(pos.y));
                cv::Vec3f tmp         = etf.flowField.at<cv::Vec3f>(py, px);
                cv::Point2f direction = cv::Point2f(tmp[1], tmp[0]);

                if (direction.x == 0 && direction.y == 0) break;

                float value  = src.at<float>(py, px);
                float weight = gau_m[step];

                gau_m_acc += value * weight;
                gau_m_weight_acc += weight;

                // move alone ETF direction
                pos += direction;
            }

            // Intergral alone inverse ETF
            for (auto [step, pos] = std::tuple{0, cv::Point2f(x, y)}; step < kernel_half; ++step) {
                if (pos.x < 0. || pos.x > static_cast<float>(img_w - 1) || pos.y < 0. ||
                    pos.y > static_cast<float>(img_h - 1))
                    break;

                const int px          = static_cast<int>(round(pos.x));
                const int py          = static_cast<int>(round(pos.y));
                cv::Vec3f tmp         = -etf.flowField.at<cv::Vec3f>(py, px);
                cv::Point2f direction = cv::Point2f(tmp[1], tmp[0]);

                if (direction.x == 0 && direction.y == 0) break;

                float value  = src.at<float>(py, px);
                float weight = gau_m[step];

                gau_m_acc += value * weight;
                gau_m_weight_acc += weight;

                // move alone ETF direction
                pos += direction;
            }

            dst.at<float>(y, x) = (gau_m_acc / gau_m_weight_acc) > 0 ? 1.0 : 1 + tanh(gau_m_acc / gau_m_weight_acc);
        }
    }

    cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
}

void CLD::gradientDoG(const cv::Mat &src, cv::Mat &dst, const double rho, const double sigma_c)
{
    const double sigma_s = SIGMA_RATIO * sigma_c;
    std::vector<double> gau_c, gau_s;
    MakeGaussianVector(sigma_c, gau_c);
    MakeGaussianVector(sigma_s, gau_s);

    const int kernel = gau_s.size() - 1;

#pragma omp parallel for
    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            double gau_c_acc        = 0;
            double gau_s_acc        = 0;
            double gau_c_weight_acc = 0;
            double gau_s_weight_acc = 0;
            cv::Vec3f tmp           = etf.flowField.at<cv::Vec3f>(y, x);
            cv::Point2f gradient    = cv::Point2f(-tmp[0], tmp[1]);

            if (gradient.x == 0 && gradient.y == 0) continue;

            for (int step = -kernel; step <= kernel; ++step) {
                const double row = y + gradient.y * step;
                const double col = x + gradient.x * step;

                if (col > dst.cols - 1 || col < 0.0 || row > dst.rows - 1 || row < 0.0) continue;

                const float value = src.at<float>((int)round(row), (int)round(col));

                const int gau_idx         = abs(step);
                const double gau_c_weight = gau_c[gau_idx];
                const double gau_s_weight = gau_s[gau_idx];

                gau_c_acc += value * gau_c_weight;
                gau_s_acc += value * gau_s_weight;
                gau_c_weight_acc += gau_c_weight;
                gau_s_weight_acc += gau_s_weight;
            }

            double v_c          = gau_c_acc / gau_c_weight_acc;
            double v_s          = gau_s_acc / gau_s_weight_acc;
            dst.at<float>(y, x) = v_c - rho * v_s;
        }
    }
}

void CLD::binaryThresholding(const cv::Mat &src, cv::Mat &dst, const double tau)
{
    const std::unordered_map<bool, int> binary2color{{true, 0}, {false, 255}};
#pragma omp parallel for
    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            dst.at<uchar>(y, x) = binary2color.at(src.at<float>(y, x) < tau);
        }
    }
}

/**
 * re-initialize the filter input 
 * by superimposing the black edge pixels of the previous binary output upon the original image
 */
void CLD::combineImage()
{
#pragma omp parallel for
    for (int y = 0; y < originalImg.rows; ++y) {
        for (int x = 0; x < originalImg.cols; ++x) {
            const float H = result.at<uchar>(y, x);

            if (H == 0) {
                originalImg.at<uchar>(y, x) = 0;
            }
        }
    }

    // Blur a little-bit to let image more smooth
    cv::GaussianBlur(originalImg, originalImg, cv::Size(3, 3), 0, 0);
}
