/*
  COMP27112 - EX4
  Horizon Detection

  by Matt Walton, c10360mw
*/

#include <stdio.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include "Functions.hpp" // just move these to another file


cv::Mat orig_image, grey_image, edge_image, hough_image, horizon_image;
static int blur = 25;
static int canny_min = 50;
static int canny_max = 125;

static double max_size = 800;

static double rho = 2;
static double theta = CV_PI/180;
static int threshold = 53;
static int minLen = 100;
static int maxGap = 10;

void draw_horizon(cv::vector<cv::Point> points)
{
  // fit a quadratic polynomial to it
  cv::vector<double> coeffs = fitPoly(points, 2);

  // draw our predicted horizon
  // going to go across the screen with jumps of 5
  horizon_image = orig_image.clone();

  for (int x=0; x<horizon_image.cols; x+=2)
  {
    cv::circle(horizon_image, pointAtX(coeffs, x), 2, cv::Scalar(0, 255, 0));
  }

  imshow("horizon_image", horizon_image);
}

void hough(int pos, void*)
{
  cv::vector<cv::Vec4i> lines;
  cv::HoughLinesP(edge_image, lines, rho, theta, threshold, minLen, maxGap);

  hough_image = orig_image.clone();

  // filter vertical lines

  printf("Number of lines: %d\n", lines.size());

  for (int i=0; i<lines.size(); i++)
  {
    cv::Vec4i l = lines[i];
    if (abs(l[3] - l[1]) > abs(l[2] - l[0]))
    {
      lines.erase(lines.begin() + i);
    }
  }

  // draw lines & create vector of points
  cv::vector<cv::Point> points;

  for (int i=0; i<lines.size(); i++)
  {
    cv::Vec4i l = lines[i];
    cv::Point p1 = cv::Point(l[0], l[1]);
    cv::Point p2 = cv::Point(l[2], l[3]);

    cv::line(hough_image, p1, p2, cv::Scalar(0, 0, 255), 2, CV_AA);
    points.push_back(p1);
    points.push_back(p2);
  }

  imshow("hough_image", hough_image);

  draw_horizon(points);
}

void edge_detect(int, void*)
{
  // make sure we have a valid blur
  if (blur % 2 == 0)
    blur++;

  // blur
  cv::GaussianBlur(grey_image, edge_image, cv::Size(blur, blur), 0);
  cv::Canny(edge_image, edge_image, canny_min, canny_max);
  cv::imshow("edge_image", edge_image);

  // hough transformation


  hough(0, NULL);

}

int main(int argc, char** argv)
{
  char* imageName = "horizon1.jpg";
  if (argc > 1)
    imageName = argv[1];

  if (argc > 3)
  {
    canny_min = atoi(argv[2]);
    canny_max = atoi(argv[3]);
  }

  printf("OpenCV version: %d.%d\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);

  printf("ang: %lf\n", (double)atan2(9, 10));
  // load and convert to grayscale
  orig_image = cv::imread(imageName, CV_LOAD_IMAGE_COLOR);


  if (orig_image.data == NULL)
    printf("Failed to load %s!\n", imageName);
  else
  {
    // rescale image

    double scale = 1.0;

    if (orig_image.cols > orig_image.rows)
    {
      if (orig_image.cols > max_size)
        scale = max_size / (double)orig_image.cols;
    }
    else
      if (orig_image.rows > max_size)
      {
        scale = max_size / (double)orig_image.rows;
      }

    cv::resize(orig_image, orig_image, cv::Size(0, 0), scale, scale);

    cv::cvtColor(orig_image, grey_image, CV_BGR2GRAY);

    cv::namedWindow("original_image", CV_WINDOW_AUTOSIZE);
    cv::imshow("original_image", orig_image);

    cv::namedWindow("edge_image", CV_WINDOW_AUTOSIZE);
    cv::createTrackbar("blur", "edge_image", &blur, 51, &edge_detect, NULL);
    cv::createTrackbar("canny_min", "edge_image", &canny_min, 200, &edge_detect, NULL);
    cv::createTrackbar("canny_max", "edge_image", &canny_max, 200, &edge_detect, NULL);

    cv::namedWindow("hough_image", CV_WINDOW_AUTOSIZE);

    cv::createTrackbar("threshold", "hough_image", &threshold, 200, &hough, NULL);
    cv::createTrackbar("minLen", "hough_image", &minLen, 300, &hough, NULL);
    cv::createTrackbar("maxGap", "hough_image", &maxGap, 300, &hough, NULL);


    cv::namedWindow("horizon_image", CV_WINDOW_AUTOSIZE);

    edge_detect(0, NULL);

    cv::waitKey(0);
  }

  return 0;
}
