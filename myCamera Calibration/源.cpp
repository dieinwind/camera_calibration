#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Settings.h"


//屏蔽内存泄漏警告
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace std;
using namespace cv;

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };

static void read(const FileNode& node, Settings& x, const Settings& default_value = Settings())
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

int main()
{

	Settings s;
	const string inputSettingsFile = "in_VID5.xml";
	FileStorage fs(inputSettingsFile, FileStorage::READ);
	const char ESC_KEY = 27;
	//检查文件是否打开
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
		return -1;
	}

	//输出Settings字段 需要自己实现read()函数
	fs["Settings"] >> s;

	//检查Settings字段是否合法
	if (!s.goodInput)
	{
		cout << "Invalid input detected. Application stopping. " << endl;
		return -1;
	}

	int mode = 0;
	Size imageSize;

	while (true)
	{
		Mat view;
		switch (mode)
		{
		case 0:
			cout << "DETECTION" << endl;
			view = s.nextImage();
			break;
		case 1:
			cout << "CALIBRATED" << endl;
			break;
		}

		imageSize = view.size();
		vector<Point2f> pointBuf;
		bool found;

		//查找CHESSBOARD
		found = findChessboardCorners(view, s.boardSize, pointBuf,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		
		if (found) //如果找到了正确的Chess board
		{
			//提高焦点坐标的精度
			Mat viewGray;
			cvtColor(view, viewGray, COLOR_BGR2GRAY);
			cornerSubPix(viewGray, pointBuf, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			//画出角点
			drawChessboardCorners(view, s.boardSize, Mat(pointBuf), found);
		}
		imshow("Image View", view);
		char key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

		if (key == ESC_KEY)
			break;
	}
	
}