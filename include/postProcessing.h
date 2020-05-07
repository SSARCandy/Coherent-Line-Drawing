#ifndef POST_PROCESSING_H_
#define POST_PROCESSING_H_

#include <opencv2/opencv.hpp>

namespace postprocess
{

cv::Mat visualizeFlowfield(const cv::Mat &flowfield);
cv::Mat visualizeETF(const cv::Mat &flowfield);
cv::Mat antiAlias(const cv::Mat &src);

} // namespace postprocess

#endif // POST_PROCESSING_H_
