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