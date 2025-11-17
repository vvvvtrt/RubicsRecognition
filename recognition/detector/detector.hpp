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




class Searcher {
public:
    virtual std::vector<Circuit> search(const std::vector<Circuit>& arr_detect) = 0;
};


class SquareSearcher : public Searcher {
public:
    std::vector<Circuit> search(const std::vector<Circuit>& arr_detect) override;
};


class SquareSearcher9 : public Searcher{
public:
    std::vector<Circuit> search(const std::vector<Circuit>& arr_detect) override;
};



#endif // DETECTOR_HPP