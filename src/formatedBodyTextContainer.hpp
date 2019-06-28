#pragma once
#include "container.hpp"
#include "coupledBodyTextWithLink.hpp"

static const string FORMATED_BODYTEXT_CONTAINER_FILENAME = R"(4)";

class FormatedBodyTextContainer : public Container {
public:
  FormatedBodyTextContainer() = default;
  FormatedBodyTextContainer(const string &filename) : Container(filename) {
    m_inputHtmlDir = HTML_OUTPUT_MAIN;
    m_outputHtmlDir = HTML_OUTPUT_MAIN;
    m_inputBodyTextDir = HTML_NON_COUPLED_CONTAINER_PATH;
    m_inputBodyTextFilename = FORMATED_BODYTEXT_CONTAINER_FILENAME;
  }
  void setInputHtmlFilename(const string &filename) {
    m_inputHtmlFilename = filename;
  }
  void numbering();

private:
  string getTempBodyTextFixFilePath() {
    return m_fixedBodyTextDir + FORMATED_BODYTEXT_CONTAINER_FILENAME +
           BODY_TEXT_SUFFIX;
  }
  void loadFixedBodyTexts();
  CoupledBodyTextWithLink m_bodyText;
};