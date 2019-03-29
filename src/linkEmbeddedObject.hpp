#pragma once
#include "link.hpp"

static const string personalCommentStartRestChars =
    R"( style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4)";
static const string endOfPersonalCommentBeginTag = R"(">)";

class PersonalComment : public Object {
public:
  PersonalComment(const string &fromFile) : m_fromFile(fromFile) {}
  string getWholeString();
  string getDisplayString();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

private:
  string m_displayText{""};
  string m_fromFile{"81"};
};

// poemTranslation
static const string poemTranslationBeginChars = R"(<samp )";
static const string poemTranslationEndChars = R"(</samp>)";
static const string endOfPoemTranslationBeginTag = R"(">)";

class PoemTranslation : public Object {
public:
  PoemTranslation(const string &fromFile) : m_fromFile(fromFile) {}
  string getWholeString();
  string getDisplayString();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

private:
  string m_displayText{""};
  string m_fromFile{"81"};
};

void testLinkOperation();
