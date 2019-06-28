#pragma once
#include "container.hpp"
#include "coupledBodyTextWithLink.hpp"

static const string FORMATED_BODYTEXT_CONTAINER_FILENAME = R"(4)";

class FormatedBodyTextContainer : public Container {
public:
  FormatedBodyTextContainer() = default;
  FormatedBodyTextContainer(const string &filename) : Container(filename) {
    m_htmlInputFilePath = HTML_OUTPUT_MAIN;
    m_htmlOutputFilePath = HTML_OUTPUT_MAIN;
    m_bodyTextInputFilePath = HTML_NON_COUPLED_CONTAINER_PATH;
    m_inputBodyTextFilename = FORMATED_BODYTEXT_CONTAINER_FILENAME;
  }
  void setInputHtmlFilename(const string &filename) {
    m_inputHtmlFilename = filename;
  }
  void numbering();

private:
  string getTempBodyTextFixFilePath() {
    return m_fixedBodyTextFilePath + FORMATED_BODYTEXT_CONTAINER_FILENAME +
           BODY_TEXT_SUFFIX;
  }
  void loadFixedBodyTexts();
  CoupledBodyTextWithLink m_bodyText;
};
