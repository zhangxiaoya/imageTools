#pragma once
#include <core/core.hpp>

class GeneralUtil
{
public:
	static uchar GetMinValueOfBlock(const cv::Mat& mat);

	static uchar GetMaxValueOfBlock(const cv::Mat& mat);

	static uchar CalculateAverageValue(const cv::Mat& frame, int leftTopX, int leftTopY, int rightBottomX, int rightBottomY);

	static void ToGray(const cv::Mat& img, cv::Mat& grayImg);
};

inline uchar GeneralUtil::GetMinValueOfBlock(const cv::Mat& mat)
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

inline uchar GeneralUtil::GetMaxValueOfBlock(const cv::Mat& mat)
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

inline uchar GeneralUtil::CalculateAverageValue(const cv::Mat& frame, int leftTopX, int leftTopY, int rightBottomX, int rightBottomY)
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

inline void GeneralUtil::ToGray(const cv::Mat& img, cv::Mat& grayImg)
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
