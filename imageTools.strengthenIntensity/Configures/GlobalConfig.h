#pragma once
static auto countX = 0;
static auto countY = 0;

static auto IMAGE_HEIGHT = 0;
static auto IMAGE_WIDTH = 0;

static auto imageIndex = 0;

const auto BLOCK_SIZE = 10;

const auto BUFF_SIZE = 100;
const auto imageListFormat = "E:\\WorkLogs\\Data\\Ir\\207\\Raw\\1_0-600m_150ms\\Frame_%08d.bmp";
static char imageFullName[BUFF_SIZE];

inline void UpdateImageSize(cv::Mat& img)
{
	std::cout << "Update Image Size" << std::endl;

	IMAGE_WIDTH = img.cols;
	IMAGE_HEIGHT = img.rows;
	countX = ceil(static_cast<double>(IMAGE_WIDTH) / BLOCK_SIZE);
	countY = ceil(static_cast<double>(IMAGE_HEIGHT) / BLOCK_SIZE);
}