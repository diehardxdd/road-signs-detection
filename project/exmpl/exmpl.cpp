#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h> 
#include <iostream>  
#include <fstream>
#include <math.h>

#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat bgrImage, grayImage, gGrayImage, cGrayImage;
	bgrImage = imread("U:\\image\\3.jpg");

	cvtColor(bgrImage, grayImage, CV_RGB2GRAY);
	GaussianBlur(grayImage, gGrayImage, Size(5, 5), 5, 5);
	Canny(gGrayImage, cGrayImage, 50, 200, 3);
	
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);

	vector<Vec4f> lines_std;
	// Detect the lines
	ls->detect(cGrayImage, lines_std);

	// Show found lines
	Mat drawnLines(cGrayImage);
	ls->drawSegments(drawnLines, lines_std);
	vector<Point2f> corners;
	goodFeaturesToTrack(grayImage,corners,1, 0.01, 10, Mat(),3,false,0.04);
	for (int i = 0; i < corners.size(); i++)
	{
		circle(grayImage, corners[i], 4, 255, -1, 8, 0);
	}
	imshow("Standard refinement12", grayImage);
	imshow("Standard refinement1", cGrayImage);
	imshow("Standard refinement", drawnLines);
	waitKey();
	return 0;
}
