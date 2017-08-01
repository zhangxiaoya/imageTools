#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <imgproc/imgproc.hpp>

const auto ImageWidth = 256;
const auto ImageHeight = 320;
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

void ChangeCols(int& row, int& col)
{
	row++;
	if (row == ImageHeight)
	{
		row = 0;
		col++;
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

void GenerateFrameByLowDifference(cv::Mat& originalImg, cv::Mat& lowDifferenceImg, uint16_t minVal)
{
	cv::Mat basicImg(ImageHeight, ImageWidth, CV_8UC1);
	for (auto r = 0; r < originalImg.rows; ++r)
	{
		auto ptr = originalImg.ptr<uint16_t>(r);
		auto smallPtr = basicImg.ptr<uchar>(r);

		for (auto c = 0; c < originalImg.cols; ++c)
		{
			if (ptr[c] < 255)
				continue;
			auto val = ptr[c] - minVal;
			smallPtr[c] = val < 256 ? val : 255;
			smallPtr[c] = static_cast<uchar>(ceil(smallPtr[c] / 25.0) * 25);
		}
	}
	equalizeHist(basicImg, lowDifferenceImg);
}

void GenerateLikeEqualizedFrame(const cv::Mat& basicFrame, cv::Mat& likeEqualizeFrame, uint16_t minVal, int diff)
{
	int pixelFrequency[256] = {0};
	double props[256] = {0.0};

	auto scale = (diff + 1) / 256.0;

	for (auto r = 0; r < ImageHeight; ++r)
	{
		auto ptr = basicFrame.ptr<uint16_t>(r);
		for (auto c = 0; c < ImageWidth; ++c)
		{
			if (static_cast<uint16_t>(ptr[c]) < minVal)
			{
				pixelFrequency[0] ++;
				continue;
			}
			auto pixelVal = static_cast<int>((static_cast<int>(ptr[c]) - static_cast<int>(minVal) + 0.5) / scale);

			if (pixelVal > 255)
			{
				std::cout << "Out of Range of Int8" << std::endl;
				break;
			}
			pixelFrequency[pixelVal] ++;
		}
	}

	for (auto i = 0; i < 256; ++i)
	{
		props[i] = double(pixelFrequency[i]) / double(ImageWidth * ImageHeight);

		if (i != 0)
			props[i] += props[i - 1];
	}

	uchar newPixels[256];
	for (auto i = 0; i < 256; ++i)
	{
		newPixels[i] = props[i] * 255;
	}

	for(auto r = 0;r < ImageHeight;++r)
	{
		auto dstPtr = likeEqualizeFrame.ptr<uchar>(r);
		auto srcPtr = basicFrame.ptr<uint16_t>(r);

		for (auto c = 0; c < ImageWidth; ++c)
		{
			if (static_cast<uint16_t>(srcPtr[c]) < minVal)
			{
				dstPtr[c] = newPixels[0];
				continue;
			}
			auto pixelVal = static_cast<int>((static_cast<int>(srcPtr[c]) - static_cast<int>(minVal) + 0.5) / scale);
			dstPtr[c] = newPixels[pixelVal];
		}
	}
}

int main(int argc, char* argv[])
{
	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_300m_jingzhi.bin";  std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_300m_jingzhi\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_300m_yundong.bin";	 std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_300m_yundong\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_500m_jingzhi.bin";	 std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_500m_jingzhi\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_500m_yundong.bin";	 std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_500m_yundong\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_700m_jingzhi.bin";	 std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_700m_jingzhi\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_700m_yundong.bin";	 std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_700m_yundong\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_1000m_jingzhi.bin"; std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_1000m_jingzhi\\Frame_%08d.png";
//	std::string fullBinFileName = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\ir_file_20170713_1000m_yundong.bin"; std::string fullImageFileNameFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Forth\\test\\Frames\\ir_file_20170713_1000m_yundong\\Frame_%08d.png";
	
	char fullImageFileName[FileNameBufferSize];

	auto frameIndex = 0;

	std::ifstream fin(fullBinFileName, std::fstream::in | std::fstream::binary);
	if (fin.is_open() == false)
	{
		std::cout << "Read Bin File Failed!" << std::endl;
		system("Pause");
		return -1;
	}

	if (GetFileLength(fin) == false)
	{
		system("Pause");
		return -1;
	}

	while (fin)
	{
		auto byteIndex = 0;
		auto rowIndex = 0;
		auto colIndex = 0;

		cv::Mat basicFrame(ImageHeight, ImageWidth,CV_16UC1);
		cv::Mat lowByteFrame(ImageHeight, ImageWidth, CV_8UC1);
		cv::Mat likeEqualizeFrame(ImageHeight, ImageWidth, CV_8UC1);
		cv::Mat lowDifferenceFrame;

		while (byteIndex < ImageHeight * ImageWidth * 2)
		{
			uint8_t highPart = fin.get();
			uint8_t lowPart = fin.get();

			uint16_t perPixel;
			ConstitudePixel(highPart, lowPart, perPixel);
			basicFrame.at<int16_t>(rowIndex, colIndex) = perPixel;

			if (highPart < 0x13) // for invalid pixels
				lowByteFrame.at<uchar>(rowIndex, colIndex) = 0;
			else
				lowByteFrame.at<uchar>(rowIndex, colIndex) = lowPart;

//			ChangeRows(rowIndex, colIndex);

			ChangeCols(rowIndex, colIndex);

			byteIndex += 2;
		}

		uint16_t maxVal = 0;
		uint16_t minVal = 0;

		auto diff = FindMaxDiff(basicFrame, minVal, maxVal);

		GenerateLikeEqualizedFrame(basicFrame, likeEqualizeFrame, minVal, diff);

		GenerateFrameByLowDifference(basicFrame, lowDifferenceFrame, minVal);

		sprintf_s(fullImageFileName, FileNameBufferSize, fullImageFileNameFormat.c_str(), frameIndex);
		imwrite(fullImageFileName, likeEqualizeFrame);

		std::cout << "Current Index = " << std::setw(4) << frameIndex << std::endl;

		imshow("Basic Frame", basicFrame);

		imshow("Low Difference Threshold Frame", lowDifferenceFrame);

		imshow("Low Part Original Pixel Frame", lowByteFrame);

		imshow("Like Equalized Frame", likeEqualizeFrame);

		cvWaitKey(1);
		frameIndex++;
	}

	system("Pause");
	return 0;
}
