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
