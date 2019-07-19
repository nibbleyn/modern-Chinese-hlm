#include "Object.hpp"

extern int debug;

Object::SET_OF_OBJECT_TYPES Object::setOfObjectTypes = {
    Object::OBJECT_TYPE::LINENUMBER,      Object::OBJECT_TYPE::POEM,
    Object::OBJECT_TYPE::POEMTRANSLATION, Object::OBJECT_TYPE::LINKFROMMAIN,
    Object::OBJECT_TYPE::COMMENT,         Object::OBJECT_TYPE::PERSONALCOMMENT};

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

size_t Space::loadFirstFromContainedLine(const string &containedLine,
                                         size_t after) {
  m_bodyText = displaySpace;
  m_fullString = space;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
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
  m_bodyText =
      getIncludedStringBetweenTags(m_fullString, endOfBeginTag, poemEndChars);
  return containedLine.find(poemBeginChars, after);
}

string Poem::getWholeString() {
  // display property
  string part0 = poemBeginChars;
  if (m_displayType != DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  part0 += displayPropertyAsString();
  return part0 + endOfBeginTag + m_bodyText + poemEndChars;
}

string Poem::getDisplayString() {
  if (m_displayType == DISPLAY_TYPE::HIDDEN)
    return emptyString;
  else
    return m_bodyText;
}
size_t Poem::displaySize() { return getDisplayString().length(); }
