#pragma once
#include <gflags/gflags.h>

DEFINE_string(imageList1, "", "The first image list folder");
DEFINE_string(imageList2, "", "The second image list folder");
DEFINE_string(saveFileName, "", "The video file name to save");
DEFINE_int32(ftps, 10, "The video ftps to save");
DEFINE_string(nameTemplate1, "Frame_%08d", "The first image list name template");
DEFINE_string(nameTemplate2, "Frame_%08d", "The second image list name template");
DEFINE_string(format1, ".png", "The first images' format.");
DEFINE_string(format2, ".png", "The second images' format.");
DEFINE_string(compressFourCC, "", "The images' format.");
