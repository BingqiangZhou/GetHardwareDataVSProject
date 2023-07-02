#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "IMVApi.h"

#include <opencv2/opencv.hpp>

extern cv::Mat matConvertedImg;

// 图片转化
// Image convert
//void imageConvert(IMV_HANDLE devHandle, IMV_Frame frame, IMV_EPixelType convertFormat);

cv::Mat imageConvert(IMV_HANDLE devHandle, IMV_Frame frame, IMV_EPixelType convertFormat);
