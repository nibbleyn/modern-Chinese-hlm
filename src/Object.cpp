#include "Object.hpp"

extern int debug;

Object::SET_OF_OBJECT_TYPES Object::setOfObjectTypes = {
    Object::OBJECT_TYPE::LINENUMBER,      Object::OBJECT_TYPE::POEM,
    Object::OBJECT_TYPE::POEMTRANSLATION, Object::OBJECT_TYPE::LINKFROMMAIN,
    Object::OBJECT_TYPE::COMMENT,         Object::OBJECT_TYPE::PERSONALCOMMENT};

Object::ObjectTypeToString Object::StartTags = {
    {OBJECT_TYPE::LINENUMBER, linkStartChars},
    {OBJECT_TYPE::SPACE, space},
    {OBJECT_TYPE::POEM, poemBeginChars},
    {OBJECT_TYPE::LINKFROMMAIN, linkStartChars},
    {OBJECT_TYPE::PERSONALCOMMENT, personalCommentStartChars},
    {OBJECT_TYPE::POEMTRANSLATION, poemTranslationBeginChars},
    {OBJECT_TYPE::COMMENT, commentBeginChars},
    {OBJECT_TYPE::CITATION, citationStartChars}};

Object::ObjectTypeToString Object::EndTags = {
    {OBJECT_TYPE::LINENUMBER, linkEndChars},
    {OBJECT_TYPE::SPACE, emptyString},
    {OBJECT_TYPE::POEM, poemEndChars},
    {OBJECT_TYPE::LINKFROMMAIN, linkEndChars},
    {OBJECT_TYPE::PERSONALCOMMENT, personalCommentEndChars},
    {OBJECT_TYPE::POEMTRANSLATION, poemTranslationEndChars},
    {OBJECT_TYPE::COMMENT, commentEndChars},
    {OBJECT_TYPE::CITATION, citationEndChars}};

static const string nameOfLineNumberType = R"(LINENUMBER)";
static const string nameOfSpaceType = R"(SPACE)";
static const string nameOfPoemType = R"(POEM)";
static const string nameOfPoemTranslationType = R"(POEMTRANSLATION)";
static const string nameOfLinkFromMainType = R"(LINKFROMMAIN)";
static const string nameOfLinkFromAttachmentType = R"(LINKFROMATTACHMENT)";
static const string nameOfCommentType = R"(COMMENT)";
static const string nameOfPersonalCommentType = R"(PERSONALCOMMENT)";
static const string nameOfTextType = R"(TEXT)";
static const string nameOfCitationType = R"(CITATION)";

Object::ObjectTypeToString Object::ObjectNames = {
    {OBJECT_TYPE::LINENUMBER, nameOfLineNumberType},
    {OBJECT_TYPE::SPACE, nameOfSpaceType},
    {OBJECT_TYPE::POEM, nameOfPoemType},
    {OBJECT_TYPE::LINKFROMMAIN, nameOfLinkFromMainType},
    {OBJECT_TYPE::PERSONALCOMMENT, nameOfPersonalCommentType},
    {OBJECT_TYPE::POEMTRANSLATION, nameOfPoemTranslationType},
    {OBJECT_TYPE::COMMENT, nameOfCommentType},
    {OBJECT_TYPE::CITATION, nameOfCitationType},
    {OBJECT_TYPE::TEXT, nameOfTextType}};

Object::StringToObjectType Object::ObjectTypes = {
    {nameOfLineNumberType, OBJECT_TYPE::LINENUMBER},
    {nameOfSpaceType, OBJECT_TYPE::SPACE},
    {nameOfPoemType, OBJECT_TYPE::POEM},
    {nameOfLinkFromMainType, OBJECT_TYPE::LINKFROMMAIN},
    {nameOfPersonalCommentType, OBJECT_TYPE::PERSONALCOMMENT},
    {nameOfPoemTranslationType, OBJECT_TYPE::POEMTRANSLATION},
    {nameOfCommentType, OBJECT_TYPE::COMMENT},
    {nameOfCitationType, OBJECT_TYPE::CITATION},
    {nameOfTextType, OBJECT_TYPE::TEXT}};

string Object::typeSetAsString(SET_OF_OBJECT_TYPES typeSet) {
  string result;
  for (const auto &type : typeSet) {
    result += displaySpace + getNameOfObjectType(type);
  }
  return result;
}

Object::SET_OF_OBJECT_TYPES Object::getTypeSetFromString(const string &str) {
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

void Object::readDisplayType() {
  auto containedPart = getIncludedStringBetweenTags(
      m_fullString, getStartTagOfObjectType(m_objectType), endOfBeginTag);
  if (containedPart.find(unhiddenDisplayProperty) != string::npos) {
    m_displayType = DISPLAY_TYPE::UNHIDDEN;
  } else if (containedPart.find(hiddenDisplayProperty) != string::npos) {
    m_displayType = DISPLAY_TYPE::HIDDEN;
  } else {
    m_displayType = DISPLAY_TYPE::DIRECT;
  }
  if (debug >= LOG_INFO) {
    if (m_displayType == DISPLAY_TYPE::DIRECT)
      METHOD_OUTPUT << "display Type: "
                    << "direct." << endl;
    else
      METHOD_OUTPUT << "display Type: " << displayPropertyAsString() << endl;
  }
}

string Object::getStringWithTags() {
  // display property
  string part0 = getStartTagOfObjectType(m_objectType);
  if (m_displayType != DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  part0 += displayPropertyAsString();
  return part0 + endOfBeginTag + m_bodyText +
         getEndTagOfObjectType(m_objectType);
}

string Object::getStringFromTemplate(const string &templateStr,
                                     const string &defaultBodyText) {
  string result = templateStr;
  replacePart(result, defaultBodyText, m_bodyText);
  return result;
}

size_t Space::loadFirstFromContainedLine(const string &containedLine,
                                         size_t after) {
  m_bodyText = displaySpace;
  m_fullString = space;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_displayType = DISPLAY_TYPE::DIRECT;
  return containedLine.find(space, after);
}

size_t Poem::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, poemBeginChars,
                                           poemEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readDisplayType();
  m_bodyText =
      getIncludedStringBetweenTags(m_fullString, endOfBeginTag, poemEndChars);
  m_displayText = m_bodyText;
  return containedLine.find(poemBeginChars, after);
}
