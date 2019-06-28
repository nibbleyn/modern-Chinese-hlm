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
  string getBodyTextFilePath() {
    return m_bodyTextDir + m_bodyTextFilename + BODY_TEXT_SUFFIX;
  }

  // dissemble/assemble before/after working on bodyText
  void disableBodyTextWithEndMark() { m_bodyTextWithEndMark = false; }
  void disableHtmlWithEndMark() { m_htmlWithEndMark = false; }
  void dissembleFromHTM();

  void clearFixedBodyTexts();
  virtual void loadFixedBodyTexts();

  void setTitle(const string &title) { m_title = title; }
  void setDisplayTitle(const string &displayTitle) {
    m_displayTitle = displayTitle;
  }
  void assembleBackToHTM();

protected:
  string m_inputHtmlDir{HTML_SRC_MAIN};
  string m_inputHtmlFilename{emptyString};

  string m_outputHtmlDir{HTML_OUTPUT_MAIN};
  string m_outputHtmlFilename{emptyString};

  string m_bodyTextDir{BODY_TEXT_OUTPUT};
  string m_bodyTextFilename{emptyString};

  string m_fixedBodyTextDir{BODY_TEXT_FIX};

  virtual string getInputHtmlFilePath() {
    return m_inputHtmlDir + m_inputHtmlFilename + HTML_SUFFIX;
  }

  bool m_bodyTextWithEndMark{true};
  bool m_htmlWithEndMark{true};

  string m_title{emptyString};
  string m_displayTitle{emptyString};
};
