#include "lineNumber.hpp"

extern int debug;

int LineNumber::StartNumber = START_PARA_NUMBER;
int LineNumber::Limit = START_PARA_NUMBER * 2;

// to try special case like "bottom"
static const string endOfLineNumber = R"(")";
static const string endOfGeneratedLineNumber = R"(>)";

bool operator>(LineNumber const &ln1, LineNumber const &ln2) {
  return (ln1.getParaNumber() > ln2.getParaNumber() or
          (ln1.getParaNumber() == ln2.getParaNumber() and
           ln1.getlineNumber() > ln2.getlineNumber()));
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
      if (debug >= LOG_EXCEPTION)
        METHOD_OUTPUT << "too limit to hold such paragraph: " << m_paraNumber
                      << endl;
    }
  } else
    // temporarily accept non-P number
    m_paraNumber = TurnToInt(name.substr(0));
}

/**
 * generate the line prefix for numbering a line
 * @return the prefix to add
 */
string LineNumber::generateLinePrefix() {
  string result{emptyString};
  if (!isParagraphHeader())
    result = UnhiddenLineNumberStart + asString() + endOfLineNumber +
             endOfGeneratedLineNumber;
  return result;
}

string LineNumber::asString() {
  string result{emptyString};
  if (m_paraNumber != 0)
    result = leadingChar + TurnToString(m_paraNumber);
  if (m_lineNumber != 0)
    result += middleChar + TurnToString(m_lineNumber);
  return result;
}

bool LineNumber::isWithinLineRange(int minPara, int maxPara, int minLine,
                                   int maxLine) {
  bool biggerThanMin = true;
  bool lessThanMax = true;
  // only support para now
  if (minPara != 0)
    // inclusive
    biggerThanMin = m_paraNumber >= minPara;
  if (maxPara != 0)
    // inclusive
    lessThanMax = m_paraNumber <= maxPara;
  return (biggerThanMin and lessThanMax);
}

/**
 * retrieve lineNumber from the link at the beginning of containedLine
 * and read from it the paraNumber and lineNumber
 * @param containedLine the numbered line of one body text file
 */
size_t LineNumberPlaceholderLink::loadFirstFromContainedLine(
    const string &containedLine, size_t after) {
  string subStrAfterId;
  auto beginPos = string::npos;
  string begin;
  for (const auto &start : {UnhiddenLineNumberStart, HiddenLineNumberStart,
                            DirectLineNumberStart}) {
    begin = start;
    beginPos = containedLine.find(begin, after);
    if (beginPos != string::npos)
      break;
  }
  if (beginPos == string::npos)
    return string::npos;
  // found name in subStrAfterId
  m_fullString =
      getWholeStringBetweenTags(containedLine, begin, linkEndChars, after);
  subStrAfterId = containedLine.substr(beginPos + begin.length());
  auto substr = subStrAfterId.substr(0, subStrAfterId.find(endOfLineNumber));
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << substr << endl;
  if (substr == bottomParagraphIndicator)
    m_paraLineNumber.readFromString(leadingChar +
                                    TurnToString(LineNumber::Limit - 1));
  else
    m_paraLineNumber.readFromString(substr);
  return beginPos;
}

static const string inBetweenParaAndLineNumber = R"(.)";

string LineNumberPlaceholderLink::getDisplayString() {
  if (m_paraLineNumber.getlineNumber() == 0)
    return emptyString;
  return TurnToString(m_paraLineNumber.getParaNumber()) +
         inBetweenParaAndLineNumber +
         TurnToString(m_paraLineNumber.getlineNumber());
}

static const string inBetweenTwoParas = R"(" href=")";
static const string lastPara = R"(<a unhidden id="bottom" href="#top">)";

string LineNumberPlaceholderLink::getWholeString() {
  if (m_paraLineNumber.isPureTextOnly())
    return emptyString;
  if (m_paraLineNumber.isParagraphHeader()) {
    if (m_paraLineNumber.getParaNumber() != LineNumber::Limit - 1) {
      auto nextPara = m_paraLineNumber;
      nextPara.increaseParaNumberByOne();
      return UnhiddenLineNumberStart + m_paraLineNumber.asString() +
             inBetweenTwoParas + nextPara.asString() + endOfLineNumber +
             endOfGeneratedLineNumber;
    } else {
      return lastPara;
    }
  }
  return m_paraLineNumber.generateLinePrefix() +
         TurnToString(m_paraLineNumber.getParaNumber()) +
         inBetweenParaAndLineNumber +
         TurnToString(m_paraLineNumber.getlineNumber()) + LineNumberEnd;
}

size_t LineNumberPlaceholderLink::displaySize() {
  return getDisplayString().length();
}
