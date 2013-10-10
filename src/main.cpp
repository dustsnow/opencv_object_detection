#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int LowerH = 169;
int LowerS = 150;
int LowerV = 130;
int UpperH = 180;
int UpperS = 256;
int UpperV = 256;
int HC_Param1 = 200;
int HC_Param2 = 60;
int HL_Threshold = 100;
Mat img_hsv,img_mask;

void thresh_callback(int,void*);
void createTrackbars(){
	namedWindow("Control");
	createTrackbar("LowerH","Control",&LowerH,180,NULL);
	createTrackbar("UpperH","Control",&UpperH,180,NULL);
	createTrackbar("LowerS","Control",&LowerS,256,NULL);
	createTrackbar("UpperS","Control",&UpperS,256,NULL);
	createTrackbar("LowerV","Control",&LowerV,256,NULL);
	createTrackbar("UpperV","Control",&UpperV,256,NULL);
	createTrackbar("HC_Param1","Control",&HC_Param1,500,NULL);
	createTrackbar("HC_Param2","Control",&HC_Param2,450,NULL);
	createTrackbar("HL_Threshold","Control",&HL_Threshold,450,NULL);

}
void colorDetection(Mat &frame){
	// Color Detection
	cvtColor(frame, img_hsv, CV_BGR2HSV);
	inRange(img_hsv,cv::Scalar(LowerH,LowerS,LowerV), cv::Scalar(UpperH,UpperS,UpperV),img_mask);

}
int main(int argc, char** argv){
	createTrackbars();
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout << "Cannot open camera" << endl;
		return -1;
	}

	Mat frame;
	while(1){
		#if 1
		cap >> frame;
		#else 
		frame = imread(argv[1],1);	
		#endif
		if(!frame.data){
			cout << "frame capture error"<<endl;
			return -1;
		}
		// Color Detection
		colorDetection(frame);
		imshow("Img Mask", img_mask);
//		
//		//Circle Detection
//		Mat src, src_gray; 
//		src = imread( argv[1], 1 );
//		cvtColor(src, src_gray, CV_BGR2GRAY);	
		GaussianBlur(img_mask, img_mask, Size(9, 9), 2, 2);
//		GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		HoughCircles(img_mask, circles, CV_HOUGH_GRADIENT, 1, img_mask.rows/8, HC_Param1, HC_Param2, 0, 0);
//
//		//Line Detection
	    //src = imread(argv[1], 1);
	    //if(src.empty())
	    //{
	    //    cout << "can not open " <<  endl;
	    //    return -1;
	    //}
	
	    Mat dst, cdst;
		Canny(img_mask,dst,10,200,3);
		//imshow("dst",dst);	
//	    //Canny(src, dst, 50, 200, 3);
//	    //cvtColor(dst, cdst, COLOR_GRAY2BGR);
	    vector<Vec2f> lines;
	    HoughLines(dst, lines, 1, CV_PI/180, HL_Threshold, 0, 0 );
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
	        line( frame, pt1, pt2, Scalar(0,255,0), 3, CV_AA);
	    }
	 	///Draw the circles detected
    	for( size_t i = 0; i < circles.size(); i++ )
    	{
    	    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    	    int radius = cvRound(circles[i][2]);
    	    // circle center
    	    circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
    	    // circle outline
    	    circle( frame, center, radius, Scalar(0,255,0), 3, 8, 0 );
    	}

		//cout << "Line Detected: " << lines.size() << endl;
		//cout << "Circle Detected: " << circles.size() << endl;
		//imshow("Img Mask", img_mask);
		imshow("Detected", frame);

		if(waitKey(10)>=0) break;
	}
	return 0;
}
