#include <cv.h>
#include <vector>

#include "ipoint.h"

void getMatches(IpVec &ipts1, IpVec &ipts2, IpPairVec &matches)
{
  float dist, d1, d2;
  Ipoint *match;

  matches.clear();

  for(unsigned int i = 0; i < ipts1.size(); i++) 
  {
    d1 = d2 = FLT_MAX;

    for(unsigned int j = 0; j < ipts2.size(); j++) 
    {
      dist = ipts1[i] - ipts2[j];  

      if(dist<d1) 
      {
        d2 = d1;
        d1 = dist;
        match = &ipts2[j];
      }
      else if(dist<d2) 
      {
        d2 = dist;
      }
    }

    if(d1/d2 < 0.65) 
    { 
      ipts1[i].dx = match->x - ipts1[i].x; 
      ipts1[i].dy = match->y - ipts1[i].y;
      matches.push_back(std::make_pair(ipts1[i], *match));
    }
  }
}

int translateCorners(IpPairVec &matches, const CvPoint src_corners[4], CvPoint dst_corners[4])
{
#ifndef LINUX
  double h[9];
  CvMat _h = cvMat(3, 3, CV_64F, h);
  std::vector<CvPoint2D32f> pt1, pt2;
  CvMat _pt1, _pt2;
  
  int n = (int)matches.size();
  if( n < 4 ) return 0;

  pt1.resize(n);
  pt2.resize(n);

  for(int i = 0; i < n; i++ )
  {
    pt1[i] = cvPoint2D32f(matches[i].second.x, matches[i].second.y);
    pt2[i] = cvPoint2D32f(matches[i].first.x, matches[i].first.y);
  }
  _pt1 = cvMat(1, n, CV_32FC2, &pt1[0] );
  _pt2 = cvMat(1, n, CV_32FC2, &pt2[0] );

  if(!cvFindHomography(&_pt1, &_pt2, &_h, CV_RANSAC, 5))  
    return 0;
  for(int i = 0; i < 4; i++ )
  {
    double x = src_corners[i].x, y = src_corners[i].y;
    double Z = 1./(h[6]*x + h[7]*y + h[8]);
    double X = (h[0]*x + h[1]*y + h[2])*Z;
    double Y = (h[3]*x + h[4]*y + h[5])*Z;
    dst_corners[i] = cvPoint(cvRound(X), cvRound(Y));
  }
#endif
  return 1;
}


