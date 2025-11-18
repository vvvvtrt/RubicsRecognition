#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>


struct Circuit {
    Circuit() = default;
	Circuit(int ar, cv::Point st, cv::Point en) : area(ar), start(st), end(en) {}
	~Circuit() = default;

	int area;
	cv::Point start;
	cv::Point end;
};

struct RubikColor {
    std::string name;
    cv::Vec3b bgr;
};

inline const std::vector<RubikColor> RUBIK_COLORS = {
    {"White",  cv::Vec3b(240, 240, 240)},
    {"Yellow", cv::Vec3b(30, 220, 220)},
    {"Green",  cv::Vec3b(50, 180, 50)},
    {"Blue",   cv::Vec3b(180, 80, 30)},
    {"Red",    cv::Vec3b(30, 30, 200)},
    {"Orange", cv::Vec3b(20, 100, 240)}
};


#endif // UTILS_HPP