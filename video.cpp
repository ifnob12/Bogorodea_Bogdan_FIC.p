#include <sstream>
#include <string>
#include <iostream>
//#include <opencv2\highgui.h>
#include "opencv2/highgui/highgui.hpp"
//#include <opencv2\cv.h>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


// rosu ( red )
#define H_MINR 0
#define H_MAXR 92
#define S_MINR 136
#define S_MAXR 239
#define V_MINR 249
#define V_MAXR 256

// verde ( green )
#define H_MINV 60
#define H_MAXV 76
#define S_MINV 30
#define S_MAXV 219
#define V_MINV 170
#define V_MAXV 250

// galben ( yellow )
#define H_MING
#define H_MAXG
#define S_MING
#define S_MAXG
#define V_MING
#define V_MAXG

// albastru ( blue )
#define H_MINA 92
#define H_MAXA 255
#define S_MINA 135
#define S_MAXA 255
#define V_MINA 176
#define V_MAXA 255

// alb ( white )
#define H_MINALB 92
#define H_MAXALB 255
#define S_MINALB 135
#define S_MAXALB 255
#define V_MINALB 176
#define V_MAXALB 255

// centru tablei ( black )
#define H_MINCEN 92
#define H_MAXCEN 255
#define S_MINCEN 135
#define S_MAXCEN 255
#define V_MINCEN 176
#define V_MAXCEN 255


using namespace std;
using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const std::string windowName = "Original Image";
const std::string windowName1 = "HSV Image";
const std::string windowName2 = "Thresholded Image";
const std::string windowName3 = "After Morphological Operations";
const std::string trackbarWindowName = "Trackbars";


void on_mouse(int e, int x, int y, int d, void *ptr)
{
	if (e == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}

void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed
}

string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(int x, int y, Mat &frame) {

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25 > 0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25 < FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25 > 0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25 < FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);
	//cout << "x,y: " << x << ", " << y;

}
void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area > MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				//cout << x << "," << y;
				drawObject(x, y, cameraFeed);

			}


		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}
int main(int argc, char* argv[])
{

	//some boolean variables for different functionality within this
	//program
	bool trackObjects = true;
	bool useMorphOps = true;

	Point p;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;
	//x and y values for the location of the object
	int x = 0, y = 0;
	//create slider bars for HSV filtering
	createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)


	capture.open("rtmp://172.16.254.63/live/live");


	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop


  int xc1;
  int xc2;
  int xc3;
  int xc4;
  int xc5;

  int yc1;
  int yc2;
  int yc3;
  int yc4;
  int yc5;

  // Socket socket = getSocket("TCP")
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = 20231;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("193.226.12.217");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        printf("ERROR connecting");

    // COMENZI - mai trebuie sa ma gandesc la un algoritm - cum dau comenzile?
    // as putea sa fac o functie cu prototipul:
    /* void send_command(int sockfd, char command[], int sleep_sec) // comanda se da astfel: send_command(sockfd,"f\n",2) f/r/b/l/s
       {
            int n;
            n = write(sockfd, command, 1);
            if(n<0)
            //echivalent
            //if(write(sockfd, command, 1)<0)
                printf("Error at writing to socket\n");
            sleep(sleep_sec);
       }
    */
    n = write(sockfd,"f\n",1);
    sleep(2);
        n = write(sockfd,"s\n",1);
            sleep(2);
        n = write(sockfd,"l\n",1);
            sleep(1);
        n = write(sockfd,"s\n",1);
        if (n < 0)
         printf("ERROR writing to socket");

	while (1) {


		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
    if(cameraFeed.empty())
    {
      return 1;
    }
    else
    {
		  cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		  //filter HSV image between values and store filtered image to
		  //threshold matrix
		  inRange(HSV, Scalar(H_MINV, S_MINV, V_MINV), Scalar(H_MAXV, S_MAXV, V_MAXV), threshold);
		  //perform morphological operations on thresholded image to eliminate noise
		  //and emphasize the filtered object(s)
		  if (useMorphOps)
			  morphOps(threshold);
		  //pass in thresholded frame to our object tracking function
		  //this function will return the x and y coordinates of the
		  //filtered object
		  if (trackObjects)
			  trackFilteredObject(x, y, threshold, cameraFeed);

      xc1 = x;
      yc1 = y;

      inRange(HSV, Scalar(H_MINA, S_MINA, V_MINA), Scalar(H_MAXA, S_MAXA, V_MAXA), threshold);
		  //perform morphological operations on thresholded image to eliminate noise
		  //and emphasize the filtered object(s)
		  if (useMorphOps)
			  morphOps(threshold);
		  //pass in thresholded frame to our object tracking function
		  //this function will return the x and y coordinates of the
		  //filtered object
		  if (trackObjects)
			  trackFilteredObject(x, y, threshold, cameraFeed);

      xc2 = x;
      yc2 = y;

      inRange(HSV, Scalar(H_MINR, S_MINR, V_MINR), Scalar(H_MAXR, S_MAXR, V_MAXR), threshold);
		  //perform morphological operations on thresholded image to eliminate noise
		  //and emphasize the filtered object(s)
		  if (useMorphOps)
			  morphOps(threshold);
		  //pass in thresholded frame to our object tracking function
		  //this function will return the x and y coordinates of the
		  //filtered object
		  if (trackObjects)
			  trackFilteredObject(x, y, threshold, cameraFeed);

      xc3 = x;
      yc3 = y;

      inRange(HSV, Scalar(H_MINALB, S_MINALB, V_MINALB), Scalar(H_MAXALB, S_MAXALB, V_MAXALB), threshold);
		  //perform morphological operations on thresholded image to eliminate noise
		  //and emphasize the filtered object(s)
		  if (useMorphOps)
			  morphOps(threshold);
		  //pass in thresholded frame to our object tracking function
		  //this function will return the x and y coordinates of the
		  //filtered object
		  if (trackObjects)
			  trackFilteredObject(x, y, threshold, cameraFeed);

      xc4 = x;
      yc4 = y;

      inRange(HSV, Scalar(H_MINCEN, S_MINCEN, V_MINCEN), Scalar(H_MAXCEN, S_MAXCEN, V_MAXCEN), threshold);
		  //perform morphological operations on thresholded image to eliminate noise
		  //and emphasize the filtered object(s)
		  if (useMorphOps)
			  morphOps(threshold);
		  //pass in thresholded frame to our object tracking function
		  //this function will return the x and y coordinates of the
		  //filtered object
		  if (trackObjects)
			  trackFilteredObject(x, y, threshold, cameraFeed);

      xc5 = x;
      yc5 = y;

      cout<<"Verde("<<xc1<<","<<" "<<yc1<<")"<<endl;
      cout<<"Albastru("<<xc2<<","<<" "<<yc2<<")"<<endl;
      cout<<"Rosu("<<xc3<<","<<" "<<yc3<<")"<<endl;
      cout<<"Alb("<<xc3<<","<<" "<<yc3<<")"<<endl;
      cout<<"Centru("<<xc3<<","<<" "<<yc3<<")"<<endl;

		  //show frames
		  imshow(windowName2, threshold);
		  imshow(windowName, cameraFeed);
		  // imshow(windowName1, HSV);
		  setMouseCallback("Original Image", on_mouse, &p);
		  //delay 30ms so that screen can refresh.
		  //image will not appear without this waitKey() command
		  waitKey(30);
    }
  }

	return 0;
}
