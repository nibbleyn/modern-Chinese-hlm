#include "coupledLinkEmbeddedObject.hpp"

bool PersonalComment::m_addSpecialDisplayText = false;

static constexpr const char *defaultComment = R"(XX)";
static const string personalCommentTemplate =
    R"(<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">XX</u>)";
static constexpr const char *specialDisplayPrefix = R"(夹批：)";

string PersonalComment::getFormatedFullString() {
  return getStringFromTemplate(personalCommentTemplate, defaultComment);
}

void PersonalComment::shouldHideSubObject(TypeSet &hiddenTypeSet) {
  m_hideSubObject = isObjectTypeInSet(nameOfLinkFromMainType, hiddenTypeSet);
}

size_t PersonalComment::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  if (pos != string::npos)
    m_displayText = scanForSubObjects(m_hideSubObject, m_bodyText, m_fromFile);
  if (m_addSpecialDisplayText)
    m_displayText = specialDisplayPrefix + m_displayText;
  return pos;
}

static constexpr const char *defaultTranslation = R"(XX)";
static const string poemTranslationTemplate =
    R"(<samp unhidden font style="font-size: 12pt; font-family: 宋体; color:#ff00ff">XX</samp>)";

string PoemTranslation::getFormatedFullString() {
  return m_fullString =
             getStringFromTemplate(poemTranslationTemplate, defaultComment);
}

void PoemTranslation::shouldHideSubObject(TypeSet &hiddenTypeSet) {
  m_hideSubObject = isObjectTypeInSet(nameOfLinkFromMainType, hiddenTypeSet);
}

size_t PoemTranslation::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  if (pos != string::npos)
    m_displayText = scanForSubObjects(m_hideSubObject, m_bodyText, m_fromFile);
  return pos;
}

static const string commentTemplate =
    R"(<cite unhidden>XX</cite>)";

string Comment::getFormatedFullString() {
  return m_fullString = getStringFromTemplate(commentTemplate, defaultComment);
}

void Comment::shouldHideSubObject(TypeSet &hiddenTypeSet) {
  m_hideSubObject = isObjectTypeInSet(nameOfLinkFromMainType, hiddenTypeSet);
}

size_t Comment::loadFirstFromContainedLine(const string &containedLine,
                                           size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  if (pos != string::npos)
    m_displayText = scanForSubObjects(m_hideSubObject, m_bodyText, m_fromFile);
  return pos;
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

size_t Citation::loadFirstFromContainedLine(const string &containedLine,
                                            size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  if (pos != string::npos) {
    fromSectionString(m_bodyText);
    if (isValid())
      m_displayText = m_bodyText;
  }
  return pos;
}

void Citation::updateWithAttachmentNumberAndParaLineNumber(
    AttachmentNumber num, ParaLineNumber paraLine) {
  m_num = num;
  m_paraLine = paraLine;
  m_bodyText = getExpectedSection(m_num, m_paraLine);
  if (isValid())
    m_displayText = m_bodyText;
}
