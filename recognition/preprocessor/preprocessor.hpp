#ifndef PREROPCESSOR_HPP
#define PREROPCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>


class ImageProcessor {
    virtual cv::Mat process(const cv::Mat& image) = 0;
};


class DilateProcessor : public ImageProcessor {
public:
    cv::Mat process(const cv::Mat& image) override ;
};


cv::Vec3b getMeanColor(const cv::Mat& square);

#endif // PREROPCESSOR_HPP