#pragma once

#include "coupledBodyTextContainer.hpp"
#include "coupledBodyTextWithLink.hpp"
#include "linkSetContainer.hpp"

// operations to construct a group of file names
FileSet buildFileSet(int minValue, int maxValue, const string &kind = MAIN);
