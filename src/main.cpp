/*
 * 
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

// Tomator
//int LowerH = 169;
//int LowerS = 150;
//int LowerV = 130;
//int UpperH = 180;
//int UpperS = 256;
//int UpperV = 256;

//red-green-simple
//int LowerH = 0;
//int LowerS = 142;
//int LowerV = 0;
//int UpperH = 93;
//int UpperS = 256;
//int UpperV = 256;

//giant red circle 
int LowerH = 160;
int LowerS = 52;
int LowerV = 139;
int UpperH = 180;
int UpperS = 196;
int UpperV = 170;

int HC_Param1 = 200;
int HC_Param2 = 63;
int HL_Threshold = 90;
int HL_MinLineLength = 1;
Mat img_hsv,img_mask;
Mat frame;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

Mat drawing;
vector<vector<Point> > contours;
void thresh_callback(int,void*){
	Mat canny_output;
    vector<Vec4i> hierarchy;
    /// Detect edges using canny
    Canny( img_mask, canny_output, thresh, thresh*2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	vector<vector<Point> > contours_poly( contours.size() );
	for(int i = 0; i < contours.size(); i++){
		approxPolyDP( Mat(contours[i]), contours_poly[i],3,false);
	}

    /// Draw contours
    drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	   #if 0
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
	   #else
       drawContours( drawing, contours_poly, i, color, 2, 8, hierarchy, 0, Point() );
	   #endif
    }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    //imshow( "Contours", drawing );
    //vector<int> compression_params;
    //compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    //compression_params.push_back(9);
	//imwrite("contours.jpg",drawing,compression_params);
}
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
	createTrackbar("HL_MinLineLength","Control",&HL_MinLineLength,400,NULL);

}
void colorDetection(Mat &frame){
	// Color Detection
	cvtColor(frame, img_hsv, CV_BGR2HSV);
	inRange(img_hsv,cv::Scalar(LowerH,LowerS,LowerV), cv::Scalar(UpperH,UpperS,UpperV),img_mask);

}
void lineParallelChecker(vector<Vec4i> line1,vector<Vec4i> line2){

}
void pencilDetection(vector<Vec4i> &lines){
	cout << lines.size() << endl;
	for(int i = 0; i < lines.size()-1; i++){
		for(int j = 1; i < lines.size()-1; j++){
				
		}
	}
}
int main(int argc, char** argv){
	createTrackbars();
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout << "Cannot open camera" << endl;
		return -1;
	}

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

		////#if 1
		////Mat frame_gray;
		////cvtColor(frame,frame_gray,CV_BGR2GRAY);
		////blur(frame_gray,frame_gray,Size(3,3));
		//blur(img_mask,img_mask,Size(3,3));
		//namedWindow("Source",CV_WINDOW_AUTOSIZE);
		//imshow("Source", frame);
		//createTrackbar("Canny thresh:","Source",&thresh, max_thresh, thresh_callback);
		//thresh_callback(0,0);

		////waitKey(10);
		////#else

		// Color Detection
		colorDetection(frame);
		imshow("Img Mask", img_mask);


		//Circle Detection
		//cvtColor(frame,img_mask,CV_BGR2GRAY); //Using original image to detect circle and line
		//img_mask = drawing.clone();//using color-filtered image to detect circle and line
		//cvtColor(img_mask,img_mask,CV_BGR2GRAY);
		GaussianBlur(img_mask, img_mask, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		HoughCircles(img_mask, circles, CV_HOUGH_GRADIENT, 1, img_mask.rows/8, HC_Param1, HC_Param2, 0, 0);


		//Line Detection
	    Mat dst, cdst;
		Canny(img_mask,dst,10,200,3);
	    //vector<Vec2f> lines;
	    //HoughLines(dst, lines, 1, CV_PI/180, HL_Threshold, 0, 0 );
	    vector<Vec4i> lines;
	    HoughLinesP(dst, lines, 1, CV_PI/180, HL_Threshold, HL_MinLineLength, 40 );
		//pencilDetection(lines);

		#if 0
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
		#else
		for( size_t i = 0; i < lines.size(); i++ )
    	{
    		line( frame, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,255,0), 3, 8 );
    	}
		#endif

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

		imshow("Detected", frame);


		//#endif
		if(waitKey(10)>=0) break;
	}
	return 0;
}
