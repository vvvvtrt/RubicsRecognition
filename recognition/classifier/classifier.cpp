#include "classifier.hpp"


std::vector<cv::Vec3b> KMeansClassifier::classifierColors(const std::vector<cv::Mat>& squareImages, int k){
        cv::Mat data;
        for (const auto& square : squareImages) {
            cv::Mat reshaped = square.reshape(1, square.total());
            data.push_back(reshaped);
        }
        
        data.convertTo(data, CV_32F);
        
        cv::Mat labels, centers;
        cv::kmeans(data, k, labels, 
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0),
                3, cv::KMEANS_PP_CENTERS, centers);
        
        std::vector<cv::Vec3b> dominantColors;
        for (int i = 0; i < centers.rows; i++) {
        cv::Vec3b color(
            static_cast<uchar>(centers.at<float>(i, 0)),  // B
            static_cast<uchar>(centers.at<float>(i, 1)),  // G
            static_cast<uchar>(centers.at<float>(i, 2))   // R
        );
        dominantColors.push_back(color);
    }
    
    return dominantColors;
}