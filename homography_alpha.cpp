#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define EMPTY 0

Mat original_img, warped_img;
Mat h_matrix = cv::Mat::zeros(cv::Size(3, 3),CV_32FC1);
Mat a_matrix = cv::Mat::zeros(cv::Size(9, 8),CV_32FC1);
vector<Point2f> src_corners;
vector<Point2f> dest_corners;

void setDestinationCorners(){	
	int width = MAX(src_corners[1].x - src_corners[0].x, src_corners[2].x - src_corners[3].x);
	int height = MAX(src_corners[3].y - src_corners[0].y, src_corners[2].y - src_corners[1].y);
	dest_corners.push_back(Point2f(0,0));
	dest_corners.push_back(Point2f(width,0));
	dest_corners.push_back(Point2f(width,height));
	dest_corners.push_back(Point2f(0,height));
	cout << "Resulting image proportions " << width << " " << height << endl << endl;
	warped_img = cv::Mat::zeros(cv::Size(height, width),CV_32FC1);
}

void setAMatrix(){
	int i, j;
	float ux, uy, vx, vy;
	for (i = 0; i < 4; i++){
		ux = -(dest_corners[i].x * src_corners[i].x);
		uy = -(dest_corners[i].x * src_corners[i].y);
		vx = -(dest_corners[i].y * src_corners[i].x);
		vy = -(dest_corners[i].y * src_corners[i].y);
		a_matrix.at<float>(i*2,0) = src_corners[i].x;
		a_matrix.at<float>(i*2,1) = src_corners[i].y;
		a_matrix.at<float>(i*2,2) = 1;
		a_matrix.at<float>(i*2,3) = 0;
		a_matrix.at<float>(i*2,4) = 0;
		a_matrix.at<float>(i*2,5) = 0;
		a_matrix.at<float>(i*2,6) = ux;
		a_matrix.at<float>(i*2,7) = uy;
		a_matrix.at<float>(i*2,8) = -dest_corners[i].x;
		a_matrix.at<float>(i*2+1,0) = 0;
		a_matrix.at<float>(i*2+1,1) = 0;
		a_matrix.at<float>(i*2+1,2) = 0;
		a_matrix.at<float>(i*2+1,3) = src_corners[i].x;
		a_matrix.at<float>(i*2+1,4) = src_corners[i].y;
		a_matrix.at<float>(i*2+1,5) = 1;
		a_matrix.at<float>(i*2+1,6) = vx;
		a_matrix.at<float>(i*2+1,7) = vy;
		a_matrix.at<float>(i*2+1,8) = -dest_corners[i].y;
	}
	cout << "Resulting A-Matrix: " << endl << a_matrix << endl << endl;
}


void warpImage(){
	int i, j;
	Point2f dest;
	cv::Mat_<float> src(3,1);
	cv::Mat_<float> dst;
	// Soften the image

	// Set a discrete set of points based on a floating point operation (h * img)
	for (i = 0; i < original_img.rows; i++){
		for (j = 0; j < original_img.cols; j++){
			src(0,0)=float(i); 
			src(1,0)=float(j);
			src(2,0)=1.0;
			dst = h_matrix * src;
			dest.x = int(dst.at<float>(0,0));
			dest.y = int(dst.at<float>(1,0));
			//cout << dest.x << " " << dest.y << endl;
			//warped_img.at<float>(roundf(dest.x),roundf(dest.y)) = original_img.at<float>(i,j);
		}
	}
	//cout << warped_img << endl;

	// Interpolate the results as required
}


static void onMouse(int event, int x, int y, int flag, void* userdata){
	if (event == EVENT_LBUTTONDOWN && src_corners.size() < 4){
		// Push back puts things "in order", back being the last index
		src_corners.push_back(Point2f(x, y));
		cout << "Got click at position: " << src_corners.back() << endl;
	}
}

int main(int argc, char** argv){
	int i, j, last_col;
	Mat aux1, aux2, v_matrix;

	if (argc != 2){
		cout << "Error: use <Image_Path> as argument\n" << endl;
		return -1;
	}

	original_img = cv::imread(argv[1], cv::IMREAD_COLOR);
	if (original_img.data == EMPTY){
		cout << "Error: invalid image path.\n" << endl;
	}
	
    	namedWindow("Original Image", WINDOW_AUTOSIZE );
	setMouseCallback("Original Image",onMouse,0);
    	imshow("Original Image", original_img);
	while (src_corners.size() < 4){
		waitKey(0);
	}
	/* Set up destination corners */
	setDestinationCorners();
	/* Set up the linear transform matrix A for Ap = 0 */
	setAMatrix();
	/* Use SVD to estimate the homography matrix' values */
	cv::SVD::compute(a_matrix, aux1, aux2, v_matrix);
	last_col = v_matrix.cols;
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			h_matrix.at<float>(i,j) = v_matrix.at<float>(j+i,last_col-1);
		}
	}
	cout << "Resulting H-Matrix: " << endl << h_matrix << endl;
	/* Project and restore the image */
	warpImage(); 
	/* Display the results */
	namedWindow("Warped Image", WINDOW_AUTOSIZE );
	imshow("Warped Image", warped_img);  
	waitKey(0);
	return 0;
}
