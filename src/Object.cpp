#include "Object.hpp"

extern int debug;

string Object::typeSetAsString(TypeSet typeSet) {
  string result;
  for (const auto &type : typeSet) {
    result += displaySpace + type;
  }
  return result;
}

TypeSet Object::getTypeSetFromString(const string &str) {
  auto typeListToCheck = {nameOfPoemTranslationType,
                          nameOfPoemType,
                          nameOfPersonalCommentType,
                          nameOfCommentType,
                          nameOfLinkFromAttachmentType,
                          nameOfLinkFromMainType,
                          nameOfTextType};
  string toCheck = str;
  TypeSet resultSet;
  for (const auto &nameOfType : typeListToCheck) {
    if (toCheck.find(nameOfType) != string::npos) {
      resultSet.insert(nameOfType);
      replacePart(toCheck, nameOfType, emptyString);
    }
  }
  return resultSet;
}

void Object::readDisplayType() {
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << m_fullString << endl;
    METHOD_OUTPUT << "m_objectType: " << getName() << endl;
  }
  auto containedPart =
      getIncludedStringBetweenTags(m_fullString, getStartTag(), endOfBeginTag);
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
  string part0 = getStartTag();
  if (m_displayType != DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  part0 += displayPropertyAsString();
  return part0 + endOfBeginTag + m_bodyText + getEndTag();
}

string Object::getStringFromTemplate(const string &templateStr,
                                     const string &defaultBodyText) {
  string result = templateStr;
  replacePart(result, defaultBodyText, m_bodyText);
  return result;
}

size_t
Object::getFullStringAndBodyTextFromContainedLine(const string &containedLine,
                                                  size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, getStartTag(),
                                           getEndTag(), after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readDisplayType();
  m_bodyText =
      getIncludedStringBetweenTags(m_fullString, endOfBeginTag, getEndTag());
  return containedLine.find(getStartTag(), after);
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
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  if (pos != string::npos)
    m_displayText = m_bodyText;
  return pos;
}
