#ifndef _MY_IMAGE_PROCESSOR_H_
#define _MY_IMAGE_PROCESSOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdint>

using namespace cv;

class ImageProcessor{
private:
	Mat framePers_, frameGray_, frameThresh_, frameEdge_, frameFinal_, frameFinalDuplicate_;
	std::vector<Point2f> source_, destination_;
	Mat perspectiveMatrix_;
	Mat RoiLane_;
	std::vector<int> histogramLane_;
	int LeftLanePos_, RightLanePos_, LaneCenter_, frameCenter_, Results_,
				LineCenter_, LeftLinePos_, RightLinePos_;
public:
	ImageProcessor();
	void Perspective(const Mat& frame, const std::vector<Point2f> &source, 
						const std::vector<Point2f> &destination);
	void Threshold(int lowerb_, int upperb_);
	void Histogram();
	void LaneFinder();
	void LaneCenter();
	void LineFinder();
	void LineCenter();
	Mat& getFramePers();
	Mat& getFrameFinalDup();
	Mat& getFrameFinal();
	int getResults();
};

#endif
