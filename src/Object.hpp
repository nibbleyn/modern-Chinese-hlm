#pragma once
#include "utf8StringUtil.hpp"

// do not add space in these definitions
// since two spaces might result in addition
static const string linkStartChars = R"(<a)";
static const string linkEndChars = R"(</a>)";
static const string titleStartChars = R"(title=")";
static const string titleEndChars = R"(")"; // ! contained by titleStartChars

// line number is placeholder hyperlink auto generated once for each line
// since a ParagraphHeader would have href attribute and is a line number also
static const string LineNumberIdentity = R"(id=")";
static const string UnhiddenLineNumberStart = linkStartChars + displaySpace +
                                              unhiddenDisplayProperty +
                                              displaySpace + LineNumberIdentity;
static const string HiddenLineNumberStart = linkStartChars + displaySpace +
                                            hiddenDisplayProperty +
                                            displaySpace + LineNumberIdentity;
static const string DirectLineNumberStart =
    linkStartChars + displaySpace + LineNumberIdentity;
static const string LineNumberEnd = R"(</a>)";

static const string space = R"(&nbsp;)";
static const string doubleSpace = space + space;
static const string fourSpace = space + space + space + space;

static const string poemBeginChars = R"(<strong)";
static const string poemEndChars = R"(</strong>)";

static const string poemTranslationBeginChars = R"(<samp)";
static const string poemTranslationEndChars = R"(</samp>)";

// comments
static const string commentBeginChars = R"(<cite)";
static const string commentEndChars = R"(</cite>)";
static const string commentStart = bracketStartChars + commentBeginChars;
static const string commentEnd = commentEndChars + bracketEndChars;

static const string personalCommentStartChars = R"(<u)";
static const string personalCommentStartRestChars =
    R"( style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4)";
static const string personalCommentEndChars = R"(</u>)";

static const string nameOfLineNumberType = R"(LINENUMBER)";
static const string nameOfSpaceType = R"(SPACE)";
static const string nameOfPoemType = R"(POEM)";
static const string nameOfPoemTranslationType = R"(POEMTRANSLATION)";
static const string nameOfLinkFromMainType = R"(LINKFROMMAIN)";
static const string nameOfLinkFromAttachmentType = R"(LINKFROMATTACHMENT)";
static const string nameOfCommentType = R"(COMMENT)";
static const string nameOfPersonalCommentType = R"(PERSONALCOMMENT)";
static const string nameOfTextType = R"(TEXT)";

enum class DISPLAY_TYPE { DIRECT, HIDDEN, UNHIDDEN };

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
      return linkStartChars;
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
    if (m_fullString.length() != getWholeString().length()) {
      METHOD_OUTPUT << " size not match: " << m_fullString.length() << " vs "
                    << getWholeString().length() << endl;
      METHOD_OUTPUT << m_fullString << endl;
      METHOD_OUTPUT << getWholeString() << endl;
    }
    return m_fullString.length();
  }
  void readDisplayType();
  void hide() { m_displayType = DISPLAY_TYPE::HIDDEN; }
  void unhide() { m_displayType = DISPLAY_TYPE::UNHIDDEN; }
  string displayPropertyAsString() {
    string result{emptyString};
    if (m_displayType == DISPLAY_TYPE::UNHIDDEN)
      result = unhiddenDisplayProperty;
    if (m_displayType == DISPLAY_TYPE::HIDDEN)
      result = hiddenDisplayProperty;
    return result;
  }

protected:
  DISPLAY_TYPE getDisplayType() { return m_displayType; }
  string m_bodyText{emptyString};
  string m_fullString{emptyString};
  DISPLAY_TYPE m_displayType{DISPLAY_TYPE::UNHIDDEN};
  OBJECT_TYPE m_objectType{OBJECT_TYPE::TEXT};
};

class Space : public Object {
public:
  Space() { m_objectType = OBJECT_TYPE::SPACE; }
  string getWholeString() override { return space; };
  string getDisplayString() override { return displaySpace; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  size_t displaySize() override { return displaySpace.length(); };
  void readDisplayType() { m_displayType = DISPLAY_TYPE::DIRECT; }
};

class Poem : public Object {
public:
  Poem() { m_objectType = OBJECT_TYPE::POEM; }
  string getWholeString() override;
  string getDisplayString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  size_t displaySize() override;
};
