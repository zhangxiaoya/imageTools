#include <iostream>
#include <opencv2/core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

#include "Models/DifferenceElem.hpp"
#include "Configures/GlobalConfig.h"
#include "Utils/GeneralUtils.hpp"
#include <iomanip>

#define DEBUG 1;

std::vector<std::vector<uchar>> GetMaxMinPixelValueDifferenceMap(cv::Mat& curFrame)
{
	std::vector<std::vector<uchar>> maxmindiff(countY, std::vector<uchar>(countX, 0));
	for (auto br = 0; br < countY; ++br)
	{
		auto height = br == (countY - 1) ? IMAGE_HEIGHT - (countY - 1) * BLOCK_SIZE : BLOCK_SIZE;
		for (auto bc = 0; bc < countX; ++bc)
		{
			auto width = bc == (countX - 1) ? IMAGE_WIDTH - (countX - 1) * BLOCK_SIZE : BLOCK_SIZE;
			maxmindiff[br][bc] = GeneralUtil::GetMaxValueOfBlock(curFrame(cv::Rect(bc * BLOCK_SIZE, br * BLOCK_SIZE, width, height))) - GeneralUtil::GetMinValueOfBlock(curFrame(cv::Rect(bc * BLOCK_SIZE, br * BLOCK_SIZE, width, height)));
		}
	}
	return maxmindiff;
}

void GetMaxValueOfMatrix(const std::vector<std::vector<uchar>>& maxmindiff, DifferenceElem& diffElem)
{
	for (auto br = 0; br < countY; ++br)
	{
		for (auto bc = 0; bc < countX; ++bc)
		{
			if (diffElem.diffVal < static_cast<int>(maxmindiff[br][bc]))
			{
				diffElem.diffVal = static_cast<int>(maxmindiff[br][bc]);
				diffElem.blockX = bc;
				diffElem.blockY = br;
			}
		}
	}
}

void GetDiffValueOfMatrixBigThanThreshold(const std::vector<std::vector<uchar>>& maxmindiff, std::vector<DifferenceElem>& diffElemVec)
{
	diffElemVec.clear();
	for (auto br = 0; br < countY; ++br)
	{
		for (auto bc = 0; bc < countX; ++bc)
		{
			if (3 <= static_cast<int>(maxmindiff[br][bc]))
			{
				DifferenceElem diffElem;
				diffElem.blockX = bc;
				diffElem.blockY = br;
				diffElem.diffVal = static_cast<int>(maxmindiff[br][bc]);
				diffElemVec.push_back(diffElem);
			}
		}
	}
}

std::vector<DifferenceElem> GetMostMaxDiffBlock(const std::vector<std::vector<uchar>>& maxmindiff)
{
	std::vector<DifferenceElem> mostPossibleBlocks;

	//	DifferenceElem diffElem;
	//	GetMaxValueOfMatrix(maxmindiff, diffElem);
	//	mostPossibleBlocks.push_back(diffElem);

	GetDiffValueOfMatrixBigThanThreshold(maxmindiff, mostPossibleBlocks);

	return mostPossibleBlocks;
}


void StrengthenIntensityOfBlock(cv::Mat& currentGrayFrame)
{
	auto maxmindiffMatrix = GetMaxMinPixelValueDifferenceMap(currentGrayFrame);
#ifdef  DEBUG
	for (auto r = 0; r < countY; ++r)
	{
		for (auto c = 0; c < countX; ++c)
		{
			std::cout << std::setw(3) << static_cast<int>(maxmindiffMatrix[r][c]);
		}
		std::cout << std::endl;
	}
#endif

	auto differenceElems = GetMostMaxDiffBlock(maxmindiffMatrix);

	for (auto elem : differenceElems)
	{
#ifdef  DEBUG
		std::cout << " Block X = " << elem.blockX << " Block Y = " << elem.blockY << " Diff Value = " << elem.diffVal << std::endl;
#endif

		auto centerX = elem.blockX * BLOCK_SIZE + BLOCK_SIZE / 2;
		auto centerY = elem.blockY * BLOCK_SIZE + BLOCK_SIZE / 2;
		auto boundingBoxLeftTopX = centerX - BLOCK_SIZE >= 0 ? centerX - BLOCK_SIZE : 0;
		auto boundingBoxLeftTopY = centerY - BLOCK_SIZE >= 0 ? centerY - BLOCK_SIZE : 0;
		auto boundingBoxRightBottomX = centerX + BLOCK_SIZE < IMAGE_WIDTH ? centerX + BLOCK_SIZE : IMAGE_WIDTH - 1;
		auto boundingBoxRightBottomY = centerY + BLOCK_SIZE < IMAGE_HEIGHT ? centerY + BLOCK_SIZE : IMAGE_HEIGHT - 1;
		auto averageValue = GeneralUtil::CalculateAverageValue(currentGrayFrame, boundingBoxLeftTopX, boundingBoxLeftTopY, boundingBoxRightBottomX, boundingBoxRightBottomY);

		auto maxdiffBlockRightBottomX = (elem.blockX + 1) * BLOCK_SIZE > IMAGE_WIDTH ? IMAGE_WIDTH - 1 : (elem.blockX + 1) * BLOCK_SIZE;
		auto maxdiffBlockRightBottomY = (elem.blockY + 1) * BLOCK_SIZE > IMAGE_HEIGHT ? IMAGE_HEIGHT - 1 : (elem.blockY + 1) * BLOCK_SIZE;

		for (auto r = elem.blockY * BLOCK_SIZE; r < maxdiffBlockRightBottomY; ++r)
		{
			for (auto c = elem.blockX * BLOCK_SIZE; c < maxdiffBlockRightBottomX; ++c)
			{
				if (currentGrayFrame.at<uchar>(r, c) > averageValue)
				{
					currentGrayFrame.at<uchar>(r, c) = currentGrayFrame.at<uchar>(r, c) + 10 > 255 ? 255 : currentGrayFrame.at<uchar>(r, c) + 10;
				}
			}
		}
	}
}


void GetTheFirstImage(cv::Mat& currentFrame)
{
	auto startIndex = 0;
	sprintf_s(imageFullName, BUFF_SIZE, imageListFormat, startIndex);
	currentFrame = cv::imread(imageFullName);
}

int main(int argc, char* argv[])
{
	std::cout << "This is Strengthen intensity" << std::endl;

	cv::Mat currentFrame;
	cv::Mat grayImg;

	GetTheFirstImage(currentFrame);

	if (currentFrame.empty() == true)
	{
		std::cout << "Open Image File Failed!" << std::endl;
		system("Pause");
		return -1;
	}
	UpdateImageSize(currentFrame);

	while (true)
	{
		sprintf_s(imageFullName, BUFF_SIZE, imageListFormat, imageIndex);
		currentFrame = cv::imread(imageFullName);

		if (currentFrame.empty() == true)
		{
			break;
		}

		GeneralUtil::ToGray(currentFrame, grayImg);

		imshow("Before Strengthen Intensity", grayImg);

		StrengthenIntensityOfBlock(grayImg);

		imshow("After Strengthen Intensity", grayImg);

		std::cout << "Current Index = " << std::setw(8) << ++imageIndex << std::endl;

		cv::waitKey(100);
	}

	cv::destroyAllWindows();

	system("Pause");
	return 0;
}
