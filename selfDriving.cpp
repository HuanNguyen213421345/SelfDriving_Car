#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <cstring>
#include "min.h"

using namespace std;
using namespace cv;
using namespace raspicam;

//Image Processing Variables
Mat frame, Matrix, framePers, frameGray, frameThresh, 
			frameEdge, frameFinal, frameFinalDuplicate, frameFinalDuplicate1;
Mat ROILane, ROILaneEnd;
int LeftLanePos, RightLanePos, laneCenter, laneEnd, frameCenter, Result, serial_fd;
uint8_t sta;

RaspiCam_Cv Camera;
struct min_context min_ctx;

stringstream ss;

vector<int> histrogramLane; 
vector<int> histrogramLaneEnd;

Point2f Source[] = {Point2f(25,160), Point2f(330,160), Point2f(0,210), Point2f(360,210)};
Point2f Destination[] = {Point2f(80,0), Point2f(280,0), Point2f(80,240), Point2f(280,240)};

//MachineLearning Variables
CascadeClassifier Stop_Cascade;
Mat frame_stop, ROI_Stop, gray_stop;
vector<Rect> Stop; 
int dist_stop;
uint8_t tx_buffer[3] ={0};

void min_tx_start(uint8_t port)
{
	
}
void min_tx_finished(uint8_t port)
{
	
}

uint16_t min_tx_space(uint8_t port)
{
	return 512;
}

void min_tx_byte(uint8_t port, uint8_t c)
{
	serialPutchar(serial_fd, c);
}

uint32_t min_time_ms(void)
{
	return millis();
}

void min_application_handler(uint8_t min_id,
		uint8_t const *min_payload, uint8_t len_payload, uint8_t port)
{

}

void serial_setup()
{
    if ((serial_fd = serialOpen("/dev/serial0", 9600)) < 0) {
        fprintf(stderr, "Unable to open serial device\n");
        exit(1);
    }
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Unable to start wiringPi\n");
        exit(1);
    }

}

	
void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
{
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv, 360 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv, 240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0));
    
}

void Perspective()
{
	line(frame,Source[0],Source[1],Scalar(0,0,255), 2);
	line(frame,Source[1],Source[3],Scalar(0,0,255), 2);
	line(frame,Source[3],Source[2],Scalar(0,0,255), 2);
	line(frame,Source[2],Source[0],Scalar(0,0,255), 2);
	
	//line(frame,Destination[0],Destination[1],Scalar(0,255,0), 2);
	//line(frame,Destination[1],Destination[3],Scalar(0,255,0), 2);
	//line(frame,Destination[3],Destination[2],Scalar(0,255,0), 2);
	//line(frame,Destination[2],Destination[0],Scalar(0,255,0), 2);
	
	Matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePers, Matrix, Size(360, 240));
}

void Threshold()
{
	cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	inRange(frameGray, 150, 255, frameThresh); //chinh thong so [1] de khu nhieu
	Canny(frameGray, frameEdge, 600, 700, 3, false);
	add(frameThresh, frameEdge, frameFinal);
	cvtColor(frameFinal,frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal,frameFinalDuplicate, COLOR_RGB2BGR); // use in histrogram only
	cvtColor(frameFinal,frameFinalDuplicate1, COLOR_RGB2BGR); // use in histrogram only

}

void HistrogramLane()
{
	histrogramLane.resize(400);
	histrogramLane.clear();
	
	for(int i = 0;i < frame.size().width; i++)
	{
		ROILane = frameFinalDuplicate(Rect(i,140,1,100));  //Toa do canh tren ben trai, toa do canh duoi ben phai
		divide(255, ROILane, ROILane);
		histrogramLane.push_back((int)(sum(ROILane)[0]));
	}	
	
	histrogramLaneEnd.resize(360);
	histrogramLaneEnd.clear();
	
	for(int i = 0;i < frame.size().width; i++)
	{
		ROILaneEnd = frameFinalDuplicate1(Rect(i,0,1,240));  //Toa do canh tren ben trai, toa do canh duoi ben phai
		divide(255, ROILaneEnd, ROILane);
		histrogramLaneEnd.push_back((int)(sum(ROILaneEnd)[0]));
	}
	
	laneEnd = sum(histrogramLaneEnd)[0];
	cout << "laneEnd = " << laneEnd << endl;
}

void LaneFinder()
{
	vector<int>::iterator LeftPtr;
	LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin() + 150);
	LeftLanePos = distance(histrogramLane.begin(), LeftPtr);
	
	vector<int>::iterator RightPtr;
	RightPtr = max_element(histrogramLane.begin() + 250, histrogramLane.end());
	RightLanePos = distance(histrogramLane.begin(), RightPtr);
	
	line(frameFinal, Point2f(LeftLanePos, 0), Point(LeftLanePos, 240), Scalar(0, 255, 0), 2);
	line(frameFinal, Point2f(RightLanePos, 0), Point(RightLanePos, 240), Scalar(0, 255, 0), 2);
}

