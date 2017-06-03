#include <opencv2/core/core.hpp>
#include <gflags/gflags.h>
#include <iostream>
#include <highgui/highgui.hpp>
#include <iomanip>

DEFINE_string(imagelist, "", "The image list folder");
DEFINE_string(saveFileName, "", "The video file name to save");

int main(int argc,char ** argv)
{
	std::string usageStr = "";
	usageStr += "Do combine image list to one video file.\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.combine --imagelist=imageList folder --saveFileName=videoName.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_imagelist.empty())
	{
		std::cout << "Must input image file" << std::endl;
	}
	else if(FLAGS_saveFileName.empty())
	{
		std::cout << "Must in put save video file name" << std::endl;
	}
	else
	{
		char fileName[100];
		auto imageListFormat = FLAGS_imagelist + "\\resFrame_%04d.png";
		sprintf_s(fileName, 100, imageListFormat.c_str(),0);

		auto img = cv::imread(fileName);
		auto frameSize = cv::Size(img.cols, img.rows);

		auto saveVideoName = FLAGS_saveFileName;
		cv::VideoWriter video_writer(saveVideoName,-1,25,frameSize);

		if(video_writer.isOpened())
		{
			auto index = 1;
			while (true)
			{
				sprintf_s(fileName, 100, imageListFormat.c_str(), ++index);
				img = cv::imread(fileName);
				video_writer << img;

				std::cout << "index : " << std::setw(4) << index << std::endl;

				if(img.empty())
					break;
			}
			video_writer.release();
			std::cout << "Done!" << std::endl;
		}
		else
		{
			std::cout << "Save Video File Failed" << std::endl;
		}
	}
	return 0;
}