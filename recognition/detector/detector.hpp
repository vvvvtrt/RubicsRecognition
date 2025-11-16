#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <utils/circut.hpp>
#include <vector>

class Detector {
public:
    virtual ~Detector() = default;
    virtual std::vector<Circuit> detect(const cv::Mat& image) = 0;
};


class ContourDetector : public Detector {
public:
    std::vector<Circuit> detect(const cv::Mat& image) override;
};


class SquareSearcher {
public:
    std::vector<Circuit> search(const std::vector<Circuit>& arr_detect) {
        if (arr_detect.size() < 10) { 
            throw std::runtime_error("Not enough circuits");
        }

        std::vector<Circuit> sorted_arr = arr_detect;
        std::sort(sorted_arr.begin(), sorted_arr.end(),
                  [](const Circuit& x, const Circuit& y) { return x.area < y.area; });

        int index_detected = -1;
        double min_diff = std::numeric_limits<double>::max();

        for (size_t i = 0; i <= sorted_arr.size() - 10; ++i) {
            double diff = sorted_arr[i + 9].area - sorted_arr[i].area;
            
            if (diff <= sorted_arr[i].area / 2.0) {
                min_diff = diff;
                index_detected = static_cast<int>(i);
            }
        }

        if (index_detected == -1) {
            throw std::runtime_error("No suitable group found");
        }

        auto end_it = sorted_arr.begin() + index_detected + 10;
        return std::vector<Circuit>(sorted_arr.begin() + index_detected, end_it);
    }
};




#endif // DETECTOR_HPP