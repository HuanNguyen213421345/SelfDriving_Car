#include <video.h>

CameraProcessor::CameraProcessor() : cap_(), frame_(), camIndex_()
{
} 

CameraProcessor::~CameraProcessor()
{
    std::cerr << "Destroying CameraProcessor Class!!!";
}

bool CameraProcessor::open()
{
    if(!cap_.isOpened()) cap_.open(camIndex_);
    return cap_.isOpened();
}

bool CameraProcessor::setup()
{
    if(!open()) return false;
    cap_.set(CAP_PROP_FRAME_WIDTH,320);
    cap_.set(CAP_PROP_FRAME_HEIGHT, 240);
    cap_.set(CAP_PROP_BRIGHTNESS, 50);
    cap_.set(CAP_PROP_CONTRAST, 50);
    cap_.set(CAP_PROP_SATURATION, 50);
    cap_.set(CAP_PROP_GAIN, 50);
    cap_.set(CAP_PROP_FPS, 0);	
    return true;
}

bool CameraProcessor::capture()
{
	bool success =cap_.read(frame_);
	if(!success){
		std::cerr << "Camera Capture Failed!!!" << std::endl;
		return false;
	}

	return true;
}

void CameraProcessor::show(const std::string& winName,const Mat& frameShow, int X, int Y)
{
	namedWindow(winName, WINDOW_NORMAL);
	moveWindow(winName, X, Y);
	resizeWindow(winName, 360, 240);
	Mat frameResized;
	resize(frameShow, frameResized, Size(360, 240));
	imshow(winName, frameResized);	
}

Mat& CameraProcessor::getFrame(){ return frame_;}

bool CameraProcessor::isOpened() {return cap_.isOpened();}
