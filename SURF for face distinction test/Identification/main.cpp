#include <opencv2/opencv.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

// program uses SURF features to compare two faces as in www.newengineeringjournal.in/download/47/2-2-38.pdf
// SURF keypoints are calculated for both images and then matched using opencv api
// if the faces are cropped well and their resolution is similar, then the ratio of matches to the keypoints in original image is >0.3

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

void match(Mat face1, Mat face2)
{
	Ptr<SurfFeatureDetector> det = SurfFeatureDetector::create(1000);
	vector<KeyPoint> key1, key2;

	Ptr<SurfDescriptorExtractor> ext = SurfDescriptorExtractor::create();
	Mat desc1, desc2;

	cvtColor(face1, face1, COLOR_BGR2GRAY);

	resize(face2, face2, face1.size());
	cvtColor(face2, face2, COLOR_BGR2GRAY);

	normalize(face1, face1, 1, 0, NORM_MINMAX);
	normalize(face2, face2, 1, 0, NORM_MINMAX);
	equalizeHist(face1, face1);
	equalizeHist(face2, face2);

	det->detect(face1, key1);
	det->detect(face2, key2);

	Mat face1k, face2k;
	//drawKeypoints(face1, key1, face1k, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	//drawKeypoints(face2, key2, face2k, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	ext->compute(face1, key1, desc1);
	ext->compute(face2, key2, desc2);

	cout << desc1.size() << " " << key1.size() << endl;
	cout << desc2.size() << " " << key2.size() << endl;

	BFMatcher matcher(NORM_L2, 1);
	vector<DMatch> matches;
	matcher.match(desc1, desc2, matches);

	cout << "1: " << key1.size() << endl;
	cout << "2: " << key2.size() << endl;
	cout << "Matching: " << matches.size() << endl;

	double prediction = (double)matches.size() / (double)key1.size();

	cout << "Matching percentage: " << prediction << endl;
	
	Mat matchimg;
	drawMatches(face1, key1, face2, key2, matches, matchimg);

	//imshow("matches", matchimg);
	//waitKey(0);
}

int main()
{
	Mat base = imread("gary_base.png");
	Mat garyrot = imread("gary.png");
	Mat garyhp = imread("gary1.png");
	Mat garygs = imread("gary2.png");
	Mat brad = imread("brad.png");
	Mat leo = imread("leo.png");

	cout << "Original vs original" << endl;
	match(base, base);

	cout << endl << "Original vs rotated" << endl;
	match(base, garyrot);

	cout << endl << "Original vs HP" << endl;
	match(base, garyhp);

	cout << endl << "Original vs grayscale" << endl;
	match(base, garyrot);

	cout << endl << "Gary vs Brad" << endl;
	match(base, brad);

	cout << endl << "Gary vs Leo" << endl;
	match(base, leo);

	int exit;
	cin >> exit;

	return 0;
}
