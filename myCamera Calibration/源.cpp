#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <conio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Settings.h"
#include "calibration.h"


//屏蔽内存泄漏警告
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace std;
using namespace cv;

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };


//FileStorage重载的>>的实现
static void read(const FileNode& node, Settings& x, const Settings& default_value = Settings())
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}


Mat view;
Mat viewDrawed;
Calibration calibration;
Settings s;
bool isEnd = false;
//响应鼠标动作
void on_mouse(int event, int x, int y, int flags, void* ustc)
{
	if (event == CV_EVENT_LBUTTONDBLCLK)    //左键双击
	{
		view = s.nextImage();
		if (!view.empty())
		{
			cout << "RUN FOUND   ";
			bool found = calibration.findFeaturePoints(view, viewDrawed);
			cout << "  found =  " << found << "  " << endl;
			if (!viewDrawed.empty())
			{
				imshow("Drawed", viewDrawed);
			}
		}
		else
		{
			isEnd = true;
		}
		
			
	}
}

int main()
{
	
	const Scalar RED(0, 0, 255), GREEN(0, 255, 0);
	const string inputSettingsFile = "in_VID5.xml";
	FileStorage fs(inputSettingsFile, FileStorage::READ);
	const char ESC_KEY = 27;
	const char ENTER = 13;
	const char DOWN_KEY = 40;
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

	calibration.setParameters(s.boardSize,s.squareSize, s.flag);

	int mode = 0;
	bool found;
	Size imageSize;
	vector<Point2f> pointBuf;
	char key;

	//创建窗口
	namedWindow("show");
	cvSetMouseCallback("show", on_mouse, 0);
	VideoCapture cap(0);
	Mat frame;
	while (true)
	{

		cap>>frame;
		string msg = "out put test";
		int baseLine = 0;
		Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);
		
		putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);
		
		if (isEnd == true)
		{
			calibration.runCalibrationAndSave("out.xml");
			Mat image = imread("left01.jpg");
			Mat distorted;
			calibration.Undistort(image, distorted);
			imshow("left01", distorted);
		}

		imshow("show", frame);
		key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

		if (key == ESC_KEY)
			break;
	}
	
}