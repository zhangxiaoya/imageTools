#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iomanip>
#include <iterator>

const auto DELAY = 10;
const auto KernelSize = 3;
const auto NameBufferSize = 150;
const auto Bin = 15;

enum FieldType
{
	Four,
	Eight
};

const char* firstImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_1_8bit\\Frame_%04d.png";
//const char* firstImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_2_8bit\\Frame_%04d.png";

//const char* outputImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_1_8bit_maxFilter\\Frame_%04d.png";
//const char* outputImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_2_8bit_maxFilter\\Frame_%04d.png";

const char* outputImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_1_8bit_maxFilter_discrezated\\Frame_%04d.png";
//const char* outputImageList = "E:\\WorkLogs\\Gitlab\\ExtractVideo\\ExtractVideo\\ir_file_20170531_1000m_2_8bit_maxFilter_discrezated\\Frame_%04d.png";

uchar MaxOfVector(const std::vector<uchar>::iterator& begin, const std::vector<uchar>::iterator& end)
{
	auto maxResult = *begin;
	for (auto it = begin; it != end; ++it)
	{
		if (maxResult < *it)
			maxResult = *it;
	}
	return maxResult;
}

unsigned char GetMaxPixelValue(const cv::Mat& curFrame, int r, int c, int kernelSize)
{
	auto radius = kernelSize / 2;
	auto leftTopX = c - radius;
	auto leftTopY = r - radius;

	auto rightBottomX = leftTopX + 2 * radius;
	auto rightBottomY = leftTopY + 2 * radius;

	uchar maxVal = 0;

	for (auto row = leftTopY; row <= rightBottomY; ++row)
	{
		if (row >= 0 && row < curFrame.rows)
		{
			for (auto col = leftTopX; col <= rightBottomX; ++col)
			{
				if (col >= 0 && col < curFrame.cols && maxVal < curFrame.at<uchar>(row, col))
					maxVal = curFrame.at<uchar>(row, col);
			}
		}
	}

	return maxVal;
}

void MaxFilter(const cv::Mat& curFrame, cv::Mat& filtedFrame, int kernelSize)
{
	std::vector<uchar> pixelVector;

	for (auto r = 0; r < curFrame.rows; ++r)
	{
		for (auto c = 0; c < curFrame.cols; ++c)
		{
			pixelVector.clear();
			filtedFrame.at<uchar>(r, c) = GetMaxPixelValue(curFrame, r, c, kernelSize);
		}
	}
}

void RemoveInvalidPixel(cv::Mat curFrame)
{
	for (auto r = 0; r < 2; ++r)
		for (auto c = 0; c < 11; ++c)
			curFrame.at<uchar>(r, c) = 0;
}

void Discretization(const cv::Mat& filtedFrame, cv::Mat& discretizatedFrame, uint8_t bin)
{
	for (auto r = 0; r < filtedFrame.rows; ++r)
		for (auto c = 0; c < filtedFrame.cols; ++c)
			discretizatedFrame.at<uint8_t>(r, c) = (filtedFrame.at<uint8_t>(r, c) / bin) * bin;
}

int main(int argc, char* argv[])
{
	cv::VideoCapture video_capture;
	video_capture.open(firstImageList);

	char fileNameBuffer[NameBufferSize];

	cv::Mat curFrame;
	auto frameIndex = 0;

	if (video_capture.isOpened())
	{
		std::cout << "Open Image List Success!" << std::endl;

		while (!curFrame.empty() || frameIndex == 0)
		{
			video_capture >> curFrame;
			if (!curFrame.empty())
			{
				RemoveInvalidPixel(curFrame);

				imshow("Current Frame", curFrame);
				cv::waitKey(DELAY);

				cv::Mat filtedFrame(cv::Size(curFrame.cols, curFrame.rows), CV_8UC1);
				cv::Mat discretizatedFrame(cv::Size(curFrame.cols, curFrame.rows), CV_8UC1);

				MaxFilter(curFrame, filtedFrame, KernelSize);

				Discretization(filtedFrame, discretizatedFrame, Bin);

				imshow("Max Filter", filtedFrame);

				sprintf_s(fileNameBuffer, NameBufferSize, outputImageList, frameIndex);
				imwrite(fileNameBuffer, filtedFrame);

				std::cout << "Index : " << std::setw(4) << frameIndex << std::endl;
				++frameIndex;
			}
		}

		cv::destroyAllWindows();
	}
	else
	{
		std::cout << "Open Image List Failed" << std::endl;
	}

	system("pause");
	return 0;
}
