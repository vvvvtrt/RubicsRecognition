#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>


struct Circuit {
	Circuit(int ar, cv::Point st, cv::Point en) : area(ar), start(st), end(en) {}
	~Circuit() = default;

	int area;
	cv::Point start;
	cv::Point end;
};

#endif // UTILS_HPP