#include "segmentation.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>

int main() {
    cv::Mat img = cv::imread("test.jpg", cv::IMREAD_COLOR);
    cv::Mat out = cv::Mat(img.size().height, img.size().width, CV_8UC3);
    
    int k;
    std::cout << "input a value for k..." << std::endl;
    std::cin >> k;
    auto begin = std::chrono::steady_clock::now();
    segment(img, out, k, color, 10, true);
    auto end = std::chrono::steady_clock::now();

    std::cout << "total time:"<< std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << " ms" << std::endl;

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