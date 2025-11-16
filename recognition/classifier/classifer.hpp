#ifndef CLASSIFER_HPP
#define CLASSIFER_HPP

#include <opencv2/opencv.hpp>
#include <utils/circut.hpp>

class Classifier {
    virtual std::vector<Color> classiColors(const cv::Mat& image) = 0;
};

#endif // CLASSIFER_HPP