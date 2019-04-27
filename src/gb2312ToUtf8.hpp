#pragma once
#include "libraryInterface.hpp"

using namespace std;

static const string GB2312_HTML_SRC = R"(gb2312HTML/)";

// fix gb2312 files to utf8 format files
void ConvertNonPrefixedGb2312FilesToUtf8();
