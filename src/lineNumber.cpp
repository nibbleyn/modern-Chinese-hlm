#include "lineNumber.hpp"
#include <regex>

extern int debug;

static const string endOfLineNumber =
    R"(")"; // to try special case like "bottom"
static const string endOfGeneratedLineNumber = R"(>)";

const string LineNumber::UnhiddenLineNumberStart = R"(<a unhidden name=")";
const string LineNumber::HiddenLineNumberStart = R"(<a hidden name=")";
int LineNumber::StartNumber = START_PARA_NUMBER;
int LineNumber::Limit = START_PARA_NUMBER * 2;

size_t LineNumber::length() { return 0; }
size_t LineNumber::displaySize() { return 0; }
/**
 * retrieve lineNumber from the link at the beginning of containedLine
 * and read from it the paraNumber and lineNumber
 * @param containedLine the numbered line of one body text file
 */
size_t LineNumber::loadFirstFromContainedLine(const string &containedLine) {
  string start = UnhiddenLineNumberStart;
  auto linkBegin = containedLine.find(start);
  if (linkBegin == string::npos) {
    start = HiddenLineNumberStart;
    linkBegin = containedLine.find(start);
  }
  if (linkBegin != string::npos) // found name in lineName
  {
    string end = endOfLineNumber;
    string lineName = containedLine.substr(linkBegin + start.length());
    if (debug >= LOG_INFO)
      cout << lineName << endl;
    auto linkEnd = lineName.find(end);
    if (debug >= LOG_INFO)
      cout << lineName.substr(0, linkEnd) << endl;
    if (lineName.substr(0, linkEnd) == bottomParagraphIndicator)
      readFromString(leadingChar + TurnToString(Limit - 1));
    else
      readFromString(lineName.substr(0, linkEnd));
  }
  return linkBegin;
}
/**
 * generate the line prefix for numbering a line
 * @return the prefix to add
 */
string LineNumber::generateLinePrefix() {
  string result{""};
  if (m_lineNumber != 0)
    result = UnhiddenLineNumberStart + asString() + endOfLineNumber +
             endOfGeneratedLineNumber;
  return result;
}

bool LineNumber::isWithinLineRange(int minPara, int maxPara, int minLine,
                                   int maxLine) {
  bool biggerThanMin = true;
  bool lessThanMax = true;
  // only support para now
  if (minPara != 0)
    biggerThanMin = m_paraNumber >= minPara; // inclusive
  if (maxPara != 0)
    lessThanMax = m_paraNumber <= maxPara; // inclusive
  return (biggerThanMin and lessThanMax);
}

/**
 * read from a string of format "PxxLyy"
 * and assign xx to paraNumber
 * and assign yy to lineNumber
 * @param name a string of format "PxxLyy"
 */
void LineNumber::readFromString(const string &name) {
  auto lineNumberStart = name.find(middleChar);
  if (lineNumberStart != string::npos) {
    // sign of already numbered
    m_lineNumber = TurnToInt(name.substr(lineNumberStart + 1));
    if (name.substr(0, 1) == leadingChar)
      m_paraNumber = TurnToInt(name.substr(1, lineNumberStart - 1));
  } else if (name.substr(0, 1) == leadingChar) {
    m_paraNumber = TurnToInt(name.substr(1));
    if (m_paraNumber >= Limit) {
      cout << "too limit to hold such paragraph: " << m_paraNumber << endl;
    }
  } else
    m_paraNumber = TurnToInt(name.substr(0)); // temporarily accept non-P number
}
