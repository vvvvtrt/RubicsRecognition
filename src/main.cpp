#include <opencv2/opencv.hpp>
#include <map>
#include <algorithm>
#include <string>
#include "recognition.hpp"
#include <gui/display.hpp>


int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Ошибка камеры!" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);
    
    recognition::DilateProcessor preprocessor;   
    recognition::ContourDetector detector;
    recognition::SquareSearcher9 searcher;
    
    std::vector<std::vector<cv::Vec3b>> saveColors;

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;
        
        cv::Mat processed = preprocessor.process(frame);
        std::vector<recognition::Circuit> circuits = detector.detect(processed);
        recognition::Circuit cubeBox;
        bool cubeDetected = false;
        
        if (!circuits.empty()) {
            auto maxIt = std::max_element(circuits.begin(), circuits.end(),
                [](const recognition::Circuit& a, const recognition::Circuit& b) { return a.area < b.area; });
            cubeBox = *maxIt;
            cubeDetected = true;
            
        }
        
        std::vector<Circuit> selected;
        
        if (cubeDetected) {
            std::vector<recognition::Circuit> insideCube;
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
                [](const recognition::Circuit& a, const recognition::Circuit& b) {

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

            for (const recognition::Circuit& c : selected) {
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
        recognition::KMeansClassifier classifier;
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
                if (saveColors.size() >= 6) {
                    break;
                }
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

            cv::imshow("Last Saved Cube", cubeFace);
        }

        if (cv::waitKey(30) == 27) {
            break;
        }
        
    }
    
    cv::destroyAllWindows();


    RubiksCubeApp app(saveColors);
    app.run();


    return 0;
}
