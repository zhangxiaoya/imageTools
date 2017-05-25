#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <gflags/gflags.h>

void split(const std::string& str, const std::string& delim, std::vector< std::string >& result)
{
	size_t last = 0;
	auto index = str.find_first_of(delim, last);

	while (index != std::string::npos)
	{
		result.push_back(str.substr(last, index - last));
		last = index + 1;
		index = str.find_first_of(delim, last);
	}
	if (index - last>0)
		result.push_back(str.substr(last, index - last));
}


DEFINE_string(srcImg, "timg.jpg", "The Color image's fullname");

int main(int argc, char** argv)
{
	std::string usageStr = "";
	usageStr += "Do convert RGB color image to gray image.\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.toGray --srcImg=rgb_image_fullname.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_srcImg.empty())
		std::cout << "Must input image file" << std::endl;
	else
	{
		const auto srcImageFullname = FLAGS_srcImg;
		auto srcImage = cv::imread(srcImageFullname);
		if (!srcImage.empty())
		{
			if (srcImage.channels() == 1)
				std::cout << "Input Image is not an color image!" << std::endl;
			else
			{
				cv::Mat destImg;
				cv::cvtColor(srcImage, destImg, CV_BGR2GRAY);

				std::vector<std::string> splitResult;

				split(srcImageFullname, ".", splitResult);
				auto grayImageName = splitResult[0] + "_gray.png";

				cv::imwrite(grayImageName, destImg);
			}
		}
		else
			std::cout << "Read image file failed!" << std::endl;
	}
	system("pause");
	return 0;
}