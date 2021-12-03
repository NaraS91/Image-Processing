#include <array>
#include <vector>
#include <tuple>
#include <opencv2/core.hpp>

class segment {
  public:
    std::vector<size_t> histogram(int bins){
      std::vector<size_t> hist = std::vector<size_t>(bins * 3, 0);
      int binSize = 255 / bins;
      for (size_t i = _x; i < _x + _height; i++){
        for (size_t j = _y; j < _y + _width; j++){
            cv::Vec3b row = img->at<cv::Vec3b>(i, j);
            hist[row[0] / binSize]++;
            hist[row[1] / binSize + bins]++;
            hist[row[2] / binSize + bins * 2]++;
        }
      }
    }
  private:
    size_t _x;
    size_t _y;
    size_t _height;
    size_t _width;
    cv::Mat *img;
};