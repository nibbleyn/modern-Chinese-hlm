#pragma once
#include "gb2312ToUtf8.hpp"
#include "story.hpp"
#include "linkFix.hpp"
#include "numbering.hpp"
#include "search.hpp"

void fixHeaderAndFooter(const string &filename = "084");

void tools(int num);
