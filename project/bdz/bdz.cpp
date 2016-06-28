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

///Цикл и сохранения
//int main()
//{
//	Mat oimg;
//	Mat simg;
//	Mat binRed;
//	Mat binBlue;
//	string path;
//	for (int i = 0; i < 10; i++)
//	{
//		path = "C:\\Users\\student\\Desktop\\Test\\0000" + to_string(i) + ".ppm";
//		//загрузка картинки
//		oimg = imread(path);
//		cvtColor(oimg, simg, CV_RGB2HSV);
//
//		inRange(simg, Scalar(0, 50, 100), Scalar(33, 255, 255), binBlue);
//		inRange(simg, Scalar(50, 100, 100), Scalar(150, 255, 255), binRed);
//		vector<vector<Point>> conts;
//		Mat nbin;
//		binBlue.copyTo(nbin);
//		nbin += binRed;
//		Mat cont = Mat::zeros(nbin.size(), CV_8UC1);
//
//		findContours(nbin, conts, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//		drawContours(nbin, conts, -1, 255, 2, 8);
//
//		stringstream file;
//		file << "C:\\Users\\student\\Desktop\\migun\\build\\project\\bdz\\ex\\" << i << ".jpg";
//		imwrite(file.str(), nbin);
//	}
//	//отображение картинки
//	/*imshow("Original Image", oimg);
//	imshow("HSV Image", simg);
//	imshow("binRed", binRed);
//	imshow("binBlue", binBlue);
//	imshow("nbin", nbin);
//	imshow("cont", cont);
//	waitKey();
//	cvDestroyAllWindows();*/
//
//	return 0;
//}

double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int main()
{
	Mat bgrImage;
	Mat hsvImage;
	Mat binRed;
	Mat binBlue;

	bool morph = false;
	bool detectRed = false;
	bool detectBlue = false;
	bool detectYellow = false;
	bool showContours = false;
	bool detectSigns = false;

	Mat imageDetectRed;
	Mat imageDetectBlue;
	Mat imageDetectYellow;

	///загрузка картинки
	bgrImage = imread("U:\\image\\3.jpg");
	blur(bgrImage, bgrImage, Size(5, 5));
	cvtColor(bgrImage, hsvImage, CV_BGR2HSV);

	Mat low, high;

	inRange(hsvImage, Scalar(0, 100, 50), Scalar(10, 255, 255), low);
	inRange(hsvImage, Scalar(160, 100, 50), Scalar(180, 255, 255), high);
	addWeighted(low, 1.0, high, 1.0, 0.0, imageDetectRed, -1);
	inRange(hsvImage, Scalar(100, 110, 40), Scalar(125, 255, 255), imageDetectBlue);
	inRange(hsvImage, Scalar(20, 100, 100), Scalar(30, 255, 255), imageDetectYellow);


	vector<vector<Point>> conts;
	vector<vector<Point>> nconts;
	vector<Point> point(4);
	vector<vector<Point>> fconts;
	int maxx = 0, minx = 10000, maxy = 0, miny = 10000;

	Mat nbin;
	imageDetectRed.copyTo(nbin);
	nbin += imageDetectBlue;
	nbin += imageDetectYellow;
	Mat cont = Mat::zeros(nbin.size(), CV_8UC1);
	Mat ncont = Mat::zeros(nbin.size(), CV_8UC1);

	Canny(nbin, nbin, 10, 100, 5);
	findContours(nbin, conts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0));
	drawContours(cont, conts, -1, 255, 2, 8);
	double wh = bgrImage.rows * bgrImage.cols * 0.0008;

	///Первичное отсеивание контуров
	for (int i = 0; i < conts.size(); i++)//цикл по количеству контуров
	{
		if ((conts[i].size() >= 3) && contourArea(conts[i])>=wh)
		{
			nconts.push_back(conts[i]);
		}
	}

	///Определение области знака квадратом
	for (int i = 0; i < nconts.size(); i++)
	{
		for (int j = 0; j < nconts[i].size(); j++)
		{
			if (maxx <= nconts[i][j].x) maxx = nconts[i][j].x;
			if (minx >= nconts[i][j].x) minx = nconts[i][j].x;
			if (maxy <= nconts[i][j].y) maxy = nconts[i][j].y;
			if (miny >= nconts[i][j].y) miny = nconts[i][j].y;
		}
		point[0].x = minx, point[0].y = miny;
		point[1].x = minx, point[1].y = maxy;
		point[2].x = maxx, point[2].y = maxy;
		point[3].x = maxx, point[3].y = miny;

		fconts.push_back(point);
		maxx = 0, minx = 10000, maxy = 0, miny = 10000;
	}
	///конец
	
	///Круги
	vector<vector<Point>> ffconts;
	for (int i = 0; i < nconts.size(); i++)
	{
		double area = fabs(contourArea(nconts[i]));
		double perim = arcLength(nconts[i],true);
		// 1/4*CV_PI = 0,079577
		if (area / (perim * perim) > 0.07 && area / (perim * perim)< 0.097) 
		{ 
			ffconts.push_back(nconts[i]);
			nconts.erase(nconts.begin() + i);
			vector<vector<Point>> (nconts).swap(nconts);
			i--;
		}
	}
	
	///Тест
	//Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);

	//vector<Vec4f> lines_std;
	//// Detect the lines
	//ls->detect(image, lines_std);
	//double duration_ms = (double(getTickCount()) - start) * 1000 / getTickFrequency();
	//std::cout << "It took " << duration_ms << " ms." << std::endl;
	//// Show found lines
	//Mat drawnLines(image);
	//ls->drawSegments(drawnLines, lines_std);
	//imshow("Standard refinement", drawnLines);
	


	///Квадраты и треугольники
	//Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);
	//vector<Vec4f> lines_std;
	//drawContours(ncont, nconts, -1, 255, 2, 8);
	//ls->detect(ncont, lines_std);
	//ls->drawSegments(ncont, lines_std);

	///Отрисовка контуров
	//drawContours(bgrImage, nconts, -1, Scalar(0, 255, 0), 2, 8);
	//drawContours(bgrImage, ffconts, -1, Scalar(255, 0, 255), 2, 8);
	//drawContours(bgrImage, fconts, -1, Scalar(0, 0, 255), 2, 8);
	//drawContours(bgrImage, f1conts, -1, Scalar(255, 0, 0), 2, 8);
	drawContours(ncont, nconts, -1, 255, 2, 8);

	///Открытие изобажений
	imshow("Original Image", bgrImage);
	//imshow("HSV Image", hsvImage);
	//imshow("imageDetectRed", imageDetectRed);
	//imshow("imageDetectBlue", imageDetectBlue);
	//imshow("imageDetectYellow", imageDetectYellow);
	//imshow("nbin", nbin);
	//imshow("cont", cont);
 	imshow("ncont", ncont);
	waitKey();
	cvDestroyAllWindows();

	return 0;
}

