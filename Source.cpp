#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include<string>


using namespace cv;
using namespace std;

Mat src;
Mat src_gray;
Mat image_roi;

int thresh = 255;
int max_thresh = 255;
int Tumor_Total = 0;
int Tumor_Lessthan_25 = 0;
int Tumor_Greaterthen_25 = 0;
int Total_Tumor_Area = 0;
int Total_Tumor_Length = 0;
int Max_Tumor_Area = 0;
RNG rng(12345);

char filename[50];
string filestring;


void thresh_callback(int, void*);

int main(int argc, char** argv) {

	string path = "Images/Cancer_08.jpg";
	src = imread(path);
	cout << "Enter Image Name With Extention For Example (Cancer_08.jpg) :";
	/*cin >> filename;
	cout<<filename;
	filestring = "/Images/";
	filestring.append(filename);
	cout << filestring;
	src = imread(filestring);*/

	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
	Rect rectangle(60, 180, src_gray.cols - 150, src_gray.rows - 260);
	image_roi = src_gray(rectangle);

	namedWindow("Segmentation Focus", WINDOW_AUTOSIZE);
	imshow("Segmentation Focus", image_roi);

	const char* source_window = "Source";
	namedWindow(source_window, WINDOW_AUTOSIZE);

	imshow(source_window, src);
	createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
	thresh_callback(0, 0);

	waitKey(0);
	return 0;
}

void thresh_callback(int, void*) {
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	Canny(image_roi, canny_output, thresh, thresh * 2, 3);

	imshow("Segmented Lung", canny_output);

	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

	double  area;
	double clength;
	for (int i = 0;i < contours.size();i++)
	{
		if (hierarchy[i][2] != -1)
		{
			Tumor_Total = Tumor_Total + 1;
			area = contourArea(contours[i]);
			if (area > Max_Tumor_Area)
			{
				Max_Tumor_Area = area;
			}
			Total_Tumor_Area = Total_Tumor_Area + area;
			if (area <= 35)
			{
				Tumor_Lessthan_25 = Tumor_Lessthan_25 + 1;
			}
			else if (area > 35)
			{
				Tumor_Greaterthen_25 = Tumor_Greaterthen_25 + 1;
			}


			clength = arcLength(contours[i], true);
			Total_Tumor_Length = Total_Tumor_Length + clength;
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());

		}

	}


	cout << "Total Tumor : " << Tumor_Total << "\n";
	cout << "Total Tumot Area : " << Total_Tumor_Area << "\n";
	cout << "Total Tumor Length : " << Total_Tumor_Length << "\n";
	cout << "Tumor Area leass than 35 : " << Tumor_Lessthan_25 << "\n";
	cout << "Tumor Area Greater than 35 : " << Tumor_Greaterthen_25 << "\n";
	cout << "Maximum Tumor Area : " << Max_Tumor_Area << "\n";

	ofstream myfile;
	myfile.open("Tumorfile.txt");

	myfile << Tumor_Total << "\t" << Total_Tumor_Area << "\t" << Total_Tumor_Length << "\t" << Tumor_Lessthan_25 << "\t" << Tumor_Greaterthen_25 << "\t" << Max_Tumor_Area << " ?";

	myfile.close();

	namedWindow("Tumors", WINDOW_AUTOSIZE);
	imshow("Tumors", drawing);
}