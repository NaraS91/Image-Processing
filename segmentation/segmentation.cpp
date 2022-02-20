#include "segmentation.hpp"
#include "../utils/disjoint-set.hpp"
#include "Segment.hpp"
#include <iostream>
#include <algorithm>


//TODO: optimize, add image smoothing
static inline Edge createEdge(size_t i0, size_t j0, size_t i1, size_t j1, cv::Mat img)
{
  return Edge{Pixel{i0, j0, img.at<cv::Vec3b>(i0, j0)}, Pixel{i1, j1, img.at<cv::Vec3b>(i1, j1)}};
}

unsigned inline index(Pixel p, unsigned width) {
  return p.x * width + p.y;
}

std::vector<unsigned> adjacentPixels(unsigned i, unsigned j, unsigned height, unsigned width) {
  std::vector<unsigned> result;
  result.reserve(8);
  if (i > 0) {
    result.push_back((i - 1) * width + j);
    if (j > 0) 
      result.push_back((i - 1) * width + (j - 1));

    if (j + 1 < width)
      result.push_back((i - 1) * width + (j + 1));
  }

  if (j > 0)
    result.push_back(i * width + (j - 1));

  if (j + 1 < width)
    result.push_back(i * width + (j + 1));

  if (i + 1 < height) {
    result.push_back((i + 1) * width + j);
    if (j > 0)
      result.push_back((i + 1) * width + (j - 1));

    if (j + 1 < width)
      result.push_back((i + 1) * width + (j + 1));
  }

  return result;
}

void run_segmentation(utils::Set& segments, std::vector<Edge>& edges, uint8_t color, uint32_t k, unsigned image_width, unsigned image_height)
{
  std::sort(edges.begin(), edges.end(), [color](Edge& first, Edge& second)
    {
      auto w1 = abs(first.a.color[color] - first.b.color[color]);
      auto w2 = abs(second.a.color[color] - second.b.color[color]);

      return w1 < w2;
    });

  std::vector<float> internal_diff = std::vector<float>(image_width * image_height + 1, 0.0);
  for (auto edge : edges)
  {
    unsigned p1_index = index(edge.a, image_width);
    unsigned p2_index = index(edge.b, image_width);
    if (segments.Find(p1_index) != segments.Find(p2_index))
    {
      auto diff1 = internal_diff[p1_index];
      auto diff2 = internal_diff[p2_index];
      float t1 = k / segments.Size(p1_index);
      float t2 = k / segments.Size(p2_index);

      float diff = diff1 + t1 < diff2 + t2 ? diff1 + t1 : diff2 + t2;

      float edge_weight = (float) abs(edge.a.color[color] - edge.b.color[color]);
      if(edge_weight <= diff)
      {
        segments.Union(p1_index, p2_index);
        auto rep = segments.Find(p1_index);
        internal_diff[rep] = edge_weight + 1;
      }
    }
  }
}

void intersect(utils::Set& out, utils::Set &segments_red, utils::Set &segments_green, utils::Set &segments_blue,
               size_t height, size_t width)
{
  size_t p_index = 0;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      std::vector<unsigned> candidates = adjacentPixels(i, j, height, width);
      for (auto other_pixel : candidates) {
        bool same_red = segments_red.Find(other_pixel) == segments_red.Find(p_index);
        bool same_green = segments_green.Find(other_pixel) == segments_green.Find(p_index);
        bool same_blue = segments_blue.Find(other_pixel) == segments_blue.Find(p_index);

        if (same_blue && same_red && same_green) {
          out.Union(p_index, other_pixel);
        }
      }

      p_index++;
    }
  }
}

void createOutputImage(cv::Mat &out, utils::Set& segments, unsigned height, unsigned width)
{
  auto colors = std::unordered_map<unsigned, cv::Vec3b>();

  size_t p_index = 0;
  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      auto rep = segments.Find(p_index);
      cv::Vec3b color;
      if (colors.find(rep) != colors.end())
        color = colors[rep];
      else {
        color[0] = rand() % 256;
        color[1] = rand() % 256;
        color[2] = rand() % 256;
        colors[rep] = color;
      }
      out.at<cv::Vec3b>(i, j) = color;

      p_index++;
    }
  }
}

//void createOutputSegments(std::vector<>& out, utils::Set& segments, unsigned height, unsigned width)

void segment(cv::Mat& img, utils::Set& out, int k, EvalMethod method) {

  auto size = img.size();
  std::vector<Edge> edges;
  edges.reserve(size.height * size.width * 5);

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
  for (int j = 0; j < size.width - 1; j++) {
    edges.push_back(createEdge(i, j, i, j + 1, img));
  }

  auto red_segments = utils::Set(size.height * size.width);
  auto green_segments = utils::Set(size.height * size.width);
  auto blue_segments = utils::Set(size.height * size.width);
  // algorithm runs for all colors separately
  run_segmentation(red_segments, edges, 0, k, size.width, size.height);
  run_segmentation(green_segments, edges, 1, k, size.width, size.height);
  run_segmentation(blue_segments, edges, 2, k, size.width, size.height);

  intersect(out, red_segments, green_segments, blue_segments, size.height, size.width);
}

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method)
{
  utils::Set segmented = utils::Set(img.size().height * img.size().width);
  segment(img, segmented, k, method);
  createOutputImage(out, segmented, img.size().height, img.size().width);
  std::cout << "out done" << std::endl;
}

//void segment(cv::Mat& img, std::vector<Segment>& out, int k, EvalMethod method) {
//  utils::Set segmented = utils::Set(img.size().height * img.size().width);
//  segment(img, segmented, k, method);
//  createOutputSegments(out, segmented, img.size().height, img.size().width);
//  std::cout << "out done" << std::endl;
//}