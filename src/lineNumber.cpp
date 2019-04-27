#include "lineNumber.hpp"

extern int debug;

int LineNumber::StartNumber = START_PARA_NUMBER;
int LineNumber::Limit = START_PARA_NUMBER * 2;

static const string endOfLineNumber =
    R"(")"; // to try special case like "bottom"
static const string endOfGeneratedLineNumber = R"(>)";
static const string inBetweenTwoParas = R"(" href=")";
static const string inBetweenParaAndLineNumber = R"(.)";
static const string lastPara = R"(<a unhidden id="bottom" href="#top">)";

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
    m_paraNumber = TurnToInt(name.substr(0)); // temporarily accept non-P number
}

/**
 * retrieve lineNumber from the link at the beginning of containedLine
 * and read from it the paraNumber and lineNumber
 * @param containedLine the numbered line of one body text file
 */
size_t LineNumber::loadFirstFromContainedLine(const string &containedLine,
                                              size_t after) {
  string begin = UnhiddenLineNumberStart;
  auto beginPos = containedLine.find(begin, after);
  if (beginPos == string::npos) {
    begin = HiddenLineNumberStart;
    beginPos = containedLine.find(begin, after);
  }
  if (beginPos != string::npos) // found name in lineName
  {
    string end = linkEndChars;
    auto endPos = containedLine.find(end, beginPos);

    m_fullString =
        containedLine.substr(beginPos, endPos + end.length() - beginPos);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "m_fullString: " << endl;
      METHOD_OUTPUT << m_fullString << endl;
    }

    string lineName = containedLine.substr(beginPos + begin.length());
    end = endOfLineNumber;
    endPos = lineName.find(end);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << lineName.substr(0, endPos) << endl;
    if (lineName.substr(0, endPos) == bottomParagraphIndicator)
      readFromString(leadingChar + TurnToString(Limit - 1));
    else
      readFromString(lineName.substr(0, endPos));
  }
  return beginPos;
}

string LineNumber::getDisplayString() {
  if (m_lineNumber == 0)
    return "";
  return TurnToString(m_paraNumber) + inBetweenParaAndLineNumber +
         TurnToString(m_lineNumber);
}

string LineNumber::getWholeString() {
  if (isPureTextOnly())
    return "";
  if (isParagraphHeader()) {
    if (m_paraNumber != Limit - 1) {
      auto nextPara = *this;
      nextPara.m_paraNumber++;
      return UnhiddenLineNumberStart + asString() + inBetweenTwoParas +
             nextPara.asString() + endOfLineNumber + endOfGeneratedLineNumber;
    } else {
      return lastPara;
    }
  }
  return generateLinePrefix() + TurnToString(m_paraNumber) +
         inBetweenParaAndLineNumber + TurnToString(m_lineNumber) +
         LineNumberEnd;
}
/**
 * generate the line prefix for numbering a line
 * @return the prefix to add
 */
string LineNumber::generateLinePrefix() {
  string result{""};
  if (!isParagraphHeader())
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

size_t LineNumber::displaySize() { return getDisplayString().length(); }
