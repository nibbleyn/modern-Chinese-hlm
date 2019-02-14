#include "utf8StringUtil.hpp"

Poem::Poem(const string &poemString) {
  loadFirstFromContainedLine(poemString);
  cout << m_poemText << endl;
}

size_t Poem::loadFirstFromContainedLine(const string &containedLine) {
  string begin = poemBeginChars;
  string end = poemEndChars;
  auto endPos = containedLine.find(end);
  if (containedLine.find(begin) == string::npos or endPos == string::npos)
    return string::npos;
  auto beginPos = containedLine.find_last_of(endOfBeginTag, endPos);
  m_poemText = containedLine.substr(beginPos + 1, endPos - beginPos - 1);
  return beginPos + 1;
}

size_t Poem::length() {
  return poemBeginChars.length() + unhiddenDisplayPropterty.length() +
         endOfBeginTag.length() + m_poemText.length() + poemEndChars.length();
}
size_t Poem::displaySize() { return m_poemText.length(); }

void testPoem() {
  string poemStr =
      R"(<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>)";
  string line =
      R"(<a unhidden name="P11L1">11.1</a>　　<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  Poem poem1(poemStr);
  cout << "length: " << poem1.length()
       << " display size: " << poem1.displaySize() << endl;
  Poem poem2;
  cout << "first appearance offset: " << poem2.loadFirstFromContainedLine(line)
       << " length: " << poem2.length()
       << " display size: " << poem2.displaySize() << endl;
}

/**
 * all file names like a077.htm, b003_8.htm
 * have a 2-digit string part "77", "03"
 * specifying chapter 77,3 etc.
 * this function return a 2-digit string from chapter number
 * for example, "07" from chapter 7
 * @param chapterNumber the index of the chapter
 * @return 2-digit string of chapter number
 */
string formatIntoTwoDigitChapterNumber(int chapterNumber) {
  stringstream formatedNumber;
  formatedNumber.fill('0');
  formatedNumber.width(2);
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
  for (int i = min; i <= std::min(99, max); i++) {
    fs.insert(formatIntoTwoDigitChapterNumber(i));
  }
  for (int i = 100; i <= max; i++) {
    fs.insert(TurnToString(i));
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
