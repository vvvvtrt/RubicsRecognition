#include "classifier.hpp"

std::vector<cv::Vec3b> KMeansClassifier::classifierColors(
        const std::vector<cv::Vec3b>& meanColors, int k)
{
    std::vector<cv::Vec3b> clusteredColors = meanColors;

    if (meanColors.size() != 9) {
        return clusteredColors;
    }

    cv::Mat colorData(meanColors.size(), 3, CV_32F);
    for (size_t i = 0; i < meanColors.size(); i++) {
        colorData.at<float>(i, 0) = meanColors[i][0];
        colorData.at<float>(i, 1) = meanColors[i][1];
        colorData.at<float>(i, 2) = meanColors[i][2];
    }

    cv::Mat labels, centers;
    cv::kmeans(
        colorData,
        k,
        labels,
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.1),
        3,
        cv::KMEANS_PP_CENTERS,
        centers
    );

    for (size_t i = 0; i < meanColors.size(); i++) {
        int clusterIdx = labels.at<int>(i);

        clusteredColors[i] = cv::Vec3b(
            static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 0))),
            static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 1))),
            static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 2)))
        );
    }

    return clusteredColors;
}



std::string getColorName(const cv::Vec3b& color){
    double min_dist = std::numeric_limits<double>::max();
    std::string result = "Unknown";
    
    for (const auto& rubik : RUBIK_COLORS) {
        double dist = std::sqrt(
            std::pow(color[0] - rubik.bgr[0], 2) +
            std::pow(color[1] - rubik.bgr[1], 2) +
            std::pow(color[2] - rubik.bgr[2], 2)
        );
        
        if (dist < min_dist) {
            min_dist = dist;
            result = rubik.name;
        }
    }
    
    return result;
}