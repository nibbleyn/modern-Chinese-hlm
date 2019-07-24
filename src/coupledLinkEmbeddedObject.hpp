#pragma once
#include "coupledLink.hpp"

class PersonalComment : public Object {
public:
  PersonalComment(const string &fromFile) : m_fromFile(fromFile) {}
  string getStartTag() override { return personalCommentStartChars; }
  string getEndTag() override { return personalCommentEndChars; }
  string getName() override { return nameOfPersonalCommentType; }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};

class PoemTranslation : public Object {
public:
  PoemTranslation(const string &fromFile) : m_fromFile(fromFile) {}
  string getStartTag() override { return poemTranslationBeginChars; }
  string getEndTag() override { return poemTranslationEndChars; }
  string getName() override { return nameOfPoemTranslationType; }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};
