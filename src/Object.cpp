#include "Object.hpp"

extern int debug;

string getNameOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return "LINENUMBER";
  else if (type == OBJECT_TYPE::SPACE)
    return "SPACE";
  else if (type == OBJECT_TYPE::POEM)
    return "POEM";
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return "LINKFROMMAIN";
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return "PERSONALCOMMENT";
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return "POEMTRANSLATION";
  else if (type == OBJECT_TYPE::COMMENT)
    return "COMMENT";
  return "";
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
  return poemBeginChars + unhiddenDisplayProperty + endOfBeginTag + m_bodyText +
         poemEndChars;
}

string Poem::getDisplayString() { return m_bodyText; }
size_t Poem::displaySize() { return getDisplayString().length(); }
