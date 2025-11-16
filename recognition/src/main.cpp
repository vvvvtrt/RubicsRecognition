#include <opencv2/opencv.hpp>
#include <preprocessor/preprocessor.hpp>
#include <detector/detector.hpp>
#include <classifier/classifier.hpp>
#include <map>
#include <algorithm>

// Эталонные цвета кубика Рубика в BGR
struct RubikColor {
    std::string name;
    cv::Vec3b bgr;
};

std::vector<RubikColor> RUBIK_COLORS = {
    {"White",  cv::Vec3b(240, 240, 240)},
    {"Yellow", cv::Vec3b(30, 220, 220)},
    {"Green",  cv::Vec3b(50, 180, 50)},
    {"Blue",   cv::Vec3b(180, 80, 30)},
    {"Red",    cv::Vec3b(30, 30, 200)},
    {"Orange", cv::Vec3b(20, 100, 240)}
};

// Функция для определения названия цвета по BGR
std::string getColorName(const cv::Vec3b& color) {
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

// Функция для получения среднего цвета центральной части квадрата
cv::Vec3b getMeanColor(const cv::Mat& square) {
    if (square.empty()) {
        return cv::Vec3b(0, 0, 0);
    }
    
    // Берем центральную часть квадрата (исключаем границы)
    int margin = square.rows / 4;
    cv::Rect centerROI(margin, margin, 
                       square.cols - 2*margin, 
                       square.rows - 2*margin);
    
    if (centerROI.width <= 0 || centerROI.height <= 0) {
        centerROI = cv::Rect(0, 0, square.cols, square.rows);
    }
    
    cv::Mat center = square(centerROI);
    
    // Вычисляем средний цвет
    cv::Scalar meanScalar = cv::mean(center);
    
    cv::Vec3b meanColor(
        static_cast<uchar>(std::round(meanScalar[0])),
        static_cast<uchar>(std::round(meanScalar[1])),
        static_cast<uchar>(std::round(meanScalar[2]))
    );
    
    return meanColor;
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Ошибка камеры!" << std::endl;
        return -1;
    }
    
    // Настройка камеры для лучшего качества
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);
    
    DilateProcessor preprocessor;   
    ContourDetector detector;
    SquareSearcher searcher;
    
    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;
        
        cv::Mat processed = preprocessor.process(frame);

        std::vector<Circuit> circuits = detector.detect(processed);

        std::vector<Circuit> selected;
        try {
            selected = searcher.search(circuits);
        } catch (const std::runtime_error& e) {
            selected.clear();
        }
        
        std::vector<cv::Vec3b> meanColors;
        
        for (const Circuit& c : selected) {
            cv::Point tl = c.start;
            cv::Point br = c.end;
            
            tl.x = std::max(0, tl.x);
            tl.y = std::max(0, tl.y);
            br.x = std::min(frame.cols, br.x);
            br.y = std::min(frame.rows, br.y);
            
            if (br.x > tl.x && br.y > tl.y) {
                cv::Rect roi(tl, br);
                cv::Mat square = frame(roi).clone();
                
                cv::Vec3b color = getMeanColor(square);
                meanColors.push_back(color);
            } else {
                meanColors.push_back(cv::Vec3b(0, 0, 0));
            }
        }
        
        std::vector<cv::Vec3b> clusteredColors = meanColors;
        
        if (meanColors.size() >= 6) {
            cv::Mat colorData(meanColors.size(), 3, CV_32F);
            for (size_t i = 0; i < meanColors.size(); i++) {
                colorData.at<float>(i, 0) = meanColors[i][0];
                colorData.at<float>(i, 1) = meanColors[i][1];
                colorData.at<float>(i, 2) = meanColors[i][2];
            }
            
            cv::Mat labels, centers;
            int K = 6;
            cv::kmeans(colorData, K, labels,
                       cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.1),
                       3, cv::KMEANS_PP_CENTERS, centers);

            for (size_t i = 0; i < meanColors.size(); i++) {
                int clusterIdx = labels.at<int>(i);
                clusteredColors[i] = cv::Vec3b(
                    static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 0))),
                    static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 1))),
                    static_cast<uchar>(std::round(centers.at<float>(clusterIdx, 2)))
                );
            }
        }
        
        for (size_t i = 0; i < selected.size(); i++) {
            const Circuit& c = selected[i];
            
            if (i >= clusteredColors.size()) continue;
            
            cv::Vec3b color = clusteredColors[i];
            cv::Vec3b originalColor = (i < meanColors.size()) ? meanColors[i] : color;
            
            cv::Scalar rectColor(color[0], color[1], color[2]);
            cv::rectangle(frame, c.start, c.end, rectColor, 3);
            
            std::string colorName = getColorName(color);
            
            cv::Size textSize = cv::getTextSize(colorName, cv::FONT_HERSHEY_SIMPLEX, 0.6, 2, nullptr);
            cv::rectangle(frame, 
                cv::Point(c.start.x, c.start.y - textSize.height - 10),
                cv::Point(c.start.x + textSize.width, c.start.y),
                cv::Scalar(0, 0, 0),
                cv::FILLED);

            cv::putText(frame,
                colorName,
                cv::Point(c.start.x, c.start.y - 5),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(255, 255, 255),
                2);
            
            std::string bgrText = cv::format("Orig: B:%d G:%d R:%d", 
                originalColor[0], originalColor[1], originalColor[2]);
            cv::putText(frame,
                bgrText,
                cv::Point(c.start.x, c.end.y + 20),
                cv::FONT_HERSHEY_SIMPLEX,
                0.35,
                cv::Scalar(200, 200, 200),
                1);
            
            std::string clusterText = cv::format("Clust: B:%d G:%d R:%d", 
                color[0], color[1], color[2]);
            cv::putText(frame,
                clusterText,
                cv::Point(c.start.x, c.end.y + 35),
                cv::FONT_HERSHEY_SIMPLEX,
                0.35,
                cv::Scalar(255, 255, 0),
                1);
        }
        
        if (!clusteredColors.empty()) {
            int paletteWidth = std::min(600, static_cast<int>(clusteredColors.size() * 60));
            int squareSize = paletteWidth / clusteredColors.size();
            cv::Mat palette(80, paletteWidth, CV_8UC3, cv::Scalar(50, 50, 50));
            
            for (size_t i = 0; i < clusteredColors.size(); i++) {
                cv::Rect colorRect(i * squareSize, 10, squareSize - 5, 60);
                cv::rectangle(palette, colorRect, 
                    cv::Scalar(clusteredColors[i][0], clusteredColors[i][1], clusteredColors[i][2]), 
                    cv::FILLED);
                
                std::string name = getColorName(clusteredColors[i]);
                cv::putText(palette, name.substr(0, 3),
                    cv::Point(i * squareSize + 5, 35),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4,
                    cv::Scalar(255, 255, 255), 1);
            }
            cv::imshow("Color Palette", palette);
        }
        
        cv::imshow("Processed", processed);
        cv::imshow("Detections", frame);
        
        if (cv::waitKey(1) == 27) break; // ESC для выхода
    }
    
    cv::destroyAllWindows();
    return 0;
}