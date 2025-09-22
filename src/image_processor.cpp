#include <iostream>
#include <numeric>
#include "image_processor.h"

ImageProcessor::ImageProcessor() {}

void ImageProcessor::Perspective(const Mat& frame, const std::vector<Point2f> &source, const std::vector<Point2f> &destination)
{
		source_ = source;
		destination_ = destination;
		line(frame, source_[0],source_[1], Scalar(0,0,255),2);
		line(frame, source_[1],source_[3], Scalar(0,0,255),2);
		line(frame, source_[3],source_[2], Scalar(0,0,255),2);
		line(frame, source_[2],source_[0], Scalar(0,0,255),2);

		perspectiveMatrix_ = getPerspectiveTransform(source_, destination_);
		warpPerspective(frame, framePers_, perspectiveMatrix_, Size(360,240));
}

void ImageProcessor::Threshold(int lowerb_, int upperb_)
{
	cvtColor(framePers_, frameGray_, COLOR_RGB2GRAY);
	inRange(frameGray_, Scalar(lowerb_), Scalar(upperb_), frameThresh_);
	Canny(frameThresh_, frameEdge_, 600, 700, 3, false);
	add(frameThresh_, frameEdge_, frameFinal_);
	cvtColor(frameFinal_, frameFinal_, COLOR_GRAY2RGB);
	cvtColor(frameFinal_, frameFinalDuplicate_, COLOR_RGB2BGR);
}

void ImageProcessor::Histogram()
{
	histogramLane_.resize(400);
	histogramLane_.clear();

	for(int i = 0; i < 360; i++){
		RoiLane_ = frameFinalDuplicate_(Rect(i, 140, 1, 100));
		divide(255, RoiLane_, RoiLane_);
		histogramLane_.push_back((int)(sum(RoiLane_)[0]));
	}
}

void ImageProcessor::LaneFinder()
{
	std::vector<int>::iterator LeftPtr;
	LeftPtr = max_element(histogramLane_.begin(), histogramLane_.begin() + 150);
	LeftLanePos_ = distance(histogramLane_.begin(), LeftPtr);
	std::vector<int>::iterator RightPtr;
	RightPtr = max_element(histogramLane_.begin(), histogramLane_.begin() + 250);
	RightLanePos_ = distance(histogramLane_.begin(), RightPtr);

	line(frameFinal_, Point2f(LeftLanePos_, 0), Point2f(LeftLanePos_, 240), Scalar(0, 255, 0), 2);
	line(frameFinal_, Point2f(RightLanePos_, 0), Point2f(RightLanePos_, 240), Scalar(0, 255, 0), 2);
}

void ImageProcessor::LaneCenter()
{
	LaneCenter_ = (RightLanePos_ - LeftLanePos_) / 2 + LeftLanePos_;
	frameCenter_ = 320/2;
	
	line(frameFinal_, Point2f(LaneCenter_, 0), Point2f(LaneCenter_, 240), Scalar(0, 255 ,0), 2);
	line(frameFinal_, Point2f(frameCenter_, 0), Point2f(frameCenter_, 240), Scalar(0, 255,0), 2);

	Results_ = LaneCenter_ - frameCenter_;
}

void ImageProcessor::LineFinder()
{
	std::vector<int>::iterator LeftPtr;
	LeftPtr = min_element(histogramLane_.begin(), histogramLane_.begin() + 360);
	LeftLinePos_ = distance(histogramLane_.begin(), LeftPtr);
	std::vector<int>::reverse_iterator RightPtr;
	RightPtr = min_element(histogramLane_.rbegin(), histogramLane_.rbegin() + 360);
	RightLinePos_ = distance(histogramLane_.begin(), RightPtr.base());

	line(frameFinal_, Point2f(LeftLinePos_, 0), Point2f(LeftLinePos_, 240), Scalar(0, 255, 0), 2);
	line(frameFinal_, Point2f(RightLinePos_, 0), Point2f(RightLinePos_, 240), Scalar(0, 255, 0), 2);
}

void ImageProcessor::LineCenter()
{
	LineCenter_ = (RightLinePos_ + LeftLinePos_)/2;
	frameCenter_ = 360/2;

	line(frameFinal_, Point2f(LineCenter_, 0), Point2f(LineCenter_, 240), Scalar(0, 0 , 255), 2);
	line(frameFinal_, Point2f(frameCenter_, 0), Point2f(frameCenter_, 240), Scalar(0, 0, 255), 2);

	Results_ = LineCenter_ - frameCenter_;

}

Mat& ImageProcessor::getFramePers(){ return framePers_;}
Mat& ImageProcessor::getFrameFinalDup(){ return frameFinalDuplicate_;}
Mat& ImageProcessor::getFrameFinal(){ return frameFinal_;}
int ImageProcessor::getResults(){ return Results_;}

