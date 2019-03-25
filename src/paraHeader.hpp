#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

class ParaHeader {
  static const string firstParaHeader;
  static const string MiddleParaHeader;
  static const string lastParaHeader;

public:
  void loadFromFirstParaHeader(const string &header);
  void loadFromMiddleParaHeader(const string &header);
  void loadFromLastParaHeader(const string &header);
  string getDisplayString();

  void fixFirstParaHeaderFromTemplate();
  void fixMiddleParaHeaderFromTemplate();
  void fixLastParaHeaderFromTemplate();

  string getFixedResult() { return m_result; }

  int m_startNumber{0};
  string m_color{MAIN_SEPERATOR_COLOR};
  int m_currentParaNo{0};
  bool m_hidden{false};
  bool m_lastPara{false};

private:
  string m_result{""};
  string m_displayText{""};
};

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);

static const string defaultUnit = R"(回)";
static const string attachmentUnit = R"(篇)";
static const string searchUnit = R"(条)";
static const string numberingUnit = R"(段)";

string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara,
                                      const string &units = defaultUnit,
                                      bool hidden = false);

string insertParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                    int startParaNo, int endParaNo,
                                    int totalPara, int preTotalPara,
                                    const string &color,
                                    const string &units = defaultUnit,
                                    bool hidden = false, bool lastPara = false);

string fixBackParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                     int totalPara, const string &color,
                                     const string &units = defaultUnit,
                                     bool hidden = false);
