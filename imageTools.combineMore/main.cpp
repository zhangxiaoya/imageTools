#include <opencv2/core/core.hpp>
#include <iostream>
#include <highgui/highgui.hpp>
#include <iomanip>
#include <imgproc/imgproc.hpp>

#include "googleFlag.h"

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

void PrepareVideoWriter(google::int32 ftps, cv::Size frameSize, std::string saveVideoName, cv::VideoWriter& videoWriter)
{
	if (FLAGS_compressFourCC.empty())
	{
		std::cout << "No Compress with MJPG" << std::endl;
	}

	if (FLAGS_compressFourCC == "yes")
	{
		videoWriter.open(saveVideoName, CV_FOURCC('M', 'J', 'P', 'G'), ftps, frameSize);
	}
	else
	{
		videoWriter.open(saveVideoName, -1, ftps, frameSize);
	}
}

void SetSeprateLine(const cv::Size& newFrameSize, cv::Mat& frame)
{
	auto ptr = frame.ptr<cv::Vec3b>(newFrameSize.height / 2);
	for (auto c = 0; c < newFrameSize.width; ++c)
	{
		ptr[c][0] = 255;
		ptr[c][1] = 255;
		ptr[c][2] = 255;
	}
}

int main(int argc, char** argv)
{
	std::string usageStr = "";
	usageStr += "Do combine two image lists to one video file.\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.combine --imagelist1=imageList1 folder --imageList2=imageList2 folder --saveFileName=videoName --ftps=10 --nameTemplate1=Frame_%08d --nameTemplate2=Frame_%08d --format1=.png --format2=.png --compressFourCC=yes/no.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_imageList1.empty() || FLAGS_imageList2.empty())
	{
		std::cout << "Must input two image list file" << std::endl;
	}
	else
	{
		auto ftps = FLAGS_ftps;

		char fileName1[buffer_count];
		auto imageListFormat1 = FLAGS_imageList1 + "\\" + FLAGS_nameTemplate1 + FLAGS_format1;
		sprintf_s(fileName1, buffer_count, imageListFormat1.c_str(), 0);

		char fileName2[buffer_count];
		auto imageListFormat2 = FLAGS_imageList2 + "\\" + FLAGS_nameTemplate2 + FLAGS_format2;
		sprintf_s(fileName2, buffer_count, imageListFormat2.c_str(), 0);

		auto img1 = cv::imread(fileName1);
		auto img2 = cv::imread(fileName1);
		if (img1.empty())
		{
			std::cout << fileName1 << std::endl;
			std::cout << "Cannot read the first image list file!" << std::endl;
			return -1;
		}
		if (img2.empty())
		{
			std::cout << fileName2 << std::endl;
			std::cout << "Cannot read the second image list file!" << std::endl;
			return -1;
		}
		auto frameSize1 = cv::Size(img1.cols, img1.rows);
		auto frameSize2 = cv::Size(img2.cols, img2.rows);
		if (frameSize1 != frameSize2)
		{
			std::cout << "Two frame size of two image list is not equal!" << std::endl;
			std::cout << "The frame size (width , height) of first image list is ( " << img1.cols << ", " << img1.rows << ")" << std::endl;
			std::cout << "The frame size (width , height) of second image list is ( " << img2.cols << ", " << img2.rows << ")" << std::endl;
			return -1;
		}
		auto convertImg1ToColor = false;
		if (img1.channels() == 1)
		{
			convertImg1ToColor = true;
		}

		auto convertImg2ToColor = false;
		if (img2.channels() == 1)
		{
			convertImg2ToColor = true;
		}

		auto newFrameSize = cv::Size(img1.cols, img1.rows * 2 + 1);

		std::string saveVideoName;
		if (FLAGS_saveFileName.empty())
		{
			ConstructSaveFileName(ftps, saveVideoName);
		}
		else
		{
			saveVideoName = FLAGS_saveFileName;
		}

		std::cout << "The saved file name is " << saveVideoName << std::endl;

		cv::VideoWriter video_writer;
		PrepareVideoWriter(ftps, newFrameSize, saveVideoName, video_writer);

		if (video_writer.isOpened())
		{
			auto index = 0;
			cv::Mat frame(newFrameSize,CV_8UC3);
			SetSeprateLine(newFrameSize, frame);

			cv::Mat colorImg1, colorImg2;
			while (true)
			{
				sprintf_s(fileName1, buffer_count, imageListFormat1.c_str(), index);
				img1 = cv::imread(fileName1);

				sprintf_s(fileName2, buffer_count, imageListFormat2.c_str(), index);
				img2 = cv::imread(fileName2);

				if (img1.empty() && img2.empty())
				{
					break;
				}

				if ((img1.empty() && img2.empty() != true) || (img1.empty() != true && img2.empty()))
				{
					std::cout << "Length of two image list is not equal!" << std::endl;
					break;
				}

				auto upRect = cv::Rect(0, 0, img1.cols, img1.rows);
				auto downRect = cv::Rect(0, img1.rows + 1, img2.cols, img2.rows);

				if (convertImg1ToColor)
				{
					cvtColor(img1, colorImg1, CV_GRAY2RGB);
				}
				else
				{
					colorImg1 = img1;
				}

				if (convertImg2ToColor)
				{
					cvtColor(img2, colorImg2, CV_GRAY2RGB);
				}
				else
				{
					colorImg2 = img2;
				}

				colorImg1.copyTo(frame(upRect));
				colorImg2.copyTo(frame(downRect));

				video_writer << frame;

				std::cout << "index : " << std::setw(4) << ++index << std::endl;
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