///Вывод координат в файл
//ofstream fout("cppstudio.txt");
//for (int i = 0; i < nconts.size(); i++)//цикл по количеству контуров
//{
//	fout << "#" << i << endl;
//	for (int j = 0; j < nconts[i].size(); j++)
//	{
//		fout << nconts[i][j].x << ";" << nconts[i][j].y << endl;
//	}
//}
//fout.close();

///Перевод в градусы - (180 / 3.14159265358979323846*acos(0.5199));

///Квадрат
//int mx = INT_MIN, mix = INT_MAX, my1 = INT_MIN, miy1 = INT_MAX, my2 = INT_MIN, miy2 = INT_MAX;
//vector<Point> pnt;
//vector<vector<Point>> f1conts;
//for (int i = 0; i < nconts.size(); i++)//цикл по количеству контуров
//{
//	int mx = INT_MIN, mix = INT_MAX, my1 = INT_MIN, miy1 = INT_MAX, my2 = INT_MIN, miy2 = INT_MAX;
//	for (int j = 0; j < nconts[i].size(); j++)
//	{
//		if (nconts[i][j].x >= mx) mx = nconts[i][j].x;
//		if (nconts[i][j].x <= mix) mix = nconts[i][j].x;
//	}
//	for (int j = 0; j < nconts[i].size(); j++)
//	{
//		if ((nconts[i][j].x == mix) && (nconts[i][j].x <= miy1)) { miy1 = nconts[i][j].y; } //min,min
//		if ((nconts[i][j].x == mix) && (nconts[i][j].x >= my1)) { my1 = nconts[i][j].y; } //min,max
//		if ((nconts[i][j].x == mx) && (nconts[i][j].x >= my2)) { my2 = nconts[i][j].y; } //max,max
//		if ((nconts[i][j].x == mx) && (nconts[i][j].x <= miy2)) { miy2 = nconts[i][j].y; } //max,min
//	}
//	point[0].x = mix, point[0].y = miy1;
//	point[1].x = mix, point[1].y = my1;
//	point[2].x = mx, point[2].y = my2;
//	point[3].x = mx, point[3].y = miy2;

//	f1conts.push_back(point);
//}

///Поиск вершин
//vector<vector<Point>> ver;
//vector<Point> pnt;
//int x, y;
//for (int i = 0; i < nconts.size(); i++)//цикл по количеству контуров
//{
//	for (int j = 0; j < nconts[i].size(); j++)
//	{
//		if ((nconts[i][j] == fconts[i][0]) || (nconts[i][j] == fconts[i][1]) || (nconts[i][j] == fconts[i][2]) || (nconts[i][j] == fconts[i][3]))
//		{
//			x = nconts[i][j].x;
//			y = nconts[i][j].y;
//			pnt.push_back(Point(x,y));
//			ver.push_back(pnt);
//		}
//	}
//}