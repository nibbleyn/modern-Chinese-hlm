#pragma once
#include "paraHeader.hpp"

class Container {
public:
  Container() = default;
  Container(const string &filename) : m_outputHtmlFilename(filename) {}
  virtual ~Container(){};

  string getoutputHtmlFilepath() {
    return m_htmlOutputFilePath + m_outputHtmlFilename + HTML_SUFFIX;
  }
  void setInputHtmlFilename(const string &filename) {
    m_inputHtmlFilename = filename;
    disableUsingDefaultInputHtmlFileName();
  }

  // dissemble/assemble before/after working on bodyText
  void disableBodyTextWithEndMark() { m_bodyTextWithEndMark = false; }
  void disableHtmlWithEndMark() { m_htmlWithEndMark = false; }
  void dissembleFromHTM();
  void setTitle(const string &title) { m_title = title; }
  void setDisplayTitle(const string &displayTitle) {
    m_displayTitle = displayTitle;
  }
  void assembleBackToHTM();

protected:
  string m_htmlInputFilePath{HTML_SRC_MAIN};
  string m_inputHtmlFilename{emptyString};

  string m_htmlOutputFilePath{HTML_OUTPUT_MAIN};
  string m_outputHtmlFilename{emptyString};

  string m_bodyTextInputFilePath{BODY_TEXT_OUTPUT};
  string m_inputBodyTextFilename{emptyString};

  string m_fixedBodyTextFilePath{BODY_TEXT_FIX};

  bool m_disableUsingDefaultInputHtmlFileName{false};
  void disableUsingDefaultInputHtmlFileName() {
    m_disableUsingDefaultInputHtmlFileName = true;
  }

  virtual string getInputHtmlFilePath() {
    return m_htmlInputFilePath + m_inputHtmlFilename + HTML_SUFFIX;
  }
  virtual string getBodyTextFilePath() {
    return m_bodyTextInputFilePath + m_inputBodyTextFilename + BODY_TEXT_SUFFIX;
  }

  bool m_bodyTextWithEndMark{true};
  bool m_htmlWithEndMark{true};

  string m_title{emptyString};
  string m_displayTitle{emptyString};
};
