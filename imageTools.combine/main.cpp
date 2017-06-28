#include <opencv2/core/core.hpp>
#include <iostream>
#include <highgui/highgui.hpp>
#include <iomanip>

#include <gflags/gflags.h>

DEFINE_string(imageList, "", "The image list folder");
DEFINE_string(saveFileName, "", "The video file name to save");
DEFINE_int32(ftps, 10, "The video ftps to save");
DEFINE_string(nameTemplate, "Frame_%08d", "The video ftps to save");
DEFINE_string(format, ".png", "The images' format.");
DEFINE_string(compressFourCC, "no", "The images' format.");

const auto buffer_count = 300;

void ConstructSaveFileName(const google::int32 ftps, std::string& saveVideoName)
{
	saveVideoName = "video_ftps_";

	std::stringstream stream;
	stream << ftps;
	saveVideoName += stream.str();
	saveVideoName += "_";

	time_t td;
	time(&td);
	auto localtd = localtime(&td);

	const auto size_in_bytes = 256;
	char date_time[size_in_bytes];
	strftime(date_time, size_in_bytes, "%Y%m%d_%H%M%S", localtd);

	saveVideoName += date_time;

	saveVideoName += ".avi";
}

int main(int argc, char** argv)
{
	std::string usageStr = "";
	usageStr += "Do combine image list to one video file.\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.combine --imagelist=imageList folder --saveFileName=videoName --ftps=10 --nameTemplate=Frame_%08d --format=.png --compressFourCC=yes/no.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_imageList.empty())
	{
		std::cout << "Must input image file" << std::endl;
	}
	else if (FLAGS_compressFourCC.empty())
	{
		std::cout << "Compress with MJPG" << std::endl;
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
			std::cout << fileName << std::endl;
			std::cout << "Cannot read image file!" << std::endl;;
			return -1;
		}
		auto frameSize = cv::Size(img.cols, img.rows);

		std::string saveVideoName;
		if (FLAGS_saveFileName.empty())
		{
			ConstructSaveFileName(ftps, saveVideoName);
		}
		else
		{
			saveVideoName = FLAGS_saveFileName;
		}

		std::cout << saveVideoName << std::endl;

		cv::VideoWriter video_writer;
		if (FLAGS_compressFourCC.empty())
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
