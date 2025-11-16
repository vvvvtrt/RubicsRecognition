#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <utils/circut.hpp>

class Detector {
public:
    virtual ~Detector() = default;
    virtual std::vector<Circuit> detect(const cv::Mat& image) = 0;
};


class ContourDetector : public Detector {
public:
    std::vector<Circuit> detect(const cv::Mat& image) override;
};


#endif // DETECTOR_HPP