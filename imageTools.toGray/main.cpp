#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char** argv)
{
	auto img = cv::imread("timg.jpg");
	if(img.empty())
	{
		std::cout << "Open Image file failed" << std::endl;
	}
	else
	{
		cv::imshow("Test Image",img);
		std::cout << img.channels()<<std::endl;
		cv::waitKey(0);
		cv::destroyAllWindows();
	}
	system("Pause");
	return 0;
}