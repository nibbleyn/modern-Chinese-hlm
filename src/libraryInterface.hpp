#pragma once

#include "coupledBodyTextContainer.hpp"
#include "coupledBodyTextWithLink.hpp"
#include "linkSetContainer.hpp"

// operations to construct a group of file names
FileSet buildFileSet(int min, int max, int digits = TWO_DIGIT_FILENAME);
