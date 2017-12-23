#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Settings.h"


//�����ڴ�й©����
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
	//����ļ��Ƿ��
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
		return -1;
	}

	//���Settings�ֶ� ��Ҫ�Լ�ʵ��read()����
	fs["Settings"] >> s;

	//���Settings�ֶ��Ƿ�Ϸ�
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

		//����CHESSBOARD
		found = findChessboardCorners(view, s.boardSize, pointBuf,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		
		if (found) //����ҵ�����ȷ��Chess board
		{
			//��߽�������ľ���
			Mat viewGray;
			cvtColor(view, viewGray, COLOR_BGR2GRAY);
			cornerSubPix(viewGray, pointBuf, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			//�����ǵ�
			drawChessboardCorners(view, s.boardSize, Mat(pointBuf), found);
		}
		imshow("Image View", view);
		char key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

		if (key == ESC_KEY)
			break;
	}
	
}