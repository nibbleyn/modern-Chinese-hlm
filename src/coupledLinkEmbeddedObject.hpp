#pragma once
#include "coupledLink.hpp"

class PersonalComment : public Object {
public:
  PersonalComment(const string &fromFile) : m_fromFile(fromFile) {
    m_objectType = OBJECT_TYPE::PERSONALCOMMENT;
  }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};

class PoemTranslation : public Object {
public:
  PoemTranslation(const string &fromFile) : m_fromFile(fromFile) {
    m_objectType = OBJECT_TYPE::POEMTRANSLATION;
  }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};
