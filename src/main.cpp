#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define ZERO_INT 0

int main(int argc, char** argv){
	Mat original_image;

	if ( argc != 2 ){
		cout << "error: use <Image_Path> as argument\n" << endl;
		return -1;
	}

	original_image = cv::imread(argv[1], cv::IMREAD_COLOR);
	if (original_image.data != ZERO_INT){
		
	}
    	namedWindow("Original Image", WINDOW_AUTOSIZE );
    	imshow("Original Image", original_image );
    	waitKey(0);         
                                 
	return 0;
}
