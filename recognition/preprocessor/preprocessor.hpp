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


cv::Vec3b getMeanColor(const cv::Mat& square) {
    if (square.empty()) {
        return cv::Vec3b(0, 0, 0);
    }
    
    int margin = square.rows / 4;
    cv::Rect centerROI(margin, margin, 
                       square.cols - 2*margin, 
                       square.rows - 2*margin);
    
    if (centerROI.width <= 0 || centerROI.height <= 0) {
        centerROI = cv::Rect(0, 0, square.cols, square.rows);
    }
    
    cv::Mat center = square(centerROI);
    
    cv::Scalar meanScalar = cv::mean(center);
    
    cv::Vec3b meanColor(
        static_cast<uchar>(std::round(meanScalar[0])),
        static_cast<uchar>(std::round(meanScalar[1])),
        static_cast<uchar>(std::round(meanScalar[2]))
    );
    
    return meanColor;
}

#endif // PREROPCESSOR_HPP