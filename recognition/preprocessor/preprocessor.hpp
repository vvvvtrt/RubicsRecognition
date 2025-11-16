#ifndef PREROPCESSOR_HPP
#define PREROPCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>


class ImageProcessor {
    virtual cv::Mat process(const cv::Mat& image) = 0;
};


class DilateProcessor : public ImageProcessor {
public:
    cv::Mat process(const cv::Mat& image) override {
        cv::Mat imgGray, imgBlur, imgCanny, imgDil, imgCLAHE;

        cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        clahe->apply(imgGray, imgCLAHE);
        cv::GaussianBlur(imgCLAHE, imgBlur, cv::Size(3, 3), 3, 0);
        cv::Canny(imgBlur, imgCanny, 25, 75);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::dilate(imgCanny, imgDil, kernel);

        return imgDil;
    }
};


#endif // PREROPCESSOR_HPP