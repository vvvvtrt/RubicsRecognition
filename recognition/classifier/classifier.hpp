#ifndef CLASSIFER_HPP
#define CLASSIFER_HPP

#include <opencv2/opencv.hpp>
#include <utils/circut.hpp>
#include <vector>

class Classifier {
public:
    virtual std::vector<cv::Vec3b>classifierColors(const std::vector<cv::Mat>& squareImages, int k) = 0;
};

class KMeansClassifier : public Classifier {
public:
    std::vector<cv::Vec3b> classifierColors(const std::vector<cv::Mat>& squareImages, int k) override;
};

#endif // CLASSIFER_HPP