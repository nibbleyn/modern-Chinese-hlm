#include "coupledLinkEmbeddedObject.hpp"

string scanForSubLinks(const string &original, const string &fromFile) {
  string result;
  // start offset -> end offset
  using SubStringOffsetTable = map<size_t, size_t>;
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
    auto current = make_unique<LinkFromMain>(fromFile);
    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset = link.second + endTag.length();
  }
  result += original.substr(endOfSubStringOffset);
  return result;
}

static constexpr const char *defaultTranslation = R"(XX)";
static const string personalCommentTemplate =
    R"(<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">XX</u>)";

string fixPersonalCommentFromTemplate(const string &comment) {
  string result = personalCommentTemplate;
  replacePart(result, defaultTranslation, comment);
  return result;
}

string PersonalComment::getWholeString() {
  return fixPersonalCommentFromTemplate(m_bodyText);
}
string PersonalComment::getDisplayString() {
  if (m_displayType == DISPLAY_TYPE::HIDDEN)
    return emptyString;
  else
    return m_displayText;
}

size_t PersonalComment::displaySize() { return getDisplayString().length(); }

size_t PersonalComment::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, personalCommentStartChars, personalCommentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readDisplayType();
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfBeginTag,
                                            personalCommentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(personalCommentStartChars, after);
}

static constexpr const char *defaultComment = R"(XX)";
static const string poemTranslationTemplate =
    R"(<samp unhidden font style="font-size: 12pt; font-family: 宋体; color:#ff00ff">XX</samp>)";

string fixPoemTranslationFromTemplate(const string &translation) {
  string result = poemTranslationTemplate;
  replacePart(result, defaultComment, translation);
  return result;
}

string PoemTranslation::getWholeString() {
  return fixPoemTranslationFromTemplate(m_bodyText);
}
string PoemTranslation::getDisplayString() {
  if (m_displayType == DISPLAY_TYPE::HIDDEN)
    return emptyString;
  else
    return m_displayText;
}

size_t PoemTranslation::displaySize() { return getDisplayString().length(); }

size_t PoemTranslation::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, poemTranslationBeginChars, poemTranslationEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readDisplayType();
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfBeginTag,
                                            poemTranslationEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(poemTranslationBeginChars, after);
}

static const string commentTemplate =
    R"(<cite unhidden>XX</cite>)";

string fixCommentFromTemplate(const string &comment) {
  string result = commentTemplate;
  replacePart(result, defaultComment, comment);
  return result;
}

string Comment::getWholeString() { return fixCommentFromTemplate(m_bodyText); }
string Comment::getDisplayString() {
  if (m_displayType == DISPLAY_TYPE::HIDDEN)
    return emptyString;
  else
    return m_displayText;
}

size_t Comment::displaySize() { return getDisplayString().length(); }

size_t Comment::loadFirstFromContainedLine(const string &containedLine,
                                           size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, commentBeginChars,
                                           commentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readDisplayType();
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfBeginTag,
                                            commentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(commentBeginChars, after);
}

string Citation::getExpectedSection(AttachmentNumber num,
                                    ParaLineNumber paraLine,
                                    const string &chapterString,
                                    const string &attachmentString,
                                    const string &sectionString) {
  string unitString = chapterString;
  if (num.second != 0)
    unitString +=
        citationChapterNo + TurnToString(num.second) + attachmentString;
  return citationChapterNo + TurnToString(num.first) + unitString +
         TurnToString(paraLine.first) + citationChapterParaSeparator +
         TurnToString(paraLine.second) + sectionString;
}

/**
 * 第3回第9篇1.2节:
 */
void Citation::fromSectionString(const string &citationString,
                                 const string &chapterString,
                                 const string &attachmentString,
                                 const string &sectionString) {
  if (citationString.empty())
    return;
  string chapter = getIncludedStringBetweenTags(
      citationString, citationChapterNo, chapterString);
  if (not chapter.empty())
    m_num.first = TurnToInt(chapter);
  string para;
  string attNo = getIncludedStringBetweenTags(citationString, citationChapterNo,
                                              attachmentString);
  if (not attNo.empty()) {
    m_num.second = TurnToInt(attNo);
    para = getIncludedStringBetweenTags(citationString, attachmentString,
                                        citationChapterParaSeparator);
  } else
    para = getIncludedStringBetweenTags(citationString, chapterString,
                                        citationChapterParaSeparator);
  if (not para.empty())
    m_paraLine.first = TurnToInt(para);
  string line = getIncludedStringBetweenTags(
      citationString, citationChapterParaSeparator, sectionString);
  if (not line.empty())
    m_paraLine.second = TurnToInt(line);
}

string Citation::getWholeString() {
  // display property
  string part0 = citationStartChars;
  if (m_displayType != DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  part0 += displayPropertyAsString();
  return part0 + endOfBeginTag + m_bodyText + citationEndChars;
}

string Citation::getDisplayString() {
  if (m_displayType == DISPLAY_TYPE::HIDDEN or not isValid())
    return emptyString;
  else
    return m_bodyText;
}

size_t Citation::loadFirstFromContainedLine(const string &containedLine,
                                            size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, citationStartChars,
                                           citationEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfBeginTag,
                                            citationEndChars);
  readDisplayType();
  fromSectionString(m_bodyText);
  return containedLine.find(citationStartChars, after);
}
