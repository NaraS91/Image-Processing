#include "segmentation.hpp"
#include "../utils/disjoint-set.hpp"
#include "../utils/concurrent_disjoint_set.h"
#include "Segment.hpp"
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#define DEBUG false


//TODO: add small groups mreging
static inline Edge createEdge(size_t i0, size_t j0, size_t i1, size_t j1, cv::Mat img)
{
  auto p1 = Pixel{ i0, j0, img.at<cv::Vec3b>(i0, j0) };
  auto p2 = Pixel{ i1, j1, img.at<cv::Vec3b>(i1, j1) };
  return Edge{p1, p2, abs(p1.color[color] - p2.color[color]) };
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
      return first.w < second.w;
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
  int num_segments = 0;
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
        num_segments++;
      }
      out.at<cv::Vec3b>(i, j) = color;

      p_index++;
    }
  }
} 

//assumes width is at least 2 pixels
void concurrent_create_edges_run(cv::Mat& img, std::vector<Edge> &edges, cv::Size size,size_t startI, size_t endI) {
  for (size_t i = startI; i < endI; i++)
  {
    //index of first edfe in a row
    size_t rowI = std::max(i * (4 * size.width - 3), size_t(0));
    size_t j = 0;
    edges[rowI] = (createEdge(i, j, i + 1, j, img));
    edges[rowI + 1] = (createEdge(i, j, i + 1, j + 1, img));
    edges[rowI + 2] = (createEdge(i, j, i, j + 1, img));

    for (size_t j = 1; j < size.width - 1; j++)
    {
      edges[rowI + j * 4 - 1] = createEdge(i, j, i + 1, j, img);
      edges[rowI + j * 4] = createEdge(i, j, i + 1, j + 1, img);
      edges[rowI + j * 4 + 1] = createEdge(i, j, i, j + 1, img);
      edges[rowI + j * 4 + 2] = createEdge(i, j, i + 1, j - 1, img);
    }

    j = size.width - 1;
    edges[rowI + j * 4 - 1] = (createEdge(i, j, i + 1, j, img));
    edges[rowI + j * 4] = (createEdge(i, j, i + 1, j - 1, img));
  }
}

void concurrent_create_edges(cv::Mat& img, std::vector<Edge>& edges, cv::Size size, int threads_num) {
  edges = std::vector<Edge>((size.height - 1) * (size.width * 4 - 3) + size.width - 1);

  //populate vector with all edges
  std::vector<std::thread> threads(threads_num);
  for (int i = 0; i < threads_num - 1; i++) {
    threads[i] = std::thread(concurrent_create_edges_run, std::ref(img), std::ref(edges), size, (size.height - 1) / threads_num * i, (size.height - 1) / threads_num * (i + 1));
  }
  threads[threads_num - 1] = std::thread(concurrent_create_edges_run, std::ref(img), std::ref(edges), size, (size.height - 1) / threads_num * (threads_num - 1), size.height - 1);

  size_t i = size.height - 1;
  for (int j = 0; j < size.width - 1; j++) {
    size_t rowI = std::max(i * (4 * size.width - 3), size_t(0));
    edges[rowI + j] = createEdge(i, j, i, j + 1, img);
  }

  for (int i = 0; i < threads_num; i++) {
    threads[i].join();
  }
}

void create_edges(cv::Mat& img, std::vector<Edge>& edges, cv::Size size) {
  edges.reserve(size.height * size.width * 4);

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
}

void parallel_color_segmentation(utils::Set& red_segments, utils::Set& green_segments, utils::Set& blue_segments,
                                    std::vector<Edge>& edges, int k, cv::Size size) {
  std::vector<Edge> edges2;
  std::thread edges2_t([&edges2, &edges]() {edges2 = edges; });
  std::vector<Edge> edges3;
  std::thread edges3_t([&edges3, &edges]() {edges3 = edges; });
  edges2_t.join();
  edges3_t.join();

  std::thread red_t(run_segmentation, std::ref(red_segments), std::ref(edges), 0, k, size.width, size.height);
  std::thread green_t(run_segmentation, std::ref(green_segments), std::ref(edges2), 1, k, size.width, size.height);
  std::thread blue_t(run_segmentation, std::ref(blue_segments), std::ref(edges3), 2, k, size.width, size.height);

  red_t.join();
  green_t.join();
  blue_t.join();
}

void color_segmentation(utils::Set& red_segments, utils::Set& green_segments, utils::Set& blue_segments,
  std::vector<Edge>& edges, int k, cv::Size size) {
  run_segmentation(red_segments, edges, 0, k, size.width, size.height);
  run_segmentation(green_segments, edges, 1, k, size.width, size.height);
  run_segmentation(blue_segments, edges, 2, k, size.width, size.height);
}

//void createOutputSegments(std::vector<>& out, utils::Set& segments, unsigned height, unsigned width)
void segment(cv::Mat& img, utils::Set& out, int k, EvalMethod method, int concurrent_edges, bool parallel_segmentation) {
  std::chrono::steady_clock::time_point begin, end;
  if (DEBUG)
    begin = std::chrono::steady_clock::now();

  auto size = img.size();
  std::vector<Edge> edges;
  if (concurrent_edges > 1)
    concurrent_create_edges(img, edges, size, concurrent_edges);
  else
    create_edges(img, edges, size);

  if (DEBUG) {
    end = std::chrono::steady_clock::now();
    std::cout << "edges: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << " ms" << std::endl;
    begin = std::chrono::steady_clock::now();
  }
  
  auto red_segments = utils::Set(size.height * size.width);
  auto green_segments = utils::Set(size.height * size.width);
  auto blue_segments = utils::Set(size.height * size.width);

  if (parallel_segmentation)
    parallel_color_segmentation(red_segments, green_segments, blue_segments, edges, k, size);
  else
    color_segmentation(red_segments, green_segments, blue_segments, edges, k, size);

  if (DEBUG) {
    end = std::chrono::steady_clock::now();
    std::cout << "segmentation: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << " ms" << std::endl;
    begin = std::chrono::steady_clock::now();
  }
  
  intersect(out, red_segments, green_segments, blue_segments, size.height, size.width);

  if (DEBUG) {
    end = std::chrono::steady_clock::now();
    std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << " ms" << std::endl;
  }
}

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma, int parallel_edges, bool parallel_segmentation)
{
  utils::Set segmented = utils::Set(img.size().height * img.size().width);

  cv::Mat smoothed;
  if (sigma > 0) {
    cv::GaussianBlur(img, smoothed, cv::Size(0, 0), sigma);
    segment(smoothed, segmented, k, method, parallel_edges, parallel_segmentation);
  }
  else
    segment(img, segmented, k, method, parallel_edges, parallel_segmentation);
  createOutputImage(out, segmented, img.size().height, img.size().width);
}

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma, int parallel_edges)
{
  segment(img, out, k, method, sigma, parallel_edges, false);
}

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma)
{
  segment(img, out, k, method, sigma, 0, false);
}

//void segment(cv::Mat& img, std::vector<Segment>& out, int k, EvalMethod method) {
//  utils::Set segmented = utils::Set(img.size().height * img.size().width);
//  segment(img, segmented, k, method);
//  createOutputSegments(out, segmented, img.size().height, img.size().width);
//  std::cout << "out done" << std::endl;
//}