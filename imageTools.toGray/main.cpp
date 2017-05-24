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


DEFINE_bool(big_menu, true, "Include 'advanced' options in the menu listing");
DEFINE_string(languages, "english,french,german",
	"comma-separated list of languages to offer in the 'lang' menu");


int main(int argc, char** argv)
{
	if (FLAGS_languages.find("english") != std::string::npos)
	{
	}

	if (argc < 2)
		std::cout << "Must input image file" << std::endl;
	else
	{
		std::cout<<argv[1]<<std::endl;
		auto srcImage = cv::imread(argv[1]);
		if (!srcImage.empty())
		{
			if (srcImage.channels() == 1)
				std::cout << "Input Image is not an color image!" << std::endl;
			else
			{
				cv::Mat destImg;
				cv::cvtColor(srcImage, destImg, CV_BGR2GRAY);

				std::string fullName = argv[1];
				std::vector<std::string> splitResult;

				split(fullName, ".", splitResult);
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