#include <opencv2/core/core.hpp>
#include <gflags/gflags.h>
#include <iostream>
#include <highgui/highgui.hpp>
#include <iomanip>

DEFINE_string(imageList, "", "The image list folder");
DEFINE_string(saveFileName, "", "The video file name to save");
DEFINE_int32(ftps, 25, "The video ftps to save");
DEFINE_string(nameTemplate, "Frame_%04d", "The video ftps to save");
DEFINE_string(format, ".png", "The images' format.");
DEFINE_string(compressFourCC, "yes", "The images' format.");

const auto buffer_count = 300;

int main(int argc, char** argv)
{
	std::string usageStr = "";
	usageStr += "Do combine image list to one video file.\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.combine --imagelist=imageList folder --saveFileName=videoName --ftps=25 --nameTemplate=Frame_%04d --format=.png --compressFourCC=yes.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_imageList.empty())
	{
		std::cout << "Must input image file" << std::endl;
	}
	else if (FLAGS_saveFileName.empty())
	{
		std::cout << "Must in put save video file name" << std::endl;
	}
	else if(FLAGS_compressFourCC.empty())
	{
		std::cout << "Compress with MJPG"<<std::endl;
	}
	else
	{
		auto ftps = FLAGS_ftps;

		char fileName[buffer_count];
		auto imageListFormat = FLAGS_imageList + "\\\\" + FLAGS_nameTemplate + FLAGS_format;
		sprintf_s(fileName, buffer_count, imageListFormat.c_str(), 0);

		auto img = cv::imread(fileName);
		if (img.empty())
		{
			std::cout << fileName<<std::endl;
			std::cout << "Cannot read image file!" << std::endl;;
			return -1;
		}
		auto frameSize = cv::Size(img.cols, img.rows);

		auto saveVideoName = FLAGS_saveFileName;
		std::cout << saveVideoName <<std::endl;

		cv::VideoWriter video_writer;
		if(FLAGS_compressFourCC.empty())
			video_writer.open(saveVideoName, CV_FOURCC('M', 'J', 'P', 'G'), ftps, frameSize);
		else
			video_writer.open(saveVideoName, -1, ftps, frameSize);

		if (video_writer.isOpened())
		{
			auto index = 1;
			while (true)
			{
				sprintf_s(fileName, buffer_count, imageListFormat.c_str(), index++);
				img = cv::imread(fileName);

				if (img.empty())
					break;

				video_writer << img;

				std::cout << "index : " << std::setw(4) << index << std::endl;
			}
			video_writer.release();
			std::cout << "Done!" << std::endl;
		}
		else
		{
			std::cout << "Save Video File Failed" << std::endl;
		}
	}
	google::ShutDownCommandLineFlags();
	return 0;
}
