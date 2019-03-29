#pragma once
#include "attachmentFiles.hpp"

using namespace std;

static const string GB2312_HTML_SRC = "gb2312HTML/";

// fix gb2312 files to utf8 format files
void convertFromGB2312ToUtf8(string referFile, string format, FILE_TYPE type,
                             int attachNo = 1);
void convertFromGB2312ToUtf8(string inputFile, string outputFile);
void gb2312FixMain(int minTarget, int maxTarget);
void gb2312FixAttachment(int minTarget, int maxTarget);
void gb2312FixOriginal(int minTarget, int maxTarget);
void gb2312FixJPM();
void convertAttachmentMainMenuFromGB2312ToUtf8();
void convertMainMenuFromGB2312ToUtf8();
void convertAllFromGb2312ToUtf8();
void ConvertGb2312FilesToUtf8();
void ConvertPrefixedGb2312FilesToUtf8();
void ConvertNonPrefixedGb2312FilesToUtf8();
