#pragma once

#include "Object.hpp"

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
static const int START_PARA_NUMBER = 90;

class LineNumber {
  static int StartNumber;

public:
  static int Limit;
  static void setStartNumber(int num) {
    StartNumber = num;
    Limit = num * 2;
  }
  static int getStartNumber() { return StartNumber; }

  LineNumber() = default;
  LineNumber(int paraNumber, int lineNumber)
      : m_paraNumber(paraNumber), m_lineNumber(lineNumber) {}
  LineNumber(const string &name) { readFromString(name); }
  void readFromString(const string &name);

  int getParaNumber() const { return m_paraNumber; }
  int getlineNumber() const { return m_lineNumber; }
  void increaseParaNumberByOne() { m_paraNumber++; }

  LineNumber(const LineNumber &) = default;
  LineNumber &operator=(const LineNumber &) = default;
  bool equal(const LineNumber &ln) {
    return (m_lineNumber == ln.getlineNumber() and
            m_paraNumber == ln.getParaNumber());
  }
  bool equal(const string &lnStr) { return (lnStr == asString()); }
  friend bool operator>(LineNumber const &, LineNumber const &);

  bool valid() { return (m_paraNumber != 0 and m_lineNumber != 0); }
  bool isParagraphHeader() {
    return (m_paraNumber != 0 and m_paraNumber < Limit and m_lineNumber == 0);
  }
  bool isPureTextOnly() { return (m_paraNumber == 0); }
  string generateLinePrefix();
  string asString();
  bool isWithinLineRange(int minPara = 0, int maxPara = 0, int minLine = 0,
                         int maxLine = 0);
  ParaLineNumber getParaLineNumber() {
    return make_pair(m_paraNumber, m_lineNumber);
  }

private:
  int m_paraNumber{0};
  int m_lineNumber{0};
};

class LineNumberPlaceholderLink : public Object {
public:
  LineNumberPlaceholderLink() { m_objectType = OBJECT_TYPE::LINENUMBER; }
  LineNumberPlaceholderLink(const LineNumber &ln)
      : m_paraLineNumber(ln.getParaNumber(), ln.getlineNumber()) {
    m_objectType = OBJECT_TYPE::LINENUMBER;
    m_fullString = getWholeString();
  }
  LineNumberPlaceholderLink(int paraNumber, int lineNumber)
      : m_paraLineNumber(paraNumber, lineNumber) {
    m_objectType = OBJECT_TYPE::LINENUMBER;
    m_fullString = getWholeString();
  }
  LineNumberPlaceholderLink(const string &linkString) {
    m_objectType = OBJECT_TYPE::LINENUMBER;
    m_paraLineNumber.readFromString(linkString);
    m_fullString = getWholeString();
  }
  bool isPartOfParagraphHeader() {
    return m_paraLineNumber.isParagraphHeader();
  }
  bool isPureTextOnly() { return m_paraLineNumber.isPureTextOnly(); }
  string getParaLineString() { return m_paraLineNumber.asString(); }
  LineNumber get() { return m_paraLineNumber; }
  string getStartTag() override { return m_realStartTag; }
  string getEndTag() override { return linkEndChars; }
  string getName() override { return nameOfLineNumberType; }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  LineNumber m_paraLineNumber{0, 0};
  string m_realStartTag{UnhiddenLineNumberStart};
};
