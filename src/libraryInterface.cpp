#include "libraryInterface.hpp"

/**
 * generate a set of strings
 * containing a digits length of string of each chapter
 * starting from min
 * and ending in max
 * @param min the smallest chapter
 * @param max the largest chapter
 * @kind to determine digits the length of the chapter string
 * @return a set of strings in fileSet
 */
FileSet buildFileSet(int minValue, int maxValue, const string &kind) {
  FileSet fs;
  for (int i = minValue; i <= maxValue; i++) {
    fs.insert(formatIntoZeroPatchedChapterNumber(
        i, (kind == JPM) ? THREE_DIGIT_FILENAME : TWO_DIGIT_FILENAME));
  }
  return fs;
}
