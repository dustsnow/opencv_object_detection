#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;
int LowerH = 0;
int LowerS = 0;
int LowerV = 0;
int UpperH = 180;
int UpperS = 256;
int UpperV = 256;

int main(int argc, char** argv){
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout << "Cannot open camera" << endl;
		return -1;
	}
	namedWindow("Ball");
	createTrackbar("LowerH","Ball",&LowerH,180,NULL);
	createTrackbar("UpperH","Ball",&UpperH,180,NULL);
	createTrackbar("LowerS","Ball",&LowerS,256,NULL);
	createTrackbar("UpperS","Ball",&UpperS,256,NULL);
	createTrackbar("LowerV","Ball",&LowerV,256,NULL);
	createTrackbar("UpperV","Ball",&UpperV,256,NULL);

	Mat img_hsv,img_mask;

	while(1){
		Mat frame;
		//cap >> frame;
		frame = imread(argv[1],1);	
		if(!frame.data){return -1;}
		imshow("Origin",frame);

		// Color Detection
		cvtColor(frame, img_hsv, CV_BGR2GRAY);
		//inRange(img_hsv,cv::Scalar(LowerH,LowerS,LowerV), cv::Scalar(UpperH,UpperS,UpperV),img_mask);
		inRange(img_hsv,cv::Scalar(36,0,0), cv::Scalar(146,256,256),img_mask);
		//imshow("img_mask",img_mask);
		
		//Circle Detection
		Mat src, src_gray; 
		src = imread( argv[1], 1 );
		cvtColor(src, src_gray, CV_BGR2GRAY);	
		GaussianBlur(img_mask, img_mask, Size(9, 9), 2, 2);
		//GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		HoughCircles(img_mask, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 50, 0, 0);

		if(waitKey(30) >= 0) break;

		//Line Detection
	    src = imread(argv[1], 1);
	    if(src.empty())
	    {
	        cout << "can not open " <<  endl;
	        return -1;
	    }
	
	    Mat dst, cdst;
		Canny(img_mask,dst,50,200,3);
		//imshow("dst",dst);	
	    //Canny(src, dst, 50, 200, 3);
	    //cvtColor(dst, cdst, COLOR_GRAY2BGR);
	    vector<Vec2f> lines;
	    HoughLines(dst, lines, 1, CV_PI/180, 160, 0, 0 );
	
	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	        float rho = lines[i][0], theta = lines[i][1];
	        Point pt1, pt2;
	        double a = cos(theta), b = sin(theta);
	        double x0 = a*rho, y0 = b*rho;
	        pt1.x = cvRound(x0 + 1000*(-b));
	        pt1.y = cvRound(y0 + 1000*(a));
	        pt2.x = cvRound(x0 - 1000*(-b));
	        pt2.y = cvRound(y0 - 1000*(a));
	        line( src, pt1, pt2, Scalar(0,255,0), 3, CV_AA);
	    }
	 	///Draw the circles detected
    	for( size_t i = 0; i < circles.size(); i++ )
    	{
    	    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    	    int radius = cvRound(circles[i][2]);
    	    // circle center
    	    circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
    	    // circle outline
    	    circle( src, center, radius, Scalar(0,255,0), 3, 8, 0 );
    	}

		imshow("Detected", src);

	}
	return 0;
}
