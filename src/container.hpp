#pragma once
#include "paraHeader.hpp"

class Container {
public:
  Container() = default;
  virtual ~Container(){};
  void disableBodyTextWithEndMark() { m_bodyTextWithEndMark = false; }
  void disableHtmlWithEndMark() { m_htmlWithEndMark = false; }

  // dissemble/assemble before/after working on bodyText
  void dissembleFromHTM();
  void assembleBackToHTM();
  void setTitle(const string &title){m_title = title;}
  void setDisplayTitle(const string &displayTitle){m_displayTitle = displayTitle;}

protected:
  string m_htmlInputFilePath{HTML_SRC_MAIN};
  string m_htmlOutputFilePath{HTML_OUTPUT_MAIN};
  string m_bodyTextInputFilePath{BODY_TEXT_OUTPUT};
  string m_bodyTextOutputFilePath{BODY_TEXT_FIX};
  virtual string getInputHtmlFilePath() = 0;
  virtual string getoutputHtmlFilepath() = 0;
  virtual string getBodyTextFilePath() = 0;
  bool m_bodyTextWithEndMark{true};
  bool m_htmlWithEndMark{true};
  string m_title{emptyString};
  string m_displayTitle{emptyString};
};
