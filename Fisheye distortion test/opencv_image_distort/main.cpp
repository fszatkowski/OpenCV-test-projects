#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Matx33d K;
	K << 685.9644616537298, 0.0, 969.9887121786347, 0.0, 682.2969510421883, 1010.9140515134505, 0.0, 0.0, 1.0;
	Vec4d D;
	D << -0.021830238046820022, 0.0018477526860139503, -0.008062484741672862, 0.002183790781560894;
	Matx33d K_inv = K.inv();

	string path = "C://Users//fszat//source//repos//Pixel acess test//lena.jpg";
	Mat img = imread(path);
	Mat distorted_img = Mat::zeros(img.size(), CV_8UC3);
	Mat final_img = distorted_img.clone();

	vector<Point2f> srcp; //array of undistorded points
	vector<Point2f> dstp; //array of distorded points

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			srcp.push_back(Point2f(i, j));
		}
	}

	for (size_t i = 0; i < srcp.size(); i++) {
		Vec3d srcv = Vec3d(srcp[i].x, srcp[i].y, 1.0); //Creating a vector in homogeneous coords
		Vec3d dstv = K_inv * srcv; //Doing martix by vector multiplication
		srcp[i].x = dstv[0]; //Extracting resulting normalised x coord
		srcp[i].y = dstv[1]; //Extracting resulting normalised y coord
	}

	fisheye::distortPoints(srcp, dstp, K, D); //Performing distortion. D is distortion vector

	for (int i = 0; i < img.rows; i++)
	{
		for(int j = 0; j < img.cols; j++)
		{
			distorted_img.at<Vec3b>(dstp.at(j*img.cols+i)) = img.at<Vec3b>(i, j);
		}
	}

	if (distorted_img.empty())
	{
		cout << "Empty image" << endl;
	}
	
	Mat gray;
	cvtColor(distorted_img, gray, COLOR_BGR2GRAY);
	threshold(gray, gray, 1, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	int biggest = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		if (contourArea(contours[i]) > contourArea(contours[biggest]))
			biggest = i;
	}

	Rect ROI = boundingRect(contours[biggest]);
	Mat non_black_roi = distorted_img(ROI);

	resize(non_black_roi, final_img, final_img.size(), 0.0, 0.0, INTER_LINEAR);

	namedWindow("Undistorted", WINDOW_NORMAL);
	imshow("Undistorted", img);
	namedWindow("Distorted", WINDOW_NORMAL);
	imshow("Distorted", final_img);
	waitKey(0);

	imwrite("lena_distorted.jpg", final_img);

	return 0;
}
