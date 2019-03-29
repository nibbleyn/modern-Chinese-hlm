#include "linkEmbeddedObject.hpp"

string scanForSubLinks(const string &original, const string &fromFile) {
  string result;
  using SubStringOffsetTable =
      std::map<size_t, size_t>; // start offset -> end offset
  SubStringOffsetTable subStrings;
  string startTag = linkStartChars;
  string endTag = linkEndChars;
  auto offset = original.find(startTag);
  do {
    if (offset == string::npos)
      break;
    subStrings[offset] = original.find(endTag, offset);
    offset = original.find(startTag, offset + 1);
  } while (true);
  auto endOfSubStringOffset = 0;
  for (const auto &link : subStrings) {
    result += original.substr(endOfSubStringOffset,
                              link.first - endOfSubStringOffset);
    auto current = std::make_unique<LinkFromMain>(fromFile);

    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset = link.second + endTag.length();
  }
  result += original.substr(endOfSubStringOffset);
  return result;
}

static const string personalCommentTemplate =
    R"(<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">XX</u>)";

string fixPersonalCommentFromTemplate(const string &comment) {
  string result = personalCommentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string PersonalComment::getWholeString() {
  return fixPersonalCommentFromTemplate(m_bodyText);
}
string PersonalComment::getDisplayString() { return m_displayText; }

size_t PersonalComment::displaySize() { return getDisplayString().length(); }

size_t PersonalComment::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, personalCommentStartChars, personalCommentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(
      m_fullString, endOfPersonalCommentBeginTag, personalCommentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(personalCommentStartChars, after);
}

static const string poemTranslationTemplate =
    R"(<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">XX</samp>)";

string fixPoemTranslationFromTemplate(const string &translation) {
  string result = poemTranslationTemplate;
  replacePart(result, "XX", translation);
  return result;
}

string PoemTranslation::getWholeString() {
  return fixPoemTranslationFromTemplate(m_bodyText);
}
string PoemTranslation::getDisplayString() { return m_displayText; }

size_t PoemTranslation::displaySize() { return getDisplayString().length(); }

size_t PoemTranslation::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, poemTranslationBeginChars, poemTranslationEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(
      m_fullString, endOfPoemTranslationBeginTag, poemTranslationEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(poemTranslationBeginChars, after);
}

static const string commentTemplate =
    R"(<cite unhidden>XX</cite>)";

string fixCommentFromTemplate(const string &comment) {
  string result = commentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string Comment::getWholeString() { return fixCommentFromTemplate(m_bodyText); }
string Comment::getDisplayString() { return m_displayText; }

size_t Comment::displaySize() { return getDisplayString().length(); }

size_t Comment::loadFirstFromContainedLine(const string &containedLine,
                                           size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, commentBeginChars,
                                           commentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfCommentBeginTag,
                                            commentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(commentBeginChars, after);
}

