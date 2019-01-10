#pragma once
#include "fileUtil.hpp"

int utf8length(std::string originalString);
std::string utf8substr(std::string originalString, size_t begin, size_t &end,
                       size_t SubStrLength);
