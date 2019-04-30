#pragma once
#include <string>

static const std::string defaultTitle = R"(XXX)";
static const std::string defaultDisplayTitle = R"(YYY)";

class Container {
public:
  Container() = default;
  virtual ~Container(){};

protected:
  std::string m_htmlInputFilePath;
  std::string m_htmlOutputFilePath;
  std::string m_bodyTextInputFilePath;
  std::string m_bodyTextOutputFilePath;
  virtual std::string getInputHtmlFilePath() = 0;
};
