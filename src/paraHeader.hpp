#pragma once
//#include "fileUtil.hpp"
//#include "utf8StringUtil.hpp"

#include "attachmentFiles.hpp"

enum class PARA_TYPE { FIRST, MIDDLE, LAST };

class ParaHeader {
  static const string firstParaHeader;
  static const string MiddleParaHeader;
  static const string lastParaHeader;

  static const string firstParaHeaderDispText;
  static const string MiddleParaHeaderDispText;
  static const string lastParaHeaderDispText;

public:
  void loadFrom(const string &header);

  // if not load from string
  void markAsFirstParaHeader() { m_type = PARA_TYPE::FIRST; }
  void markAsMiddleParaHeader() { m_type = PARA_TYPE::MIDDLE; }
  void markAsLastParaHeader() { m_type = PARA_TYPE::LAST; }

  bool isFirstParaHeader() { return m_type == PARA_TYPE::FIRST; }
  bool isMiddleParaHeader() { return m_type == PARA_TYPE::MIDDLE; }
  bool isLastParaHeader() { return m_type == PARA_TYPE::LAST; }

  void fixFromTemplate();
  string getFixedResult() { return m_result; }

  string getDisplayString();

  int m_startNumber{0};
  string m_color{MAIN_SEPERATOR_COLOR};
  int m_currentParaNo{0};
  bool m_hidden{false};
  bool m_lastPara{false};

private:
  void readType(const string &header);

  void loadFromFirstParaHeader(const string &header);
  void loadFromMiddleParaHeader(const string &header);
  void loadFromLastParaHeader(const string &header);

  void fixFirstParaHeaderFromTemplate();
  void fixMiddleParaHeaderFromTemplate();
  void fixUnhiddenMiddleParaHeaderDispTextFromTemplate();
  void fixLastParaHeaderFromTemplate();

  PARA_TYPE m_type{PARA_TYPE::FIRST};
  string m_result{""};
  string m_displayText{""};
};

static const string defaultUnit = R"(回)";
static const string attachmentUnit = R"(篇)";
static const string searchUnit = R"(条)";

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
