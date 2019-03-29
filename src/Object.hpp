#pragma once
#include "utf8StringUtil.hpp"

enum class OBJECT_TYPE {
  SPACE,
  LINENUMBER,
  POEM,
  POEMTRANSLATION,
  LINKFROMMAIN,
  LINKFROMATTACHMENT,
  COMMENT,
  PERSONALCOMMENT,
  BR
};

string getNameOfObjectType(OBJECT_TYPE type);

class Object {
public:
  Object() = default;
  virtual ~Object(){};
  virtual string getWholeString() = 0;
  virtual string getDisplayString() = 0;
  size_t length() {
    if (m_fullString.length() != getWholeString().length())
      METHOD_OUTPUT << " size not match: " << m_fullString.length() << " vs "
                    << getWholeString().length() << endl;
    return m_fullString.length();
  }
  virtual size_t displaySize() = 0;
  virtual size_t loadFirstFromContainedLine(const string &containedLine,
                                            size_t after = 0) = 0;

protected:
  string m_bodyText{""};
  string m_fullString{""};
};

static const string space = R"(&nbsp;)";
static const string doubleSpace = space + space;
static const string fourSpace = space + space + space + space;
class Space : public Object {
public:
  Space() = default;
  string getWholeString() { return space; };
  string getDisplayString() { return displaySpace; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t displaySize() { return displaySpace.length(); };

private:
};

static const string poemBeginChars = R"(<strong )";
static const string poemEndChars = R"(</strong>)";
class Poem : public Object {
public:
  Poem() = default;
  string getWholeString();
  string getDisplayString();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t displaySize();

private:
};
