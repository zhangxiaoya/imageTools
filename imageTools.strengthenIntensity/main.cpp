#include <iostream>
#include <opencv2/core/core.hpp>
#include <highgui/highgui.hpp>

int main(int argc, char* argv[])
{
	std::cout << "This is Strengthen intensity"<<std::endl;

	auto img = cv::imread("E:\\WorkLogs\\Data\\Ir\\207\\Raw\\1_0-600m_150ms\\Frame_00000000.bmp");

	if(img.empty() == true)
	{
		std::cout << "Open Image File Failed!" << std::endl;
		system("Pause");
		return -1;
	}

	imshow("Before Strengthen Intensity", img);

	cv::waitKey(0);

	cv::destroyAllWindows();

	system("Pause");
	return 0;
}
