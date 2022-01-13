#include "segmentation.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main() {
    cv::Mat img = cv::imread("smallTest.png", cv::IMREAD_COLOR);
    cv::Mat out = cv::Mat(img.size().height, img.size().width, CV_8UC3);
    
    int k;
    std::cout << "input a value for k..." << std::endl;
    std::cin >> k;
    segment(img, out, k, color);

    cv::imshow("Display window", out);

    k = cv::waitKey(0);

    char c;
    std::cout << "press s to save the image"<< std::endl;
    std::cin >> c;
    if (c == 's')
    {
        cv::imwrite("out.png", out);
    }

    return 0;
}