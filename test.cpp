#include <iostream>
#include<stdio.h>
#include<math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include<iostream>
#include<fstream>
#include<string>
using namespace cv;
using namespace std;
int plot_counter=0,blink_counter=0;
const double PI = 3.14259;
double eye_x=40,eye_y=29;
Mat img(250, 250, CV_8UC3, Scalar(0,0, 0));
      
int ff = 0;

void MyLine( Mat img, Point start, Point end )
{
  int thickness = 2;
  int lineType = 24;
  line( img,
        start,
        end,
        Scalar( rand()%255 + 1, rand()%255 + 1, rand()%255 + 1 ),
        thickness,
        lineType );
   cv::imshow("image",img);  
}
int past_x=0,past_y=0,present_x,present_y;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	//cout << "in" << endl;
	if ( event == cv::EVENT_MOUSEMOVE )
	{
		if(ff==0){
			ff=1;
			present_x = x/4;
			present_y = y/4;
		}
		else{
			past_x = present_x;
			past_y = present_y;
			present_x = x/4;
			present_y = y/4;
			
		}
		//cout << past_x << "-" << past_y << "  " << present_x << "-" << present_y <<endl;
		MyLine( img, Point(past_x,past_y), Point(present_x,present_y) );
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
	
}


cv::Vec3f getEyeball(cv::Mat &eye, std::vector<cv::Vec3f> &circles)
{
  std::vector<int> sums(circles.size(), 0);
  for (int y = 0; y < eye.rows; y++)
  {
      uchar *ptr = eye.ptr<uchar>(y);
      for (int x = 0; x < eye.cols; x++)
      {
          int value = static_cast<int>(*ptr);
          for (int i = 0; i < circles.size(); i++)
          {
              cv::Point center((int)std::round(circles[i][0]), (int)std::round(circles[i][1]));
              int radius = (int)std::round(circles[i][2]);
              if (std::pow(x - center.x, 2) + std::pow(y - center.y, 2) < std::pow(radius, 2))
              {
                  sums[i] += value;
              }
          }
          ++ptr;
      }
  }
  int largestSum = 9999999;
  int largestSumIndex = -1;
  for (int i = 0; i < circles.size(); i++)
  {
      if (sums[i] < largestSum)
      {
          largestSum = sums[i];
          largestSumIndex = i;
      }
  }
  return circles[largestSumIndex];
}

cv::Rect getLeftmostEye(std::vector<cv::Rect> &eyes)
{
  int leftmost = 99999999;
  int leftmostIndex = -1;
  for (int i = 0; i < eyes.size(); i++)
  {
      if (eyes[i].tl().x < leftmost)
      {
          leftmost = eyes[i].tl().x;
          leftmostIndex = i;
      }
  }
  return eyes[leftmostIndex];
}

std::vector<cv::Point> centers;
cv::Point lastPoint;
cv::Point mousePoint;

//take the mean of the last five values to get the stabilized iris.
cv::Point stabilize(std::vector<cv::Point> &points, int windowSize)
{
  float sumX = 0;
  float sumY = 0;
  int count = 0;
  for (int i = std::max(0, (int)(points.size() - windowSize)); i < points.size(); i++)
  {
      sumX += points[i].x;
      sumY += points[i].y;
      ++count;
  }
  if (count > 0)
  {
      sumX /= count;
      sumY /= count;
  }
  return cv::Point(sumX, sumY);
}


//EYE AND FACE DETECTION:

void detectEyes(cv::Mat &frame, cv::CascadeClassifier &faceCascade, cv::CascadeClassifier &eyeCascade)
{
  cv::Mat grayscale;
  cv::cvtColor(frame, grayscale, CV_BGR2GRAY); // convert image to grayscale
  cv::equalizeHist(grayscale, grayscale); // enhance image contrast
  std::vector<cv::Rect> faces;

//---------face detection

  faceCascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(150, 150));
  if (faces.size() == 0) return; // none face was detected
  cv::Mat face = grayscale(faces[0]); // crop the face


