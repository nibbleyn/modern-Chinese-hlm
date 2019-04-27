#pragma once

#include "coupledBodyTextWithLink.hpp"
#include "coupledContainer.hpp"
#include "genericContainer.hpp"
#include <cmath>
#include <sstream>

static const int THREE_DIGIT_FILENAME = 3;
static const int TWO_DIGIT_FILENAME = 2;

// operations to construct a group of file names
fileSet buildFileSet(int min, int max, int digits = TWO_DIGIT_FILENAME);
