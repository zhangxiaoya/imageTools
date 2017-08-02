#pragma once
static auto countX = 0;
static auto countY = 0;

static auto IMAGE_HEIGHT = 0;
static auto IMAGE_WIDTH = 0;

static auto imageIndex = 0;

const auto BLOCK_SIZE = 10;

const auto DISCRATED_BIN = 15;
const auto SHOW_DELAY = 0;

const auto BUFF_SIZE = 100;
//const auto imageListFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Third\\Raw\\1_0-600m_150ms\\Frame_%08d.bmp";
//const auto imageListFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Third\\Raw\\2_500-1500_150ms\\Frame_%08d.bmp";
const auto imageListFormat = "E:\\WorkLogs\\Projects\\Project4\\Data\\Third\\Raw\\3_1500m_100ms\\images\\Frame_%08d.png";

static char imageFullName[BUFF_SIZE];

inline void UpdateImageSize(cv::Mat& img)
{
	std::cout << "Update Image Size" << std::endl;

	IMAGE_WIDTH = img.cols;
	IMAGE_HEIGHT = img.rows;
	countX = ceil(static_cast<double>(IMAGE_WIDTH) / BLOCK_SIZE);
	countY = ceil(static_cast<double>(IMAGE_HEIGHT) / BLOCK_SIZE);
}