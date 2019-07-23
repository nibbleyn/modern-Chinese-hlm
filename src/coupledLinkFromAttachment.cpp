#include "coupledLink.hpp"

static const string mainDirForLinkFromAttachment = R"(..\)";
static const string originalDirForLinkFromAttachment = R"(..\original\)";
static const string jpmDirForLinkFromAttachment = R"(..\JPM\)";
static const string pictureDirForLinkFromAttachment = R"(..\pictures\)";

/**
 * generate the link to original file after a link to main file from attachment
 */
void LinkFromAttachment::generateLinkToOrigin() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "create link to original main html thru key: " << m_usedKey
                  << endl;
  auto reservedType = m_type;
  // temporarily change type to get right path
  m_type = LINK_TYPE::ORIGINAL;
  string to = fixLinkFromOriginalTemplate(
      getPathOfReferenceFile(), getChapterName(), m_usedKey, emptyString,
      annotationToOriginal, m_referPara);
  m_linkPtrToOrigin = make_unique<LinkFromAttachment>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  needToChange();
  m_type = reservedType;
  m_displayText +=
      bracketStartChars + upArrow + annotationToOriginal + bracketEndChars;
}

/**
 * different from LinkFromMain::getHtmlFileNamePrefix for same page type of link
 */
string LinkFromAttachment::getHtmlFileNamePrefix() {
  string prefix = ATTACHMENT_HTML_PREFIX;
  if (m_type == LINK_TYPE::JPM)
    prefix = JPM_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_HTML_PREFIX;
  if (m_type == LINK_TYPE::MAIN)
    prefix = MAIN_HTML_PREFIX;
  return prefix;
}

bool LinkFromAttachment::isTargetToAttachmentFile() {
  return m_type == LINK_TYPE::SAMEPAGE or m_type == LINK_TYPE::ATTACHMENT;
}

string LinkFromAttachment::getPathOfReferenceFile() const {
  string result{emptyString};
  if (m_type == LINK_TYPE::IMAGE)
    result = pictureDirForLinkFromAttachment;
  if (m_type == LINK_TYPE::MAIN or m_annotation == returnToContentTable)
    result = mainDirForLinkFromAttachment;
  if (m_type == LINK_TYPE::ORIGINAL)
    result = originalDirForLinkFromAttachment;
  if (m_type == LINK_TYPE::JPM)
    result = jpmDirForLinkFromAttachment;
  return result;
}

/**
 * different from LinkFromMain::getHtmlFileNamePrefix for same page type of link
 */
string LinkFromAttachment::getBodyTextFilePrefix() {
  string prefix = ATTACHMENT_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::JPM)
    prefix = JPM_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::MAIN)
    prefix = MAIN_BODYTEXT_PREFIX;
  return prefix;
}