void LaneCenter()
{
	laneCenter = (RightLanePos-LeftLanePos)/2 + LeftLanePos;
	frameCenter = 188;	
	line(frameFinal, Point2f(laneCenter,0), Point(laneCenter, 240), Scalar(0, 255, 0), 3);
	line(frameFinal, Point2f(frameCenter,0), Point(frameCenter, 240), Scalar(255, 0, 0), 3);
	
	Result = laneCenter - frameCenter;
	memset(tx_buffer, 0, sizeof(tx_buffer));
	if(Result > 0)
	{
		tx_buffer[0] = Result;
	}
	else tx_buffer[1] = -Result;
}

void Capture()
{
	
	bool success = Camera.grab();
	if(!success){
		cout << "Camera grab Failed" << endl;
	}
	Camera.retrieve(frame);	
	cvtColor(frame, frame, COLOR_BGR2RGB);
	cvtColor(frame, frame_stop, COLOR_BGR2RGB);
}

void Stop_detection()
{
	if( !Stop_Cascade.load("//home//pi//Desktop//MachineLearning//classifier//Stop_cascade.xml"))
	{
		cout << "Unable to open Stop_cascade.xml file" << endl;
		return ;
	}

	ROI_Stop = frame_stop(Rect(180,0,180,140));
	cvtColor(ROI_Stop, gray_stop, COLOR_RGB2GRAY);
	equalizeHist(gray_stop, gray_stop);
	Stop_Cascade.detectMultiScale(gray_stop, Stop);
	
	for(int i = 0; i < Stop.size(); i++)
	{
		Point Pi1(Stop[i].x, Stop[i].y);
		Point Pi2(Stop[i].x + Stop[i].width, Stop[i].y + Stop[i].height);
		
		rectangle(ROI_Stop, Pi1, Pi2, Scalar(0, 0, 255), 2);
		putText(ROI_Stop, "Stop Sign", Pi1, FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 2);
		dist_stop = (-0.167)*(Pi2.x-Pi1.x) + 36.67; // nay duoc tinh tu phuong trinh:
													// y = mx + c; D= 15; P2-P1 = 82 =>> 15 = 82*x + c;
		
		tx_buffer[2] = dist_stop;
		ss.str(" ");
		ss.clear();
		ss << "D = " << dist_stop << "cm";
		putText(ROI_Stop, ss.str(), Point2f(1, 130), 0, 1, Scalar(0,0,255), 2);
	}
}

int main(int argc, char **argv)
{
	
	Setup(argc, argv, Camera);
	serial_setup();
	min_init_context(&min_ctx, 0);
	cout << "Connecting to Camera"<<endl;
	if(!Camera.open())
	{
		cout << "Failed to connect"<<endl;
		return -1;
	}
	
	cout<<"Camera ID = "<<Camera.getId()<<endl;
	
	uint32_t t_time = 0;
	while(1)
	{
		
		auto start = std::chrono::system_clock::now();
	
		Capture();
		Perspective();
		Threshold();
		HistrogramLane();
		LaneFinder();
		LaneCenter();
		Stop_detection();
		
		if(millis() - t_time >= 200)
		{
			min_send_frame(&min_ctx, 0, tx_buffer, sizeof(tx_buffer));
			t_time = millis();
		}
		
		ss.str(" ");
		ss.clear();
		ss<<"Result = "<<Result;
		
		putText(frame, ss.str(), Point2f(1,50), 0, 1, Scalar(0,0,255), 2);
		
		namedWindow("orignal", WINDOW_KEEPRATIO);
		moveWindow("orignal", 0, 100);
		resizeWindow("orignal", 360, 240);
		imshow("orignal", frame);
		
		namedWindow("Perspective", WINDOW_KEEPRATIO);
		moveWindow("Perspective", 340, 100);
		resizeWindow("Perspective", 360, 240);
		imshow("Perspective",framePers);
		
		namedWindow("Final", WINDOW_KEEPRATIO);
		moveWindow("Final", 680, 100);
		resizeWindow("Final", 360, 240);
		imshow("Final", frameFinal);
		
		namedWindow("Stop Sign", WINDOW_KEEPRATIO);
		moveWindow("Stop Sign", 680, 400);
		resizeWindow("Stop Sign", 360, 240);
		imshow("Stop Sign", ROI_Stop);
		
		waitKey(30);
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		float t = elapsed_seconds.count();
		int FPS = 1/t;
		
		cout << "FPS = " << FPS << endl;
	}
	serialClose(serial_fd);
	return 0;
}