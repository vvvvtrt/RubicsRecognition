#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>


struct Circuit {
	Circuit(int ar, cv::Point st, cv::Point en) : area(ar), start(st), end(en) {}
	~Circuit() = default;

	int area;
	cv::Point start;
	cv::Point end;
};

struct Color
{
    int class_color;
    cv::Scalar color_value;
};


#endif // UTILS_HPP