#pragma once
#include "coupledLink.hpp"

class PersonalComment : public Object {
  static bool m_addSpecialDisplayText;

public:
  static void enableAddSpecialDisplayText() { m_addSpecialDisplayText = true; };

public:
  PersonalComment(const string &fromFile) : m_fromFile(fromFile) {}
  void setFromFile(const string &fromFile) { m_fromFile = fromFile; }
  string getStartTag() override { return personalCommentStartChars; }
  string getEndTag() override { return personalCommentEndChars; }
  string getName() override { return nameOfPersonalCommentType; }
  string getFormatedFullString() override;
  void shouldHideSubObject(TypeSet &hiddenTypeSet) override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};

class PoemTranslation : public Object {
public:
  PoemTranslation(const string &fromFile) : m_fromFile(fromFile) {}
  void setFromFile(const string &fromFile) { m_fromFile = fromFile; }
  string getStartTag() override { return poemTranslationBeginChars; }
  string getEndTag() override { return poemTranslationEndChars; }
  string getName() override { return nameOfPoemTranslationType; }
  string getFormatedFullString() override;
  void shouldHideSubObject(TypeSet &hiddenTypeSet) override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};
