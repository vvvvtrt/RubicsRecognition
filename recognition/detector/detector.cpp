#include "detector.hpp"


std::vector<Circuit> ContourDetector::detect(const cv::Mat& image){
    std::vector<Circuit> arr_detect;
    std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);	

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());

	std::vector<Circuit> arr_a;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		std::string objectType;

		if (area > 3000)
		{
			double peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);

			Circuit temp(area, boundRect[i].tl(), boundRect[i].br());
			arr_detect.push_back(temp);

				//drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 2);
				//rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 5);
		}
	}

    return arr_detect;
}



std::vector<Circuit> SquareSearcher::search(const std::vector<Circuit>& arr_detect)  {
        if (arr_detect.size() < 10) { 
            throw std::runtime_error("Not enough circuits");
        }

        std::vector<Circuit> sorted_arr = arr_detect;
        std::sort(sorted_arr.begin(), sorted_arr.end(),
                  [](const Circuit& x, const Circuit& y) { return x.area < y.area; });

        int index_detected = -1;
        double min_diff = std::numeric_limits<double>::max();

        for (size_t i = 0; i <= sorted_arr.size() - 10; ++i) {
            double diff = sorted_arr[i + 9].area - sorted_arr[i].area;
            
            if (diff <= sorted_arr[i].area / 2.0) {
                min_diff = diff;
                index_detected = static_cast<int>(i);
            }
        }

        if (index_detected == -1) {
            throw std::runtime_error("No suitable group found");
        }

        auto end_it = sorted_arr.begin() + index_detected + 10;
        return std::vector<Circuit>(sorted_arr.begin() + index_detected, end_it);
}




std::vector<Circuit> SquareSearcher9::search(const std::vector<Circuit>& arr_detect) {
        if (arr_detect.size() < 9) {
            throw std::runtime_error("Not enough circuits");
        }

        std::vector<Circuit> sorted_arr = arr_detect;
        std::sort(sorted_arr.begin(), sorted_arr.end(),
                  [](const Circuit& x, const Circuit& y) { return x.area < y.area; });

        int index_detected = -1;
        double min_diff = std::numeric_limits<double>::max();

        for (size_t i = 0; i <= sorted_arr.size() - 9; ++i) {
            double diff = sorted_arr[i + 8].area - sorted_arr[i].area;

            if (diff <= sorted_arr[i].area / 2.0 && diff < min_diff) {
                min_diff = diff;
                index_detected = static_cast<int>(i);
            }
        }

        if (index_detected == -1) {
            throw std::runtime_error("No suitable group found");
        }

        return std::vector<Circuit>(
            sorted_arr.begin() + index_detected,
            sorted_arr.begin() + index_detected + 9
        );
}