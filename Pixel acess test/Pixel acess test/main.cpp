#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double pointer(Mat &img)
{
	uchar tab[256];
	for (int i = 0; i < 256; i++)
	{
		tab[i] = (i / 16) * 16;
	}

	CV_Assert(img.depth() == CV_8U);	//sprawdzamy, czy macierz jest typu unsigned char(8bit)

	int t1 = getTickCount();

	int channels = img.channels();		//pobieramy liczbe kanalow macierzy

	int nRows = img.rows;				//pobieramy liczbe kolumn macierzy
	int nCols = img.cols * channels;	//jesli macierz ma wiecej niz jeden kanal, to kazda kolumna zawiera kilka kanalow

	if (img.isContinuous())				//jesli macierz jest 'ciagla', to znaczy, ze ma jeden wiersz zawierajacy wszystkie kolumny
	{
		nCols *= nRows;
		nRows = 1;
	}

	uchar *p;							//tworzymy wskaznik
	for (int i = 0; i < nRows; i++)
	{
		p = img.ptr<uchar>(i);			//ustawiamy wskaznik na i-ty wiersz
		for (int j = 0; j < nCols; j++)
		{
			p[j] = tab[p[j]];			//ustawiamy wskaznik na j-ta kolumne
		}
	}

	int t2 = getTickCount();
	double time = (t2 - t1) / cv::getTickFrequency();
	return time;
}

double itr(Mat &img)
{
	uchar tab[256];
	for (int i = 0; i < 256; i++)
	{
		tab[i] = (i / 16) * 16;
	}

	CV_Assert(img.depth() == CV_8U);

	int t1 = getTickCount();

	int channels = img.channels();

	switch (channels)
	{
	case 1:
	{
		MatIterator_<uchar> iter, end;			//jesli macierz ma jeden kanal, tworzymy wskaznik typu uchar
		for (iter = img.begin<uchar>(), end = img.end<uchar>();		//pobieramy jego poczatek i koniec
			iter != end; ++iter)				//przechodzimy po wskazniku az do momentu, kiedy dojdziemy do konca
		{
			*iter = tab[*iter];
		}
		break;
	}
	case 3:
	{
		MatIterator_<Vec3b> iter, end;			//jesli macierz ma 3 kanaly, tworzymy wskaznik typu 3 elementowego wektora
		for (iter = img.begin<Vec3b>(), end = img.end<Vec3b>();
			iter != end; ++iter)
		{										//kazdemu z 3 kanalow przypisujemy wartosc
			(*iter)[0] = tab[(*iter)[0]];
			(*iter)[1] = tab[(*iter)[1]];
			(*iter)[2] = tab[(*iter)[2]];
		}
		break;
	}
	}

	int t2 = getTickCount();
	double time = (t2 - t1) / cv::getTickFrequency();
	return time;
}

double random_access(Mat &img)
{
	uchar tab[256];
	for (int i = 0; i < 256; i++)
	{
		tab[i] = (i / 16) * 16;
	}

	CV_Assert(img.depth() == CV_8U);

	int t1 = getTickCount();

	int channels = img.channels();

	switch (channels)
	{
	case 1:
	{
		for (int i = 0; i < img.rows; i++)
		{
			for (int j = 0; j < img.cols; j++)
			{		
				//w tym przypadku korzystamy z prostej funkcji .at(), ktorej specyfikujemy typ danych i podajemy adres w macierzy
				img.at<uchar>(i, j) = tab[img.at<uchar>(i, j)];	
			}
		}
		break;
	}
	case 3:
	{
		for (int i = 0; i < img.rows; i++)
		{
			for (int j = 0; j < img.cols; j++)
			{
				img.at<Vec3b>(i, j)[0] = tab[img.at<Vec3b>(i, j)[0]];
				img.at<Vec3b>(i, j)[1] = tab[img.at<Vec3b>(i, j)[1]];
				img.at<Vec3b>(i, j)[2] = tab[img.at<Vec3b>(i, j)[2]];
			}
		}
		break;
	}
	}

	int t2 = getTickCount();
	double time = (t2 - t1) / cv::getTickFrequency();
	return time;
}

double built_in(Mat &img)
{
	uchar tab[256];
	for (int i = 0; i < 256; i++)
	{
		tab[i] = (i / 16) * 16;
	}

	CV_Assert(img.depth() == CV_8U);

	int t1 = getTickCount();

	Mat table(1, 256, CV_8U);		//deklaracja macierzy o 256 elementach 8bitowych
	uchar* p = table.data;			//pobieramy wskaznik do poczatku tej macierzy
	for (int i = 0; i < 256; ++i)
		p[i] = tab[i];				//ustawiamy jej wartosci na odpowiednie wartosci z LUT
	LUT(img, table, img);			//funkcja pobiera macierz zrodlowa, LUT oraz macierz docelowa

	int t2 = getTickCount();
	double time = (t2 - t1) / cv::getTickFrequency();
	return time;
}

void measure_time(Mat &img)
{
	Mat test[20];
	for (int i = 0; i < 20; i++)
	{
		test[i] = img.clone();
	}

	double time_p = 0;
	double time_it = 0;
	double time_ra = 0;
	double time_oc = 0;

	for (int i = 0; i < 5; i++)
	{
		time_p += pointer(test[4 * i + 0]);
		time_it += itr(test[4 * i + 1]);
		time_ra += random_access(test[4 * i + 2]);
		time_oc += built_in(test[4 * i + 3]);
	}

	cout << "Sredni czas dla pointera: " << time_p / 5 << "ms" << endl;
	cout << "Sredni czas dla iteratora: " << time_it / 5 << "ms" << endl;
	cout << "Sredni czas dla wolnego dostepu: " << time_ra / 5 << "ms" << endl;
	cout << "Sredni czas dla wbudowanej funkcji OpenCV: " << time_oc / 5 << "ms" << endl;
	cout << endl;
}

int main()
{
	cout << "Czarno-bia³y obraz" << endl;

	cout << "800x600" << endl;
	Mat image1 = imread("lena.jpg", 0);
	resize(image1, image1, Size(800, 600));
	measure_time(image1);

	cout << "1280×720" << endl;
	Mat image2 = imread("lena.jpg", 0);
	resize(image2, image2, Size(1280, 720));
	measure_time(image2);

	cout << "1920×1080" << endl;
	Mat image3 = imread("lena.jpg", 0);
	resize(image3, image3, Size(1920, 1080));
	measure_time(image3);

	cout << "4096×2048" << endl;
	Mat image4 = imread("lena.jpg", 0);
	resize(image4, image4, Size(4096, 2048));
	measure_time(image4);

	cout << "Kolorowy obraz" << endl;

	cout << "800x600" << endl;
	Mat image5 = imread("lena.jpg");
	resize(image5, image5, Size(800, 600));
	measure_time(image5);

	cout << "1280×720" << endl;
	Mat image6 = imread("lena.jpg");
	resize(image6, image6, Size(1280, 720));
	measure_time(image6);

	cout << "1920×1080" << endl;
	Mat image7 = imread("lena.jpg", 0);
	resize(image7, image7, Size(1920, 1080));
	measure_time(image7);

	cout << "4096×2048" << endl;
	Mat image8 = imread("lena.jpg");
	resize(image8, image8, Size(4096, 2048));
	measure_time(image8);
	Mat test8[20];

	int exit;
	cin >> exit;


	return 0;
}
