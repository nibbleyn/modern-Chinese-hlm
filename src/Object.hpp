#pragma once
#include "utf8StringUtil.hpp"

static const string poemTranslationBeginChars = R"(<samp )";
static const string UnhiddenLineNumberStart = R"(<a unhidden id=")";
static const string space = R"(&nbsp;)";
static const string doubleSpace = space + space;
static const string fourSpace = space + space + space + space;
static const string poemBeginChars = R"(<strong )";
static const string poemEndChars = R"(</strong>)";
static const string poemTranslationEndChars = R"(</samp>)";
static const string endOfPoemTranslationBeginTag = R"(">)";

static const string nameOfLineNumberType = R"(LINENUMBER)";
static const string nameOfSpaceType = R"(SPACE)";
static const string nameOfPoemType = R"(POEM)";
static const string nameOfPoemTranslationType = R"(POEMTRANSLATION)";
static const string nameOfLinkFromMainType = R"(LINKFROMMAIN)";
static const string nameOfLinkFromAttachmentType = R"(LINKFROMATTACHMENT)";
static const string nameOfCommentType = R"(COMMENT)";
static const string nameOfPersonalCommentType = R"(PERSONALCOMMENT)";
static const string nameOfTextType = R"(TEXT)";

class Object {
public:
  enum class OBJECT_TYPE {
    SPACE,
    LINENUMBER,
    POEM,
    POEMTRANSLATION,
    LINKFROMMAIN,
    LINKFROMATTACHMENT,
    COMMENT,
    PERSONALCOMMENT,
    TEXT
  };
  using SET_OF_OBJECT_TYPES = set<Object::OBJECT_TYPE>;
  static SET_OF_OBJECT_TYPES setOfObjectTypes;

  static string getStartTagOfObjectType(OBJECT_TYPE type) {
    if (type == OBJECT_TYPE::LINENUMBER)
      return UnhiddenLineNumberStart;
    else if (type == Object::OBJECT_TYPE::SPACE)
      return space;
    else if (type == Object::OBJECT_TYPE::POEM)
      return poemBeginChars;
    else if (type == Object::OBJECT_TYPE::LINKFROMMAIN)
      return linkStartChars;
    else if (type == Object::OBJECT_TYPE::PERSONALCOMMENT)
      return personalCommentStartChars;
    else if (type == Object::OBJECT_TYPE::POEMTRANSLATION)
      return poemTranslationBeginChars;
    else if (type == Object::OBJECT_TYPE::COMMENT)
      return commentBeginChars;
    return emptyString;
  }

  static string getEndTagOfObjectType(OBJECT_TYPE type) {
    if (type == OBJECT_TYPE::LINKFROMMAIN)
      return linkEndChars;
    else if (type == OBJECT_TYPE::PERSONALCOMMENT)
      return personalCommentEndChars;
    else if (type == OBJECT_TYPE::POEMTRANSLATION)
      return poemTranslationEndChars;
    else if (type == OBJECT_TYPE::COMMENT)
      return commentEndChars;
    return emptyString;
  }

  static string getNameOfObjectType(OBJECT_TYPE type) {
    if (type == OBJECT_TYPE::LINENUMBER)
      return nameOfLineNumberType;
    else if (type == OBJECT_TYPE::SPACE)
      return nameOfSpaceType;
    else if (type == OBJECT_TYPE::POEM)
      return nameOfPoemType;
    else if (type == OBJECT_TYPE::LINKFROMMAIN)
      return nameOfLinkFromMainType;
    else if (type == OBJECT_TYPE::PERSONALCOMMENT)
      return nameOfPersonalCommentType;
    else if (type == OBJECT_TYPE::POEMTRANSLATION)
      return nameOfPoemTranslationType;
    else if (type == OBJECT_TYPE::COMMENT)
      return nameOfCommentType;
    else if (type == OBJECT_TYPE::TEXT)
      return nameOfTextType;
    return emptyString;
  }

  static string typeSetAsString(SET_OF_OBJECT_TYPES typeSet) {
    string result;
    for (const auto &type : typeSet) {
      result += " " + getNameOfObjectType(type);
    }
    return result;
  }

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

class Space : public Object {
public:
  Space() = default;
  string getWholeString() { return space; };
  string getDisplayString() { return displaySpace; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t displaySize() { return displaySpace.length(); };
};

class Poem : public Object {
public:
  Poem() = default;
  string getWholeString();
  string getDisplayString();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t displaySize();
};
