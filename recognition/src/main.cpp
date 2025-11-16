#include <opencv2/opencv.hpp>
#include <preprocessor/preprocessor.hpp>
#include <detector/detector.hpp>

int main() {

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Ошибка камеры!" << std::endl;
        return -1;
    }

    DilateProcessor preprocessor;   // <-- твой наследник ImageProcessor
    ContourDetector detector;        // <-- твой детектор

    while (true) {

        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;

        // --- 1. ПРЕПРОЦЕССИНГ ---
        cv::Mat processed = preprocessor.process(frame);

        // --- 2. ДЕТЕКЦИЯ КОНТУРОВ ---
        std::vector<Circuit> circuits = detector.detect(processed);

        // --- 3. ОТРИСОВКА НА ОРИГИНАЛЬНОЙ КАРТИНКЕ ---
        for (const Circuit& c : circuits) {
            cv::rectangle(
                frame,
                c.start,
                c.end,
                cv::Scalar(0, 255, 0),
                2
            );

            cv::putText(frame,
                        "Cube",
                        c.start,
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.7,
                        cv::Scalar(0, 255, 0),
                        2);

        }

        // показываем картинку
        cv::imshow("Processed", processed);
        cv::imshow("Detections", frame);

        if (cv::waitKey(1) == 27) break; // ESC для выхода
    }

    return 0;
}