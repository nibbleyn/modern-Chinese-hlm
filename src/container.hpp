#pragma once
#include "paraHeader.hpp"

static const string defaultTitle = R"(XXX)";
static const string defaultDisplayTitle = R"(YYY)";

class Container {
public:
  Container() = default;
  virtual ~Container(){};

protected:
  string m_htmlInputFilePath{HTML_SRC_MAIN};
  string m_htmlOutputFilePath{HTML_OUTPUT_MAIN};
  string m_bodyTextInputFilePath{BODY_TEXT_OUTPUT};
  string m_bodyTextOutputFilePath{BODY_TEXT_FIX};
  virtual string getInputHtmlFilePath() = 0;
};
