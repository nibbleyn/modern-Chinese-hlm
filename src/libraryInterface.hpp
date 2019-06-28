#pragma once

#include "coupledBodyTextContainer.hpp"
#include "coupledBodyTextWithLink.hpp"
#include "formatedBodyTextContainer.hpp"
#include "linkSetContainer.hpp"

static const int MAIN_MIN_CHAPTER_NUMBER = 1;
static const int MAIN_MAX_CHAPTER_NUMBER = 80;
static const int JPM_MIN_CHAPTER_NUMBER = 1;
static const int JPM_MAX_CHAPTER_NUMBER = 100;
static const int MIN_ATTACHMENT_NUMBER = 1;
static const int MAX_ATTACHMENT_NUMBER = 50;

// operations to construct a group of file names
FileSet buildFileSet(int minValue, int maxValue, const string &kind = MAIN);
