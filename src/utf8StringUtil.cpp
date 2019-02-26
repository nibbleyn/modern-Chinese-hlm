#include "utf8StringUtil.hpp"

/**
 * all file names like a077.htm, b003_8.htm
 * have a 2-digit string part "77", "03"
 * specifying chapter 77,3 etc.
 * this function return a 2-digit string from chapter number
 * for example, "07" from chapter 7
 * @param chapterNumber the index of the chapter
 * @return 2-digit string of chapter number
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
 * containing 2-digit string of chapters
 * starting from min
 * and ending in max
 * @param min the smallest chapter
 * @param max the largest chapter
 * @return a set of strings in fileSet
 */
fileSet buildFileSet(int min, int max) {
  fileSet fs;
  // build 2 digit file name set
  if (max < 100) {
    for (int i = min; i <= max; i++) {
      fs.insert(formatIntoZeroPatchedChapterNumber(i, 2));
    }
  }
  // build 3 digit file name set
  else if (max < 1000) {
    for (int i = min; i <= max; i++) {
      fs.insert(formatIntoZeroPatchedChapterNumber(i, 3));
    }
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
