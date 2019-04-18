#pragma once
//#include "Object.hpp"

#include "Object.hpp"
#include "paraHeader.hpp"

static const int START_PARA_NUMBER = 90;
// line number is placeholder hyperlink since a ParagraphHeader would have href
// attribute and is a line number also
static const string UnhiddenLineNumberStart = R"(<a unhidden id=")";
static const string HiddenLineNumberStart = R"(<a hidden id=")";
static const string LineNumberEnd = R"(</a>)";
class LineNumber : public Object {
  static int StartNumber;
  static int Limit;

public:
  /**
   * the paragraph number in a body text would start from this number
   * and increase thru downlink to limit-1
   * to hold total paragraphs of this number
   * for example, if this start number is 90
   * the maximal paragraphs to hold would be 90
   * till the last paragraph as 179
   * during which the uplink name is decreased from 89 to 1.
   * so for numbering and fixing, just use START_PARA_NUMBER should be good
   * enough but for reconstruct story, a bigger number could be used to hold
   * more paragraphs.
   * @param num the paragraph number of first paragraph header
   */
  static void setStartNumber(int num) {
    StartNumber = num;
    Limit = num * 2;
  }
  static int getStartNumber() { return StartNumber; }

  LineNumber() = default;
  LineNumber(int paraNumber, int lineNumber)
      : m_paraNumber(paraNumber), m_lineNumber(lineNumber) {}
  LineNumber(const string &name) { readFromString(name); }

  LineNumber(const LineNumber &) = default;
  LineNumber &operator=(const LineNumber &) = default;
  string getWholeString();
  string getDisplayString();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  bool valid() { return (m_paraNumber != 0 and m_lineNumber != 0); }
  bool isParagraphHeader() {
    return (m_paraNumber != 0 and m_paraNumber < Limit and m_lineNumber == 0);
  }
  bool isPureTextOnly() { return (m_paraNumber == 0); }
  string generateLinePrefix();
  string asString() {
    string result{""};
    if (m_paraNumber != 0)
      result = leadingChar + TurnToString(m_paraNumber);
    if (m_lineNumber != 0)
      result += middleChar + TurnToString(m_lineNumber);
    return result;
  }
  bool isWithinLineRange(int minPara = 0, int maxPara = 0, int minLine = 0,
                         int maxLine = 0);
  bool equal(const LineNumber &ln) {
    return (m_lineNumber == ln.getlineNumber() and
            m_paraNumber == ln.getParaNumber());
  }
  bool equal(const string &lnStr) { return (lnStr == asString()); }
  int getParaNumber() const { return m_paraNumber; }
  int getlineNumber() const { return m_lineNumber; }
  friend bool operator>(LineNumber const &, LineNumber const &);

private:
  void readFromString(const string &name);
  int m_paraNumber{0};
  int m_lineNumber{0};
};
