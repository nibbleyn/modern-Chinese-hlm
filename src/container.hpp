#pragma once
#include "paraHeader.hpp"

static const string HTML_NON_COUPLED_CONTAINER_PATH = R"(container/)";

class Container {
public:
  Container() = default;
  Container(const string &filename) : m_outputHtmlFilename(filename) {}
  virtual ~Container(){};

  string getoutputHtmlFilepath() {
    return m_outputHtmlDir + m_outputHtmlFilename + HTML_SUFFIX;
  }

  // dissemble/assemble before/after working on bodyText
  void disableBodyTextWithEndMark() { m_bodyTextWithEndMark = false; }
  void disableHtmlWithEndMark() { m_htmlWithEndMark = false; }
  void dissembleFromHTM();
  void clearFixedBodyTexts();
  void setTitle(const string &title) { m_title = title; }
  void setDisplayTitle(const string &displayTitle) {
    m_displayTitle = displayTitle;
  }
  void loadFixedBodyTexts();
  void assembleBackToHTM();

protected:
  string m_inputHtmlDir{HTML_SRC_MAIN};
  string m_inputHtmlFilename{emptyString};

  string m_outputHtmlDir{HTML_OUTPUT_MAIN};
  string m_outputHtmlFilename{emptyString};

  string m_inputBodyTextDir{BODY_TEXT_OUTPUT};
  string m_inputBodyTextFilename{emptyString};

  string m_fixedBodyTextDir{BODY_TEXT_FIX};

  virtual string getInputHtmlFilePath() {
    return m_inputHtmlDir + m_inputHtmlFilename + HTML_SUFFIX;
  }
  virtual string getBodyTextFilePath() {
    return m_inputBodyTextDir + m_inputBodyTextFilename + BODY_TEXT_SUFFIX;
  }

  bool m_bodyTextWithEndMark{true};
  bool m_htmlWithEndMark{true};

  string m_title{emptyString};
  string m_displayTitle{emptyString};
};
