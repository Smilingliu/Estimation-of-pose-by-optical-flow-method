#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <time.h>
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include<vector>
#include <iostream>
#include <ctype.h>
#include<string.h>
#include<Windows.h>
#include<mmsystem.h>
//#include "ransac.h"
#include<cstdlib>
#include<cstdio>
#include<ctime>  
#include <iomanip>  
#include <fstream> 

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	float sum = 0;
	vector<float> Y, A1, A2, A3, A4, A5, A6, vv;
	vector <Mat> X;
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 32, 0.02);//lk迭代终止条件
	Size subPixWinSize(10, 10), winSize(31, 31);
	string  rgb_time;
	const int MAX_COUNT =30;
	const int INIT = 0;
	const int FINA = 684;
	bool needToInit = false;
	bool nightMode = false;
	namedWindow("LK Demo", 1);
	vector<float> rgb_times;
	ifstream fin("pic/time.txt");
	ofstream fout("result22.txt");
	ofstream fout1("point1.txt");
	ofstream fout2("point2.txt");
	ofstream fout3("point3.txt");
	while (!fin.eof())
	{
		fin >> rgb_time;
		rgb_times.push_back(atof(rgb_time.c_str()));
	}
	std::cout << "rgbtime" << rgb_times.size() << endl;
	vector<Point2f> vecpoint2;
	Point2f point;
	vector<Point2f> point1;
	Point2f pt;
	vector<Point2f> points[2];
	vector<Point2f> vp;
	float vx, vy;//光流
	Mat framel;
	Mat frame;
	Mat depframe;
	/*Mat depframe_temp(260, 330, CV_16UC1);*/
	bool addRemovePt;
	Mat gray;
	Mat prevGray;
	float x, y;
	int u, v;
	float fx = 536.8, fy = 537.3, cx = 468.05, cy = 267.045;
	float Z;
	for (int h = INIT; h < FINA; h++)
	{
		A1.clear(); A2.clear(); A3.clear(); A4.clear(); A5.clear(); A6.clear(); Y.clear();		
		vp.clear();
		if (h % 20 == 0)
		{
			point1.clear();
			vp.clear();
			char filename1[50];
			sprintf(filename1, "H:/wl/color%d.jpg", h);
			framel = imread(filename1);
			Rect rect(360, 40, 330, 260);
			Mat frame1_temp(260, 330, CV_8UC3);
			frame1_temp = framel(rect);
			cvtColor(frame1_temp, frame1_temp, CV_BGR2GRAY);
			goodFeaturesToTrack(frame1_temp, point1, MAX_COUNT, 0.01, 5, Mat(), 5, 0, 0.04);
			cornerSubPix(frame1_temp, point1, subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = true;
			points[0].clear();
			points[1].clear();
		}

		char filename1[50];
		sprintf(filename1, "H:/wl/color%d.jpg", h);
		frame = imread(filename1);
		Rect rect(360, 40, 330, 260);
		Mat frame_temp(260, 330, CV_8UC3);
		frame_temp = frame(rect);
		cv::cvtColor(frame_temp, gray, COLOR_BGR2GRAY);

		char filename2[50];
		sprintf(filename2, "H:/wl/depth%d.png", h - 1);
		if (h > INIT)
		{
			depframe = imread(filename2, CV_LOAD_IMAGE_UNCHANGED);
			//depframe_temp = depframe(rect);
		}

		if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			size_t i, k;

			if (prevGray.empty())
			{
				gray.copyTo(prevGray);
				cout << "prevgray is empty" << h << endl;
			}
				
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
			for (int w = 0; w < points[1].size(); w++)
			{
				vp.push_back(points[1][w]);
			}
			cout << "******************************loop" << h << "***********************************" << endl;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if (!status[i])
					continue;
				if (status[i])
				{
					circle(frame_temp, points[1][i], 3, Scalar(0, 255, 0), -1, 8);
					u = points[0][i].x+360; v = points[0][i].y+40;
					/*cout << "uv=" << u <<"  "<< v << endl;*/
					x = (points[0][i].x+360 - cx) / fx; y = (points[0][i].y+40 - cy) / fy;
					Z = (float)*(depframe.ptr<ushort>(v) +u) / 1000.0;
					if (i == 1)
					{
						fout1 << x*Z << "   " << y*Z << "   " << Z << endl;
					}
					if (i == 2)
					{
						fout2 << x*Z << "   " << y*Z << "   " << Z << endl;
					}
					if (i == 3)
					{
						fout3 << x*Z << "   " << y*Z << "   " << Z << endl;
					}
					if (Z == 0)
						continue;
					vp[k++] = points[1][i];
					A1.push_back(-fx*x*y);
					A1.push_back(-fy - fy*y*y);
					A2.push_back(fx + fx*x*x);
					A2.push_back(fy*x*y);
					A3.push_back(-fx*y);
					A3.push_back(fy*x);
					A4.push_back(fx / Z);
					A4.push_back(0);
					A5.push_back(0);
					A5.push_back(fy / Z);
					A6.push_back(-fx*x / Z);
					A6.push_back(-fy*y / Z);
					/////////////
				    vx = 1.0*(points[1][i].x - points[0][i].x);//需要改成相机采集每帧图片所用的时间
					vy = 1.0*(points[1][i].y - points[0][i].y);

					Y.push_back(vx);
					Y.push_back(vy);
				}

			}
			vp.resize(k);
			/*cout << "A1.size=" << A1.size() << endl;*/
			int num_vec = A1.size();
			Mat mY(num_vec, 1, CV_32FC1);
			mY = Mat(Y, true);

			Mat mA1(num_vec, 1, CV_32FC1);
			mA1 = Mat(A1, true);
			Mat mA2(num_vec, 1, CV_32FC1);
			mA2 = Mat(A2, true);
			Mat mA3(num_vec, 1, CV_32FC1);
			mA3 = Mat(A3, true);
			Mat mA4(num_vec, 1, CV_32FC1);
			mA4 = Mat(A4, true);
			Mat mA5(num_vec, 1, CV_32FC1);
			mA5 = Mat(A5, true);
			Mat mA6(num_vec, 1, CV_32FC1);
			mA6 = Mat(A6, true);//要是不行的话需要converto

			Mat A, B, C, D, E;
			hconcat(mA1, mA2, A);
			hconcat(A, mA3, B);
			hconcat(B, mA4, C);
			hconcat(C, mA5, D);
			hconcat(D, mA6, E);

			Mat mX(6, 1, CV_32FC1);
			mX = ((E.t()*E).inv())*E.t()*mY;
			sum = sum + (double)sqrt(mX.at<float>(1, 0)*mX.at<float>(1, 0) + mX.at<float>(0, 0)*mX.at<float>(0, 0) + mX.at<float>(2, 0)*mX.at<float>(2, 0));
			float vertory = sum / (rgb_times[h] - rgb_times[0]);
			vv.push_back(vertory);
			cout << "v=" << vertory << endl;
			fout << vertory << endl;
			X.push_back(mX);

		}
		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{

			for (int i = 0; i < point1.size(); i++)
			{
				points[1].push_back(point1[i]);
			}
			addRemovePt = false;
		}
		cv::imshow("LK Demo", frame_temp);
		char c = (char)waitKey(10);
		if (c == 27)
		break;
		
		
		if (vp.size()>0)
		{
			for (int i = 0; i < vp.size(); i++)
			{
				points[1][i] = vp[i];
			}
			points[1].resize(vp.size());
		}
		
		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);

	}
	fout1.close();
	fout2.close();
	fout3.close();
	fin.close();
	return 0;
}
