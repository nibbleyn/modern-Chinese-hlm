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

  static bool isObjectTypeInSet(OBJECT_TYPE objType, SET_OF_OBJECT_TYPES &set) {
    return set.count(objType) != 0;
  }
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

  static OBJECT_TYPE getObjectTypeFromName(string name) {
    if (name == nameOfLineNumberType)
      return OBJECT_TYPE::LINENUMBER;
    else if (name == nameOfSpaceType)
      return OBJECT_TYPE::SPACE;
    else if (name == nameOfPoemType)
      return OBJECT_TYPE::POEM;
    else if (name == nameOfLinkFromMainType)
      return OBJECT_TYPE::LINKFROMMAIN;
    else if (name == nameOfPersonalCommentType)
      return OBJECT_TYPE::PERSONALCOMMENT;
    else if (name == nameOfPoemTranslationType)
      return OBJECT_TYPE::POEMTRANSLATION;
    else if (name == nameOfCommentType)
      return OBJECT_TYPE::COMMENT;
    return OBJECT_TYPE::TEXT;
  }

  static string typeSetAsString(SET_OF_OBJECT_TYPES typeSet) {
    string result;
    for (const auto &type : typeSet) {
      result += displaySpace + getNameOfObjectType(type);
    }
    return result;
  }

  static SET_OF_OBJECT_TYPES getTypeSetFromString(const string &str) {
    auto typeListToCheck = {nameOfPoemTranslationType,
                            nameOfPoemType,
                            nameOfPersonalCommentType,
                            nameOfCommentType,
                            nameOfLinkFromAttachmentType,
                            nameOfLinkFromMainType,
                            nameOfTextType};
    string toCheck = str;
    SET_OF_OBJECT_TYPES result;
    for (const auto &nameOfType : typeListToCheck) {
      if (toCheck.find(nameOfType) != string::npos) {
        result.insert(getObjectTypeFromName(nameOfType));
        replacePart(toCheck, nameOfType, emptyString);
      }
    }
    return result;
  }

public:
  Object() = default;
  virtual ~Object(){};

  virtual string getWholeString() = 0;
  virtual string getDisplayString() = 0;
  virtual size_t displaySize() = 0;
  virtual size_t loadFirstFromContainedLine(const string &containedLine,
                                            size_t after = 0) = 0;
  size_t length() {
    if (m_fullString.length() != getWholeString().length())
      METHOD_OUTPUT << " size not match: " << m_fullString.length() << " vs "
                    << getWholeString().length() << endl;
    return m_fullString.length();
  }

protected:
  string m_bodyText{emptyString};
  string m_fullString{emptyString};
};

class Space : public Object {
public:
  Space() = default;
  string getWholeString() override { return space; };
  string getDisplayString() override { return displaySpace; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  size_t displaySize() override { return displaySpace.length(); };
};

class Poem : public Object {
public:
  Poem() = default;
  string getWholeString() override;
  string getDisplayString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  size_t displaySize() override;
};
