#ifndef SEGMENT_HPP
#define SEGMENT_HPP
#include <opencv2/core.hpp>
#include <list>

class Segment {
  public:
    Segment(cv::Mat& img, int x, int y) :
      _img(img),
      _x(x),
      _x2(x),
      _y(y),
      _y2(y)
    {
      _pixels = std::list<int>(index(x, y));
    }

    void add(int x, int y) {
      _pixels.push_back(index(x, y));

      if (x < _x)
        _x = x;
      if (y < _y)
        _y = y;
      if (x > _x2)
        _x2 = x;
      if (y > _y2)
        _y2 = y;
    }

    //other segment should not be used after a call to this method
    void join(Segment other) {
      if (other._x < _x)
        _x = other._x;
      if (other._y < _y)
        _y = other._y;
      if (other._x2 > _x2)
        _x2 = other._x2;
      if (other._y2 > _y2)
        _y2 = other._y2;

      _pixels.splice(_pixels.end(), other._pixels);
    }

  private:
    int index(int x, int y) {
      return x * _img.size().width + y;
    }

    //original image
    cv::Mat& _img;

    //pixels are enumerated from top-left corner, right then down
    std::list<int> _pixels;

    //bounding box, top-left and bottom-right corners 
    int _x;
    int _y;
    int _x2;
    int _y2;
};

#endif