//---------eye detection

 std::vector<cv::Rect> eyes;
 eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30)); // same thing as above
  rectangle(frame, faces[0].tl(), faces[0].br(), cv::Scalar(255, 0, 0), 2);
	//std::cout << faces[0].tl();
  if (eyes.size() != 2) return; // both eyes were not detected
  for (cv::Rect &eye : eyes)
  {

	rectangle(frame, faces[0].tl() + eye.tl(), faces[0].tl() + eye.br(), cv::Scalar(0, 255, 0), 2);
  }

//-------left-eye

  cv::Rect eyeRect = getLeftmostEye(eyes);
  cv::Mat eye = face(eyeRect); // crop the leftmost eye
  cv::equalizeHist(eye, eye);

//------hough - circles

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(eye, circles, CV_HOUGH_GRADIENT, 1, eye.cols / 8, 250, 15, eye.rows / 8, eye.rows / 3);
  if (circles.size() > 0)
  {
      cv::Vec3f eyeball = getEyeball(eye, circles);
      cv::Point center(eyeball[0], eyeball[1]);
      centers.push_back(center);
      center = stabilize(centers, 5);
      if (centers.size() > 1)
      {
          cv::Point diff;
          diff.x = (center.x - lastPoint.x) * 30;
          diff.y = (center.y - lastPoint.y) * 40;
          mousePoint += diff;
      }
      lastPoint = center;
      int radius = (int)eyeball[2];
      ofstream fout;
      ifstream fin;
      fin.open("plot.txt");
      fout.open("plot.txt",ios::app);
      //cout<<"radius: "<<radius<<endl;
      if(radius<=12){
      	fout << plot_counter << " " << 0 << endl;
      	plot_counter++;
      	/*if(radius<10){
      		blink_counter++;
      		cout<<blink_counter<<endl;
      	}
      	*/
      }
      cv::circle(frame, faces[0].tl() + eyeRect.tl() + center, radius, cv::Scalar(0, 0, 255), 2);
      cv::circle(eye, center, radius, cv::Scalar(255, 255, 255), 2);
      //cv::rectangle(eye,center.x,center.y,cv::Scalar(255,255,255),2);
      //std::cout << center.x << " " << center.y << std::endl;
      float angle = atan2(center.y-52,center.x-66)*180/PI;
      /*if(angle<0){
      	angle = 360 + angle;
      }
      */
      if(fin.is_open()){
      	fout << plot_counter << " " << angle << endl;
      	plot_counter++;
      }
      //std::cout << angle << " " << center.x << " " << center.y << std::endl;
  }
  cv::imshow("Eye", eye);
}


void changeMouse(cv::Mat &frame, cv::Point &location)
{
  if (location.x > frame.cols) location.x = frame.cols;
  if (location.x < 0) location.x = 0;
  if (location.y > frame.rows) location.y = frame.rows;
  if (location.y < 0) location.y = 0;
  //std::cout << location.x << " " << location.y;
  //printf("\n");
  
  system(("xdotool mousemove " + std::to_string(location.x) + " " + std::to_string(location.y)).c_str());
}


//main function
int main()
{
      cv::namedWindow("My Window", 1);
      cv::setMouseCallback("My Window", CallBackFunc, NULL); 
      cv::namedWindow("image",1);
      moveWindow("image",900,100);
      

  cv::CascadeClassifier faceCascade;
  cv::CascadeClassifier eyeCascade;
  if (!faceCascade.load("./haarcascade_frontalface_alt.xml"))
  {
      std::cerr << "Could not load face detector." << std::endl;
      return -1;
  }
  if (!eyeCascade.load("./haarcascade_eye.xml"))
  {
      std::cerr << "Could not load eye detector." << std::endl;
      return -1;
  }
  cv::VideoCapture cap(0); // the fist webcam connected to your PC
  if (!cap.isOpened())
  {
      std::cerr << "Webcam not detected." << std::endl;
      return -1;
  }
  cv::Mat frame;
  mousePoint = cv::Point(800, 800);
  while (1)
  {
      cap >> frame; // outputs the webcam image to a Mat
      //std::cout << frame << std::endl;
      if (!frame.data) break;
      detectEyes(frame, faceCascade, eyeCascade);
      changeMouse(frame, mousePoint);
      cv::imshow("My Window", frame); // displays the Mat
      if (cv::waitKey(30) >= 0) break;  // takes 30 frames per second. if the user presses any button, it stops from showing the webcam
  }
  return 0;
}
