#include "segmentation.hpp"

int main() {
    cv::Mat img = cv::imread("test.jpg", cv::IMREAD_COLOR);
    cv::Mat out = cv::Mat(img.size().height, img.size().width, CV_8UC3);
    segment(img, out, 500, color);

    cv::imshow("Display window", out);
    int k = cv::waitKey(0);

    if (k == 's')
    {
        cv::imwrite("out.png", out);
    }

    return 0;
}