#include <opencv2/core/core.hpp>
#include "Functions.hpp"

// Polynomial regression function from lab sheet
cv::vector<double> fitPoly(cv::vector<cv::Point> points, int n)
{
  int nPoints = points.size();

  cv::vector<float> xValues = cv::vector<float>();
  cv::vector<float> yValues = cv::vector<float>();

  for (int i=0; i<nPoints; i++)
  {
    xValues.push_back(points[i].x);
    yValues.push_back(points[i].y);
  }

  double matrixSystem[n + 1][n + 2];

  for (int row=0; row < n+1; row++)
  {
    for (int col=0; col<n+1; col++)
    {
      matrixSystem[row][col] = 0;
      for (int i=0; i<nPoints; i++)
        matrixSystem[row][col] += pow(xValues[i], row + col);
    }

    matrixSystem[row][n+1] = 0;
    for (int i=0; i<nPoints; i++)
      matrixSystem[row][n+1] += pow(xValues[i], row) * yValues[i];
  }

  double coeffVec[n+2];

  for (int i=0; i<=n-1; i++)
    for (int k=i+1; k<=n; k++)
    {
      double t = matrixSystem[k][i]/matrixSystem[i][i];

      for (int j=0; j<=n+1; j++)
        matrixSystem[k][j] = matrixSystem[k][j] - t*matrixSystem[i][j];
    }

  for (int i=n; i>=0; i--)
  {
    coeffVec[i] = matrixSystem[i][n+1];
    for (int j=0; j<=n+1; j++)
      if (j!=i)
        coeffVec[i]=coeffVec[i]-matrixSystem[i][j] * coeffVec[j];

    coeffVec[i]=coeffVec[i]/matrixSystem[i][i];
  }

  cv::vector<double> result = cv::vector<double>();
  for (int i=0; i<n+1; i++)
    result.push_back(coeffVec[i]);

  return result;
}

cv::Point pointAtX(cv::vector<double> coeff, double x)
{
  double y = 0;
  for (int i=0; i<coeff.size(); i++)
    y+= pow(x, i) * coeff[i];

  return cv::Point(x, y);
}
