#include "segmentation.hpp"

static Edge createEdge(size_t i0, size_t j0, size_t i1, size_t j1, cv::Mat img){
  return Edge{Pixel{j0, i0, img.at<cv::Vec3b>(j0, i0)}, Pixel{j1, i1, img.at<cv::Vec3b>(j1, i1)}};
}

void run_segmentation(std::vector<std::vector<utils::Set<Pixel>>>& segments, std::list<Edge>& edges, uint8_t color, uint32_t k){
  edges.sort([color](Edge &first, Edge &second)
    {
    auto w1 = abs(first.a.color[color] - first.b.color[color]);
    auto w2 = abs(second.a.color[color] - second.b.color[color]);

    return w1 < w2; 
    });

  std::unordered_map<utils::Node<Pixel>, uint8_t> internal_diff;

  for (auto edge : edges)
  {
    auto segment1 = segments[edge.a.y][edge.a.x];
    auto segment2 = segments[edge.b.y][edge.b.x];
    if (segment1.Find() != segment2.Find()){
      auto diff1_iter = internal_diff.find(*segment1.Find());
      auto diff2_iter = internal_diff.find(*segment2.Find());
      uint8_t diff1_internal = diff1_iter == internal_diff.end() ? 0 : diff1_iter->second;
      uint8_t diff2_internal = diff2_iter == internal_diff.end() ? 0 : diff2_iter->second;
      float t1 = k / segment1.Size();
      float t2 = k / segment2.Size();

      float diff = diff1_internal + t1 < diff2_internal + t2 ? 
                      diff1_internal + t1 : diff2_internal + t2;

      uint8_t edge_weight = abs(edge.a.color[color] - edge.b.color[color]);

      if(edge_weight <= diff){
        segment1.Union(segment2);
        auto rep = segment1.Find();
        internal_diff[*rep] = edge_weight;
      }
    }
  }

}

std::vector<Pixel> segment(cv::Mat img, int k, EvalMethod method) {
  auto size = img.size();
  std::list<Edge> edges;

  // creating vector holding pixels as disjoint sets
  std::vector<std::vector<utils::Set<Pixel>>> segments(size.height);

  for (size_t i = 0; i < size.height; i++)
  {
    segments[i] = std::vector<utils::Set<Pixel>>(size.width);
    for (size_t j = 0; j < size.width; j++)
    {
      segments[i][j] = utils::Set(Pixel{i, j, img.at<cv::Vec3b>()});
    }
  }

  // creating vector of all edges
  for (size_t i = 0; i < size.height - 1; i++)
  {
    size_t j = 0;
    edges.push_back(createEdge(i, j, i + 1, j, img));
    edges.push_back(createEdge(i, j, i + 1, j + 1, img));
    edges.push_back(createEdge(i, j, i, j + 1, img));

    for (size_t j = 1; j < size.width - 1; j++)
    {
      edges.push_back(createEdge(i, j, i + 1, j, img));
      edges.push_back(createEdge(i, j, i + 1, j + 1, img));
      edges.push_back(createEdge(i, j, i, j + 1, img));
      edges.push_back(createEdge(i, j, i + 1, j - 1, img));
    }

    j = size.width - 1;
    edges.push_back(createEdge(i, j, i + 1, j, img));
    edges.push_back(createEdge(i, j, i + 1, j - 1, img));
  }

  size_t i = size.height - 1;
  for (int j = 0; j < size.width - 1; j++){
    edges.push_back(createEdge(i, j, i, j+1, img));
  }

  // algorithm runs for all colors separately
}