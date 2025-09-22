#include <chrono>
#include "video.h"
#include "image_processor.h"
#include "port_min.h"

int uart_fd;

//void uart_setup(){
//	if(gpioInitialise() < 0) {
//		std::cerr << "pigpio init failed!!!" << std::endl;
//		exit(1);
//	}
//	
//	uart_fd = serOpen((char*)"/dev/serial0", 115200, 0);
//	if(uart_fd < 0){
//		std::cerr << "Unable to open serial device with pigpio" << std::endl;
//		exit(1);
//	}
//	std::cerr << "Serial /dev/serial0 opened ai 115200 Baud (pigpio)" << std::endl;
//}

void uart_setup(unsigned int baud)
{
	if(uart_init(baud) < 0){
		std::cerr << "Uart init failed!!!" << std::endl;
		exit(0);
	}
	
//	if(gpioInitialise() < 0) {
//		std::cerr << "pigpio init failed!!!" << std::endl;
//		exit(1);
//	}
	
    std::cerr << "UART0 initialized at " << baud << " Baud (mmap)" << std::endl;
}

uint8_t getError(int Results_)
{
	if(Results_ >= -50 && Results_ <= 50) {return 3;}
	else if(Results_ >= -110 && Results_ < -50) {return 2;} 
	else if(Results_ >= -180 && Results_ < -110) {return 1;} 
	else if(Results_ < -180) {return 0;} 
	else if(Results_ > 50 && Results_ <= 110) {return 4;} 
	else if(Results_ > 110 && Results_ <= 180) {return 5;} 
	else if(Results_ > 180) {return 6;}	
	
	return 255;	
}

int main(){
	CameraProcessor camera;
	ImageProcessor image;
	uint8_t error;
	uint32_t time_ = 0;
	struct min_context min_ctx;
	std::vector<Point2f> source ={Point2f(25,160), Point2f(295,160), Point2f(0,210), Point2f(320,210)};
	std::vector<Point2f> destination = {Point2f(0,0), Point2f(360,0), Point2f(0,240), Point2f(360,240)};
	camera.setup();
	uart_setup(115200);
	min_init_context(&min_ctx, 0);
	
	while(1)
	{
		auto start = std::chrono::system_clock::now();
		
		camera.capture();
		image.Perspective(camera.getFrame(), source, destination);
		image.Threshold(25,255);
		image.Histogram();
		image.LineFinder();
		image.LineCenter();
		error = getError(image.getResults());
		camera.show("Original", camera.getFrame(), 0, 100);
		camera.show("Perspective", image.getFramePers(), 360, 100);
		camera.show("Final", image.getFrameFinal(), 720, 100);		
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		float t = elapsed_seconds.count();
		int FPS = 1/t;
		
		if(myTick()/1000 - time_ > 500)
		{
			min_send_frame(&min_ctx, 0, &error, sizeof(error));
//			uart_putc('A');
			time_ = myTick()/1000;
		}
		
		std::cout << "FPS= " << FPS << std::endl;
		std::cout << camera.getFrame().cols << "    " << camera.getFrame().rows << std::endl;
		std::cout << "Results = " << image.getResults() << std::endl;
		std::cout << "Error = " << static_cast<int>(error) <<std::endl;
		waitKey(30);
	}
	destroyAllWindows();
	return 0;

}
