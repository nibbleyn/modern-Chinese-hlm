#include "libraryInterface.hpp"
#include <cmath>

/**
 * generate a set of strings
 * containing a digits length of string of each chapter
 * starting from min
 * and ending in max
 * @param min the smallest chapter
 * @param max the largest chapter
 * @param digits the length of the chapter string
 * @return a set of strings in fileSet
 */
FileSet buildFileSet(int minValue, int maxValue, int digits) {
  FileSet fs;
  auto numberOfDigits = static_cast<int>(log10(maxValue)) + 1;
  for (int i = minValue; i <= maxValue; i++) {
    fs.insert(formatIntoZeroPatchedChapterNumber(
        i, std::max(digits, numberOfDigits)));
  }
  return fs;
}
