#include <iostream>
#include <opencv2/core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

static auto countX = 0;
static auto countY = 0;

static auto IMAGE_HEIGHT = 0;
static auto IMAGE_WIDTH = 0;

const auto BLOCK_SIZE = 10;

class DifferenceElem
{
public:
	DifferenceElem() : blockX(-1), blockY(-1), diffVal(0)
	{
	}

	int blockX;
	int blockY;
	int diffVal;
};

uchar GetMinValueOfBlock(const cv::Mat& mat)
{
	uchar minVal = 255;
	for (auto r = 0; r < mat.rows; ++r)
	{
		for (auto c = 0; c < mat.cols; ++c)
		{
			if (minVal > mat.at<uchar>(r, c))
			{
				minVal = mat.at<uchar>(r, c);
			}
		}
	}
	return minVal;
}

uchar GetMaxValueOfBlock(const cv::Mat& mat)
{
	uchar maxVal = 0;
	for (auto r = 0; r < mat.rows; ++r)
	{
		for (auto c = 0; c < mat.cols; ++c)
		{
			if (maxVal < mat.at<uchar>(r, c))
				maxVal = mat.at<uchar>(r, c);
		}
	}
	return maxVal;
}

std::vector<std::vector<uchar>> GetMaxMinPixelValueDifferenceMap(cv::Mat& curFrame)
{
	std::vector<std::vector<uchar>> maxmindiff(countY, std::vector<uchar>(countX, 0));
	for (auto br = 0; br < countY; ++br)
	{
		auto height = br == (countY - 1) ? IMAGE_HEIGHT - (countY - 1) * BLOCK_SIZE : BLOCK_SIZE;
		for (auto bc = 0; bc < countX; ++bc)
		{
			auto width = bc == (countX - 1) ? IMAGE_WIDTH - (countX - 1) * BLOCK_SIZE : BLOCK_SIZE;
			maxmindiff[br][bc] = GetMaxValueOfBlock(curFrame(cv::Rect(bc * BLOCK_SIZE, br * BLOCK_SIZE, width, height))) - GetMinValueOfBlock(curFrame(cv::Rect(bc * BLOCK_SIZE, br * BLOCK_SIZE, width, height)));
		}
	}
	return maxmindiff;
}

void GetMaxValueOfMatrix(std::vector<std::vector<uchar>> maxmindiff, DifferenceElem& diffElem)
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

std::vector<DifferenceElem> GetMostMaxDiffBlock(std::vector<std::vector<uchar>> maxmindiff)
{
	std::vector<DifferenceElem> mostPossibleBlocks;

	DifferenceElem diffElem;
	GetMaxValueOfMatrix(maxmindiff, diffElem);

	std::cout << "Block X = " << diffElem.blockX << " Block Y = " << diffElem.blockY << " Difference Value = " << diffElem.diffVal << std::endl;
	mostPossibleBlocks.push_back(diffElem);

	return mostPossibleBlocks;
}

uchar CalculateAverageValue(const cv::Mat& frame, int leftTopX, int leftTopY, int rightBottomX, int rightBottomY)
{
	auto sumAll = 0;
	for (auto r = leftTopY; r < rightBottomY; ++r)
	{
		auto sumRow = 0;
		for (auto c = leftTopX; c < rightBottomX; ++c)
		{
			sumRow += frame.at<uchar>(r, c);
		}
		sumAll += sumRow / (rightBottomX - leftTopX);
	}

	return static_cast<uchar>(sumAll / (rightBottomY - leftTopY));
}

void StrengthenIntensityOfBlock(cv::Mat& currentGrayFrame)
{
	auto maxmindiffMatrix = GetMaxMinPixelValueDifferenceMap(currentGrayFrame);

	auto differenceElems = GetMostMaxDiffBlock(maxmindiffMatrix);

	for (auto elem : differenceElems)
	{
		auto centerX = elem.blockX * BLOCK_SIZE + BLOCK_SIZE / 2;
		auto centerY = elem.blockY * BLOCK_SIZE + BLOCK_SIZE / 2;
		auto boundingBoxLeftTopX = centerX - BLOCK_SIZE >= 0 ? centerX - BLOCK_SIZE : 0;
		auto boundingBoxLeftTopY = centerY - BLOCK_SIZE >= 0 ? centerY - BLOCK_SIZE : 0;
		auto boundingBoxRightBottomX = centerX + BLOCK_SIZE < IMAGE_WIDTH ? centerX + BLOCK_SIZE : IMAGE_WIDTH - 1;
		auto boundingBoxRightBottomY = centerY + BLOCK_SIZE < IMAGE_HEIGHT ? centerY + BLOCK_SIZE : IMAGE_HEIGHT - 1;
		auto averageValue = CalculateAverageValue(currentGrayFrame, boundingBoxLeftTopX, boundingBoxLeftTopY, boundingBoxRightBottomX, boundingBoxRightBottomY);

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

void toGray(const cv::Mat& img, cv::Mat& grayImg)
{
	if (img.channels() == 3)
	{
		cvtColor(img, grayImg, CV_BGR2GRAY);
	}
	else
	{
		grayImg = img;
	}
}

void UpdateImageSize(cv::Mat& img)
{
	std::cout << "Update Image Size" << std::endl;

	IMAGE_WIDTH = img.cols;
	IMAGE_HEIGHT = img.rows;
	countX = ceil(static_cast<double>(IMAGE_WIDTH) / BLOCK_SIZE);
	countY = ceil(static_cast<double>(IMAGE_HEIGHT) / BLOCK_SIZE);
}

int main(int argc, char* argv[])
{
	std::cout << "This is Strengthen intensity" << std::endl;

	auto img = cv::imread("E:\\WorkLogs\\Data\\Ir\\207\\Raw\\1_0-600m_150ms\\Frame_00000000.bmp");

	if (img.empty() == true)
	{
		std::cout << "Open Image File Failed!" << std::endl;
		system("Pause");
		return -1;
	}

	UpdateImageSize(img);

	cv::Mat grayImg;
	toGray(img, grayImg);

	imshow("Before Strengthen Intensity", grayImg);

	StrengthenIntensityOfBlock(grayImg);

	imshow("After Strengthen Intensity", grayImg);

	cv::waitKey(0);

	cv::destroyAllWindows();

	system("Pause");
	return 0;
}
