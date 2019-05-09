#include "utf8StringUtil.hpp"
#include <sstream>

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
 * replace specific parts of a template link to actual value
 * @param linkString the link has key to replace
 * @param key the identified part to replace in the link
 * @param toReplace the actual value to replace in the link
 * @return the link after replacing all parts
 */
void replacePart(string &linkString, const string &key,
                 const string &toReplace) {
  if (key == toReplace)
    return;
  while (true) {
    auto partBegin = linkString.find(key);
    if (partBegin == string::npos)
      break;
    linkString.replace(partBegin, key.length(), toReplace);
  }
}

int utf8length(const string &originalString) {
  size_t len = 0, byteIndex = 0;
  const char *aStr = originalString.c_str();
  for (; byteIndex < originalString.size(); byteIndex++) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
  }
  return len;
}

string utf8substr(const string &originalString, size_t begin, size_t &end,
                  size_t SubStrLength) {
  const char *aStr = originalString.c_str();
  size_t origSize = originalString.size();
  size_t byteIndex = begin;
  size_t len = 0;

  end = begin;
  if (((aStr[byteIndex] & 0xc0) == 0x80))
    cout << "not aligned" << endl;
  while (byteIndex < origSize) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
    if ((len > SubStrLength))
      break;
    byteIndex++;
  }
  end = byteIndex - 1;
  return originalString.substr(begin, end - begin + 1);
}

string markDifference(const string &firstString, const string &secondString,
                      size_t begin) {
  if (firstString == secondString)
    return R"(same)";
  const char *aStr1 = firstString.c_str();
  const char *aStr2 = secondString.c_str();
  size_t firstPos = 0;
  for (size_t byteIndex = begin;
       byteIndex < min(firstString.size(), secondString.size()); byteIndex++) {
    if (aStr1[byteIndex] == aStr2[byteIndex])
      firstPos++;
    else
      break;
  }
  return firstString.substr(0, firstPos) + "^";
}

void printCompareResult(const string &firstString, const string &secondString,
                        size_t begin) {
  FUNCTION_OUTPUT << firstString << "||" << endl;
  FUNCTION_OUTPUT << secondString << "||" << endl;
  FUNCTION_OUTPUT << markDifference(firstString, secondString, begin) << endl;
}

string getIncludedStringBetweenTags(const string &originalString,
                                    const string &begin, const string &end,
                                    size_t after) {
  auto beginPos = (begin.empty()) ? after : originalString.find(begin, after);
  auto endPos = originalString.length();
  if (not end.empty())
    endPos = originalString.find(end, beginPos + begin.length());
  if (beginPos == string::npos or endPos == string::npos)
    return emptyString;
  return originalString.substr(beginPos + begin.length(),
                               endPos - begin.length() - beginPos);
}

string getWholeStringBetweenTags(const string &originalString,
                                 const string &begin, const string &end,
                                 size_t after) {
  auto beginPos = (begin.empty()) ? after : originalString.find(begin, after);
  auto endPos = originalString.length();
  if (not end.empty())
    endPos = originalString.find(end, beginPos + begin.length());
  if (beginPos == string::npos or endPos == string::npos)
    return emptyString;
  return originalString.substr(beginPos, endPos + end.length() - beginPos);
}

string getFileNameFromAttachmentNumber(AttachmentNumber num) {
  string result = TurnToString(num.first);
  if (num.second != 0)
    result += attachmentFileMiddleChar + TurnToString(num.second);
  return result;
}

/**
 * get chapter number and attachment number from an attachment file name
 * for example with input b001_15 would return pair <1,15>
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return pair of chapter number and attachment number
 */
AttachmentNumber getAttachmentNumber(const string &filename,
                                     bool prefixIncluded) {
  AttachmentNumber num(0, 0);
  // referred file not found
  if (prefixIncluded and
      filename.find(ATTACHMENT_TYPE_HTML_TARGET) == string::npos) {
    return num;
  }

  string start = prefixIncluded ? ATTACHMENT_TYPE_HTML_TARGET : emptyString;
  string middle = (filename.find(attachmentFileMiddleChar) == string::npos)
                      ? emptyString
                      : attachmentFileMiddleChar;
  num.first = TurnToInt(getIncludedStringBetweenTags(filename, start, middle));
  if (not middle.empty())
    num.second = TurnToInt(getIncludedStringBetweenTags(
        filename, attachmentFileMiddleChar, emptyString));
  return num;
}

string getChapterNameByTargetKind(const string &targetKind, int chapterNumber) {
  if (targetKind == JPM_TYPE_HTML_TARGET)
    return formatIntoZeroPatchedChapterNumber(chapterNumber,
                                              THREE_DIGIT_FILENAME);
  return formatIntoZeroPatchedChapterNumber(chapterNumber, TWO_DIGIT_FILENAME);
}
