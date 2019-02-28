#include "utf8StringUtil.hpp"

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
string formatIntoZeroPatchedChapterNumber(int chapterNumber, int digits) {
  stringstream formatedNumber;
  formatedNumber.fill('0');
  formatedNumber.width(digits);
  formatedNumber.clear();
  formatedNumber << internal << chapterNumber;
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
    fs.insert(
        formatIntoZeroPatchedChapterNumber(i, max(digits, numberOfDigits)));
  }
  return fs;
}

/**
 * replace specific parts of a template link to actual value
 * @param linkString the link has key to replace
 * @param key the identified part to replace in the link
 * @param toReplace the actual value to replace in the link
 * @return the link after replacing all parts
 */
string replacePart(string &linkString, const string &key,
                   const string &toReplace) {
  while (true) {
    auto partBegin = linkString.find(key);
    if (partBegin == string::npos)
      break;
    linkString.replace(partBegin, key.length(), toReplace);
  }
  return linkString;
}

int utf8length(std::string originalString) {
  size_t len = 0, byteIndex = 0;
  const char *aStr = originalString.c_str();
  for (; byteIndex < originalString.size(); byteIndex++) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
  }
  return len;
}

std::string utf8substr(std::string originalString, size_t begin, size_t &end,
                       size_t SubStrLength) {
  const char *aStr = originalString.c_str();
  size_t origSize = originalString.size();
  size_t byteIndex = begin;
  size_t len = 0;

  end = begin;
  for (; byteIndex < origSize; byteIndex++) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
    if (len >= SubStrLength) {
      end = byteIndex - 1;
      break;
    }
  }
  return originalString.substr(begin, byteIndex - begin);
}
