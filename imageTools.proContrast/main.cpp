#include <iostream>
#include <core/core_c.h>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

#include "googleFlag.h"

void ImageStretchByHistogram(const cv::Mat& srcImg, cv::Mat& dstImg)
{
	assert(srcImg.cols == dstImg.cols);
	assert(srcImg.rows == dstImg.rows);

	double probabilityHist[256], cumulateDistributeHist[256], pixcelValuCounts[256];

	memset(probabilityHist, 0, sizeof(probabilityHist));
	memset(cumulateDistributeHist, 0, sizeof(cumulateDistributeHist));
	memset(pixcelValuCounts, 0, sizeof(pixcelValuCounts));

	auto height = srcImg.rows;
	auto width = srcImg.cols;
	long allPixcelCount = height * width;

	for (auto c = 0; c < width; c++)
	{
		for (auto r = 0; r < height; r++)
		{
			auto val = srcImg.at<uchar>(r, c);
			pixcelValuCounts[val]++;
		}
	}

	for (auto i = 0; i < 256; i++)
	{
		probabilityHist[i] = pixcelValuCounts[i] / allPixcelCount;
	}

	for (auto i = 0; i < 256; i++)
	{
		for (auto k = 0; k <= i; k++)
		{
			cumulateDistributeHist[i] += probabilityHist[k];
		}
	}

	for (auto x = 0; x < width; x++)
	{
		for (auto y = 0; y < height; y++)
		{
			auto v = srcImg.at<uchar>(y, x);
			dstImg.at<uchar>(y, x) = cumulateDistributeHist[v] * 255 + 0.5;
		}
	}
}

int main(int argc, char* argv[])
{
	std::string usageStr = "";
	usageStr += "Pro contrast image (gray image only now).\n";
	usageStr += "Usage:\n";
	usageStr += "    imageTools.proContrast --inputImage=source image file full name --outputImage=output image fulle name --format=save file format.\n";

	google::SetUsageMessage(usageStr);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_inputImage.empty())
	{
		std::cout << "Source image file name cannot be empty!" << std::endl;
		system("Pause");
		return -1;
	}

	auto inputImageFilename = FLAGS_inputImage;
	auto sourceImage = cv::imread(inputImageFilename);

	if (!sourceImage.empty())
	{
		auto outputImageFileName = FLAGS_outputImage;
		outputImageFileName += ".";
		outputImageFileName += FLAGS_format;

		cv::Mat grayImg;
		if (sourceImage.channels() == 3)
		{
			cvtColor(sourceImage, grayImg, CV_BGR2GRAY);
		}
		else
		{
			grayImg = sourceImage;
		}

		cv::Mat resultImg(grayImg.rows, grayImg.cols, CV_8UC1);
		ImageStretchByHistogram(grayImg, resultImg);

		imwrite(outputImageFileName, resultImg);
	}

	google::ShutDownCommandLineFlags();

	std::cout << "Done!" << std::endl;
	system("Pause");

	return 0;
}
