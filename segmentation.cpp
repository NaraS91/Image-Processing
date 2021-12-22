#include "segmentation.hpp"

static Edge createEdge(size_t i0, size_t j0, size_t i1, size_t j1, cv::Mat img){
  return Edge{Pixel{i0, j0, img.at<cv::Vec3b>(i0, j0)}, Pixel{i1, j1, img.at<cv::Vec3b>(i1, j1)}};
}

std::vector<Pixel> segment(cv::Mat img, int k, EvalMethod method) {
  auto size = img.size();
  std::set<Edge> edges;

  for (size_t i = 0; i < size.height - 1; i++){
    size_t j = 0;
    edges.insert(createEdge(i, j, i+1, j, img));
    edges.insert(createEdge(i, j, i+1, j+1, img));
    edges.insert(createEdge(i, j, i, j+1, img));

    for (size_t j = 1; j < size.width - 1; j ++){
      edges.insert(createEdge(i, j, i+1, j, img));
      edges.insert(createEdge(i, j, i+1, j+1, img));
      edges.insert(createEdge(i, j, i, j+1, img));
      edges.insert(createEdge(i, j, i+1, j-1, img));
    }

    j = size.width - 1;
    edges.insert(createEdge(i, j, i+1, j, img));
    edges.insert(createEdge(i, j, i+1, j-1, img));
  }

  size_t i = size.height - 1;
  for (int j = 0; j < size.width - 1; j++){
    edges.insert(createEdge(i, j, i, j+1, img));
  }

  
}