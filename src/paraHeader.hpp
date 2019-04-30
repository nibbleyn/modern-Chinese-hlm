#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

static const string topIdBeginChars = R"(id="top")";
static const string bottomIdBeginChars = R"(id="bottom")";

enum class PARA_TYPE { FIRST, MIDDLE, LAST };

static const int DEFAULT_START_PARA_NUMBER = 90;

class ParaHeader {
public:
  virtual ~ParaHeader(){};
  void readType(const string &header);
  void loadFrom(const string &header);

  void setStartNumber(int num) { m_startNumber = num; }
  void setCurrentParaNo(int num) { m_currentParaNo = num; }
  void setColor(const string &color) { m_color = color; }

  // if not load from string
  void markAsFirstParaHeader() { m_type = PARA_TYPE::FIRST; }
  void markAsMiddleParaHeader() { m_type = PARA_TYPE::MIDDLE; }
  void markAsLastMiddleParaHeader(bool last) { m_lastPara = last; }
  void markAsLastParaHeader() { m_type = PARA_TYPE::LAST; }

  bool isFirstParaHeader() { return m_type == PARA_TYPE::FIRST; }
  bool isMiddleParaHeader() { return m_type == PARA_TYPE::MIDDLE; }
  bool isLastParaHeader() { return m_type == PARA_TYPE::LAST; }

  void fixFromTemplate();
  string getFixedResult() { return m_result; }

  virtual string getDisplayString() = 0;

protected:
  void loadFromFirstParaHeader(const string &header);
  void loadFromMiddleParaHeader(const string &header);
  void loadFromLastParaHeader(const string &header);

  virtual void fixFirstParaHeaderFromTemplate() = 0;
  virtual void fixMiddleParaHeaderFromTemplate() = 0;
  virtual void fixLastParaHeaderFromTemplate() = 0;

  int m_startNumber{DEFAULT_START_PARA_NUMBER};
  string m_color{MAIN_SEPERATOR_COLOR};
  int m_currentParaNo{0};
  bool m_hidden{false};
  bool m_lastPara{false};

  PARA_TYPE m_type{PARA_TYPE::FIRST};
  string m_result{""};
  string m_displayText{""};
};

class CoupledParaHeader : public ParaHeader {
  static const string firstParaHeader;
  static const string MiddleParaHeader;
  static const string lastParaHeader;

  static const string firstParaHeaderDispText;
  static const string MiddleParaHeaderDispText;
  static const string lastParaHeaderDispText;

public:
  string getDisplayString();

private:
  void fixFirstParaHeaderFromTemplate();
  void fixMiddleParaHeaderFromTemplate();
  void fixUnhiddenMiddleParaHeaderDispTextFromTemplate();
  void fixLastParaHeaderFromTemplate();
};

class GenericParaHeader : public ParaHeader {
  static const string firstParaHeader;
  static const string MiddleParaHeader;
  static const string lastParaHeader;

public:
  void setUnits(const string &unit) { m_units = unit; }
  void setTotalParaNumber(int total) { m_totalPara = total; }
  void setpreTotalParaNumber(int total) { m_preTotalPara = total; }
  void setSeqOfPara(int seq) { m_seqOfPara = seq; }
  void setStartParaNo(int num) { m_startParaNo = num; }
  void setEndParaNo(int num) { m_endParaNo = num; }

  string getDisplayString() { return emptyString; }

private:
  string m_units{defaultUnit};
  int m_totalPara{0};
  int m_preTotalPara{0};
  int m_seqOfPara{0};
  int m_startParaNo{0};
  int m_endParaNo{0};
  void fixFirstParaHeaderFromTemplate();
  void fixMiddleParaHeaderFromTemplate();
  void fixLastParaHeaderFromTemplate();
};
