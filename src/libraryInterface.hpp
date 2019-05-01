#pragma once

#include "coupledBodyTextWithLink.hpp"
#include "coupledContainer.hpp"
#include "genericContainer.hpp"

// operations to construct a group of file names
FileSet buildFileSet(int min, int max, int digits = TWO_DIGIT_FILENAME);
