#include <opencv2/opencv.hpp>
#include <preprocessor/preprocessor.hpp>
#include <detector/detector.hpp>
#include <classifier/classifier.hpp>
#include <utils/circut.hpp>
#include <map>
#include <algorithm>
#include <string>


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

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Ошибка камеры!" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);
    
    DilateProcessor preprocessor;   
    ContourDetector detector;
    SquareSearcher9 searcher;
    
    std::vector<std::vector<cv::Vec3b>> saveColors;

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;
        
        cv::Mat processed = preprocessor.process(frame);
        std::vector<Circuit> circuits = detector.detect(processed);
        Circuit cubeBox;
        bool cubeDetected = false;
        
        if (!circuits.empty()) {
            auto maxIt = std::max_element(circuits.begin(), circuits.end(),
                [](const Circuit& a, const Circuit& b) { return a.area < b.area; });
            cubeBox = *maxIt;
            cubeDetected = true;
            
            cv::rectangle(frame, cubeBox.start, cubeBox.end, 
                cv::Scalar(255, 0, 255), 2);
            cv::putText(frame, "Rubik Cube",
                cv::Point(cubeBox.start.x, cubeBox.start.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.7,
                cv::Scalar(255, 0, 255), 2);
        }
        
        std::vector<Circuit> selected;
        
        if (cubeDetected) {
            std::vector<Circuit> insideCube;
            int margin = 20; 
            
            for (const Circuit& c : circuits) {
                if (c.area == cubeBox.area) continue;
                
                bool insideX = (c.start.x >= cubeBox.start.x - margin) && 
                               (c.end.x <= cubeBox.end.x + margin);
                bool insideY = (c.start.y >= cubeBox.start.y - margin) && 
                               (c.end.y <= cubeBox.end.y + margin);
                
                if (insideX && insideY) {
                    insideCube.push_back(c);
                }
            }
            
            try {
                selected = searcher.search(insideCube);
            }
            catch (const std::exception& e) {
                selected.clear();
            }
        }
        
        std::vector<cv::Vec3b> meanColors;
        bool haveAllColors = false;

        if (selected.size() == 9) {
            std::sort(selected.begin(), selected.end(),
                [](const Circuit& a, const Circuit& b) {

                    int ay = (a.start.y + a.end.y) / 2;
                    int by = (b.start.y + b.end.y) / 2;

                    if (std::abs(ay - by) < 20) {
                        int ax = (a.start.x + a.end.x) / 2;
                        int bx = (b.start.x + b.end.x) / 2;
                        return ax < bx;
                    }

                    return ay < by;
                }
            );

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
            haveAllColors = true;
        }
        
        //std::vector<cv::Vec3b> clusteredColors = meanColors;
        KMeansClassifier classifier;
        std::vector<cv::Vec3b> clusteredColors = classifier.classifierColors(meanColors, 6);
        
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
            

            /*std::string bgrText = cv::format("Orig: B:%d G:%d R:%d", 
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
                1);*/
        }
        
        
        cv::imshow("Processed", processed);
        cv::imshow("Detections", frame);
        
        if (haveAllColors) {
            std::cout << "Saved colors (y/n):  ";
            std::string input;
            std::cin >> input;

            if (input == "y" || input == "Y") {
                saveColors.push_back(clusteredColors);
                std::cout << "Colors saved. Total saved sets: " << saveColors.size() << std::endl;
            }
        }

        if (!saveColors.empty()) {
            int cellSize = 100;
            cv::Mat cubeFace(3 * cellSize, 3 * cellSize, CV_8UC3, cv::Scalar(0,0,0));

            const auto& face = saveColors.back();

            for (int i = 0; i < 9; i++) {
                int row = i / 3;
                int col = i % 3;

                cv::Rect r(col * cellSize, row * cellSize, cellSize, cellSize);

                cv::rectangle(cubeFace, r, 
                    cv::Scalar(face[i][0], face[i][1], face[i][2]), 
                    cv::FILLED);

                cv::rectangle(cubeFace, r, cv::Scalar(0,0,0), 2);
            }

            cv::imshow("Last Saved Cube Side", cubeFace);
        }

        if (cv::waitKey(30) == 27) {
            break;
        }
        
    }
    
    cv::destroyAllWindows();
    return 0;
}