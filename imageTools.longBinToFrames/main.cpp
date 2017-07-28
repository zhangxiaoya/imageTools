#include <core/core.hpp>
#include <highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <imgproc/imgproc.hpp>

const auto ImageWidth = 320;
const auto ImageHeight = 256;
const auto FileNameBufferSize = 150;

void ConstitudePixel(uint8_t highPart, uint8_t lowPart, uint16_t& perPixel)
{
	perPixel = static_cast<uint16_t>(highPart);
	perPixel = perPixel << 8;
	perPixel |= lowPart;
}

void ChangeRows(int& row, int& col)
{
	col++;
	if (col == ImageWidth)
	{
		col = 0;
		row++;
	}
}

bool GetFileLength(std::ifstream& fin)
{
	fin.seekg(0, std::ios::end);
	auto len = fin.tellg();
	std::cout << "Length " << len << std::endl;
	if (len % (ImageWidth * ImageHeight * 2) != 0)
	{
		std::cout << "File Size is Not Match Image Size"<< std::endl;
		return false;
	}
	auto imageCount = len * 1.0 / (ImageHeight * ImageWidth * 2);
	std::cout << "Frame Number: " << imageCount << std::endl;
	fin.seekg(0, std::ios::beg);
	return true;
}

int FindMaxDiff(cv::Mat& img, uint16_t& min_val, uint16_t& max_val)
{
	uint16_t maxVal = 0;
	uint16_t minVal = 65535;

	for (auto r = 0; r < img.rows; ++r)
	{
		auto ptr = img.ptr<uint16_t>(r);
		for (auto c = 0; c < img.cols; ++c)
		{
			if(ptr[c] < 255)
				continue;
			if (maxVal < ptr[c])
				maxVal = ptr[c];
			if (minVal > ptr[c])
				minVal = ptr[c];
		}
	}

	min_val = minVal;
	max_val = maxVal;

	return static_cast<int>(maxVal - minVal);
}

void GenerateSmallFrame(cv::Mat& img, cv::Mat& smallImg, uint16_t min_val)
{
	for (auto r = 0; r < img.rows; ++r)
	{
		auto ptr = img.ptr<uint16_t>(r);
		auto smallPtr = smallImg.ptr<uchar>(r);

		for (auto c = 0; c < img.cols; ++c)
		{
			if (ptr[c] < 255)
				continue;
			auto val = ptr[c] - min_val;

			smallPtr[c] = val < 256 ? val : 255;
			smallPtr[c] = static_cast<uchar>(ceil(smallPtr[c] / 25.0) * 25);
		}
	}
}

int main(int argc, char* argv[])
{
	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_300m_jingzhi.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_300m_yundong.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_500m_jingzhi.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_500m_yundong.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_700m_jingzhi.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_700m_yundong.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_1000m_jingzhi.bin";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_1000m_yundong.bin";

	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_300m_jingzhi\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_300m_yundong\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_500m_jingzhi\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_500m_yundong\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_700m_jingzhi\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_700m_yundong\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_1000m_jingzhi\\Frame_08%d.png";
//	std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_1000m_yundong\\Frame_08%d.png";

	char fullImageFileName[FileNameBufferSize];

	auto frameIndex = 0;

	std::ifstream fin(fullBinFileName, std::fstream::in | std::fstream::binary);
	if (fin.is_open() == false)
	{
		std::cout << "Read Bin File Failed!" << std::endl;
		system("Pause");
		return -1;
	}
	if(GetFileLength(fin) == false)
	{
		system("Pause");
		return -1;
	}

	while (fin)
	{
		auto byteIndex = 0;
		auto rowIndex = 0;
		auto colIndex = 0;
		cv::Mat frame(ImageHeight, ImageWidth,CV_16UC1);
		cv::Mat smallframe(ImageHeight, ImageWidth, CV_8UC1);

		cv::Mat smallSmallframe(ImageHeight, ImageWidth, CV_8UC1);

		if (frameIndex == 999)
		{
			auto dummy = 1;
		}

		while (byteIndex < ImageHeight * ImageWidth * 2)
		{
			uint8_t highPart = fin.get();
			uint8_t lowPart = fin.get();

			uint16_t perPixel;
			ConstitudePixel(highPart, lowPart, perPixel);

			frame.at<int16_t>(rowIndex, colIndex) = perPixel;
			
			if (highPart < 0x13)
				smallSmallframe.at<uchar>(rowIndex, colIndex) = 0;
			else
				smallSmallframe.at<uchar>(rowIndex, colIndex) = lowPart;

			ChangeRows(rowIndex, colIndex);
			byteIndex += 2;
		}

		uint16_t maxVal = 0;
		uint16_t minVal = 0;

		auto diff = FindMaxDiff(frame, minVal, maxVal);
		std::cout << "Max Diff is " << diff << std::endl;

		GenerateSmallFrame(frame, smallframe, minVal);

		sprintf_s(fullImageFileName, FileNameBufferSize, fullImageFileNameFormat.c_str(), frameIndex);
		imwrite(fullImageFileName, frame);

		std::cout << "Current Index = " << std::setw(4)<< frameIndex <<std::endl;
		imshow("Curent Image", frame);

		cv::Mat lastImg;
		cv::equalizeHist(smallframe, lastImg);

		imshow("Small Frame", lastImg);

		imshow("Small Small Frame", smallSmallframe);

		cvWaitKey(1000);
		frameIndex++;
	}

	system("Pause");
	return 0;
}
