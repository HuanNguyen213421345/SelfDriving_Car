#ifndef _MY_VIDEO_H__
#define _MY_VIDEO_H_
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
class CameraProcessor{
private:
	VideoCapture cap_;							
	Mat frame_;
	int camIndex_;
	
public:
	CameraProcessor();
	~CameraProcessor();
	bool open();
	bool setup();
	bool capture();
	void show(const std::string &WinName, const Mat& frameShow, int X, int Y);
	
	Mat& getFrame();
	bool isOpened();
};

#endif 
