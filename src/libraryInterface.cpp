#include "libraryInterface.hpp"

/**
 * all file names like a077.htm, b003_8.htm, d084.htm, d100.htm
 * have a 2-digit or 3-digit string part "77", "03", "084", "100"
 * specifying chapter 77,3,84,100 etc.
 * this function return a digits length of string from chapter number
 * for example, "07" from chapter 7, digits=2
 * @param chapterNumber the index of the chapter
 * @param digits the length of the chapter string
 * @return a digits length of string of chapter number
 */
std::string formatIntoZeroPatchedChapterNumber(int chapterNumber, int digits) {
  std::stringstream formatedNumber;
  formatedNumber.fill('0');
  formatedNumber.width(digits);
  formatedNumber.clear();
  formatedNumber << std::internal << chapterNumber;
  return formatedNumber.str();
}

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
fileSet buildFileSet(int minValue, int maxValue, int digits) {
  fileSet fs;
  auto numberOfDigits = static_cast<int>(log10(maxValue)) + 1;
  for (int i = minValue; i <= maxValue; i++) {
    fs.insert(formatIntoZeroPatchedChapterNumber(
        i, std::max(digits, numberOfDigits)));
  }
  return fs;
}
