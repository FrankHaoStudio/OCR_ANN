#pragma once
#ifndef _FEATURE_H_
#define _FEATURE_H_
#include "opencv2/opencv.hpp"
#include <vector>
#include <list>
#include <string>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;
class Feature
{
public:
	Feature();
	~Feature();
public:
	Mat CharFeatures(Mat in, int sizeData);
	Mat ProjectedHistogram(Mat img, int t);
	Rect GetCenterRect(Mat& in);
	Mat CutTheRect(Mat& in, Rect& rect);
	Mat NormalizedImage(Mat src,Mat dst);
	float countOfBigValue(Mat &mat, int iValue);
	static vector<string> GetFiles(const string &folder, const bool all=true);
	static string GetFileName(const string &path, const bool postfix=false);
	static size_t Get_last_slash(const string &path); 
	Mat RotateImg(Mat source, float angle);
	Mat TranslateImg(Mat img, int offsetx, int offsety);
	int CStringToInt(CString sourcedata);
	CString IntToCString(int sourcedata);
	char* CStringToCharpoint(CString sourcedata);
};

#endif  