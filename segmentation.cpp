#include "segmentation.hpp"
#include "utils/disjoint-set.hpp"

static Edge createEdge(size_t i0, size_t j0, size_t i1, size_t j1, cv::Mat img)
{
  return Edge{Pixel{i0, j0, img.at<cv::Vec3b>(i0, j0)}, Pixel{i1, j1, img.at<cv::Vec3b>(i1, j1)}};
}

void run_segmentation(std::vector<std::vector<utils::Set<Pixel>>>& segments, std::list<Edge>& edges, uint8_t color, uint32_t k)
{
  edges.sort([color](Edge &first, Edge &second)
    {
    auto w1 = abs(first.a.color[color] - first.b.color[color]);
    auto w2 = abs(second.a.color[color] - second.b.color[color]);

    return w1 < w2; 
    });

  std::unordered_map<utils::Node<Pixel>, uint8_t> internal_diff;

  for (auto edge : edges)
  {
    utils::Set<Pixel>& segment1 = segments[edge.a.x][edge.a.y];
    utils::Set<Pixel>& segment2 = segments[edge.b.x][edge.b.y];
    if (segment1.Find() != segment2.Find())
    {
      auto diff1_iter = internal_diff.find(*segment1.Find());
      auto diff2_iter = internal_diff.find(*segment2.Find());
      uint8_t diff1_internal = diff1_iter == internal_diff.end() ? 0 : diff1_iter->second;
      uint8_t diff2_internal = diff2_iter == internal_diff.end() ? 0 : diff2_iter->second;
      float t1 = k / segment1.Size();
      float t2 = k / segment2.Size();

      float diff = diff1_internal + t1 < diff2_internal + t2 ? 
                      diff1_internal + t1 : diff2_internal + t2;

      uint8_t edge_weight = abs(edge.a.color[color] - edge.b.color[color]);

      if(edge_weight <= diff)
      {
        segment1.Union(segment2);
        auto rep = segment1.Find();
        internal_diff[*rep] = edge_weight;
      }
    }
  }
}

//intersection is saved in the first argument
void intersect(std::vector<std::vector<utils::Set<Pixel>>> &segments_red,
               std::vector<std::vector<utils::Set<Pixel>>> &segments_green,
               std::vector<std::vector<utils::Set<Pixel>>> &segments_blue)
{
  for (int i = 0; i < segments_red.size(); i++)
  {
    for (int j = 0; j < segments_red[0].size(); j++)
    {
      auto red_rep = segments_red[i][j].Find()->elem;
      auto green_rep = segments_green[i][j].Find()->elem;
      auto blue_rep = segments_blue[i][j].Find()->elem;

      if (red_rep != segments_red[green_rep.y][green_rep.x].Find()->elem ||
          red_rep != segments_red[blue_rep.y][blue_rep.x].Find()->elem)
      {
        segments_red[i][j] = utils::Set<Pixel>(red_rep);
      }
    }
  }
}

void createOutput(cv::Mat &out, std::vector<std::vector<utils::Set<Pixel>>> segments)
{
  auto colors = std::unordered_map<Pixel, cv::Vec3b>();

  for (int i = 0; i < segments.size(); i++){
    for (int j = 0; j < segments[0].size(); j++){
      auto rep = segments[i][j].Find()->elem;
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
    }
  }
}

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method)
{
  auto size = img.size();
  std::list<Edge> edges;

  // creating vector holding pixels as disjoint sets
  std::vector<std::vector<utils::Set<Pixel>>> segments(size.height);

  for (size_t i = 0; i < size.height; i++)
  {
    segments[i] = std::vector<utils::Set<Pixel>>(size.width);
    for (size_t j = 0; j < size.width; j++)
    {
      segments[i][j] = utils::Set<Pixel>(Pixel{i, j, img.at<cv::Vec3b>()});
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
  // TODO: deep copy of vectors
  auto blue_segments = segments;
  auto red_segments = segments;
  run_segmentation(red_segments, edges, 0, k);
  run_segmentation(segments, edges, 1, k);
  run_segmentation(blue_segments, edges, 2, k);

  intersect(red_segments, segments, blue_segments);

  createOutput(out, red_segments);
}