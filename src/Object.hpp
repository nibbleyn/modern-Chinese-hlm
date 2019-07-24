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

static const string citationStartChars = R"(<sub)";
static const string citationEndChars = R"(</sub>)";

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
static const string nameOfLinkFromJPMType = R"(LINKFROMJPM)";
static const string nameOfCommentType = R"(COMMENT)";
static const string nameOfPersonalCommentType = R"(PERSONALCOMMENT)";
static const string nameOfTextType = R"(TEXT)";
static const string nameOfCitationType = R"(CITATION)";

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
    CITATION,
    PERSONALCOMMENT,
    TEXT
  };
  using SET_OF_OBJECT_TYPES = set<Object::OBJECT_TYPE>;
  static SET_OF_OBJECT_TYPES setOfObjectTypes;
  using ObjectTypeToString = map<OBJECT_TYPE, string>;
  static ObjectTypeToString StartTags;
  static ObjectTypeToString EndTags;
  static ObjectTypeToString ObjectNames;
  using StringToObjectType = map<string, OBJECT_TYPE>;
  static StringToObjectType ObjectTypes;

  static bool isObjectTypeInSet(OBJECT_TYPE objType, SET_OF_OBJECT_TYPES &set) {
    return set.count(objType) != 0;
  }

  static string getNameOfObjectType(OBJECT_TYPE type) {
    try {
      auto endTag = ObjectNames.at(type);
      return endTag;
    } catch (exception &) {
      return emptyString;
    }
  }

  static OBJECT_TYPE getObjectTypeFromName(string name) {
    try {
      auto type = ObjectTypes.at(name);
      return type;
    } catch (exception &) {
      return OBJECT_TYPE::TEXT;
    }
  }

  static string typeSetAsString(SET_OF_OBJECT_TYPES typeSet);
  static SET_OF_OBJECT_TYPES getTypeSetFromString(const string &str);

public:
  Object() = default;
  virtual ~Object(){};
  virtual string getStartTag() = 0;
  virtual string getEndTag() = 0;
  virtual string getName() = 0;
  virtual size_t loadFirstFromContainedLine(const string &containedLine,
                                            size_t after = 0) = 0;
  virtual string getWholeString() = 0;
  string getDisplayString() {
    if (m_displayType == DISPLAY_TYPE::HIDDEN)
      return emptyString;
    else
      return m_displayText;
  };
  size_t displaySize() { return getDisplayString().length(); };
  size_t length() {
    if (m_fullString.length() != getWholeString().length()) {
      METHOD_OUTPUT << " size not match: " << m_fullString.length() << " vs "
                    << getWholeString().length() << endl;
      METHOD_OUTPUT << "m_fullString: " << m_fullString << endl;
      METHOD_OUTPUT << "getWholeString(): " << getWholeString() << endl;
    }
    return m_fullString.length();
  }
  void readDisplayType();
  string getStringWithTags();
  string getStringFromTemplate(const string &templateStr,
                               const string &defaultBodyText);

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
  size_t getFullStringAndBodyTextFromContainedLine(const string &containedLine,
                                                   size_t after);
  DISPLAY_TYPE getDisplayType() { return m_displayType; }
  string m_fullString{emptyString};
  string m_bodyText{emptyString};
  string m_displayText{emptyString};
  DISPLAY_TYPE m_displayType{DISPLAY_TYPE::UNHIDDEN};
};

class Space : public Object {
public:
  Space() {
    m_displayText = displaySpace;
    m_fullString = space;
  }
  string getStartTag() override { return space; }
  string getEndTag() override { return emptyString; }
  string getName() override { return nameOfSpaceType; }
  string getWholeString() override { return space; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
};

class Poem : public Object {
public:
  Poem() = default;
  string getStartTag() override { return poemBeginChars; }
  string getEndTag() override { return poemEndChars; }
  string getName() override { return nameOfPoemType; }
  string getWholeString() override { return getStringWithTags(); }
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
};
