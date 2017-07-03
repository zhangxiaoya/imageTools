#pragma once
static auto countX = 0;
static auto countY = 0;

static auto IMAGE_HEIGHT = 0;
static auto IMAGE_WIDTH = 0;

const auto BLOCK_SIZE = 10;

inline void UpdateImageSize(cv::Mat& img)
{
	std::cout << "Update Image Size" << std::endl;

	IMAGE_WIDTH = img.cols;
	IMAGE_HEIGHT = img.rows;
	countX = ceil(static_cast<double>(IMAGE_WIDTH) / BLOCK_SIZE);
	countY = ceil(static_cast<double>(IMAGE_HEIGHT) / BLOCK_SIZE);
}