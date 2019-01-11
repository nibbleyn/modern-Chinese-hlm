#pragma once
#include "utf8StringUtil.hpp"

static const int START_PARA_NUMBER = 90;
class LineNumber {
  static const string UnhiddenLineNumberStart;
  static const string HiddenLineNumberStart;
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
      : paraNumber(paraNumber), lineNumber(lineNumber) {}
  LineNumber(const string &name) { readFromString(name); }

  LineNumber(const LineNumber &) = default;
  LineNumber &operator=(const LineNumber &) = default;

  void loadFromContainedLine(const string &containedLine);
  bool valid() { return (paraNumber != 0 and lineNumber != 0); }
  bool isParagraphHeader() {
    return (paraNumber != 0 and paraNumber < Limit and lineNumber == 0);
  }
  bool isPureTextOnly() { return (paraNumber == 0); }
  string generateLinePrefix();
  string asString() {
    string result{""};
    if (paraNumber != 0)
      result = leadingChar + TurnToString(paraNumber);
    if (lineNumber != 0)
      result += middleChar + TurnToString(lineNumber);
    return result;
  }
  bool isWithinLineRange(int minPara = 0, int maxPara = 0, int minLine = 0,
                         int maxLine = 0) {
    bool biggerThanMin = true;
    bool lessThanMax = true;
    // only support para now
    if (minPara != 0)
      biggerThanMin = paraNumber >= minPara; // inclusive
    if (maxPara != 0)
      lessThanMax = paraNumber <= maxPara; // inclusive
    return (biggerThanMin and lessThanMax);
  }
  bool equal(LineNumber &ln) {
    return (lineNumber == ln.getlineNumber() and
            paraNumber == ln.getParaNumber());
  }
  bool equal(const string &lnStr) { return (lnStr == asString()); }
  int getParaNumber() { return paraNumber; }
  int getlineNumber() { return lineNumber; }

private:
  void readFromString(const string &name);
  int paraNumber{0};
  int lineNumber{0};
};

string replacePart(string &linkString, const string &key,
                   const string &toReplace);