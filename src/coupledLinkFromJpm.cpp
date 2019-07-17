#include "coupledLink.hpp"

static const string mainDirForLinkFromJPM = R"(..\)";
static const string originalDirForLinkFromJPM = R"(..\original\)";
static const string jpmDirForLinkFromJPM = R"(..\JPM\)";
static const string pictureDirForLinkFromJPM = R"(..\pictures\)";

/**
 * generate the link to original file after a link to main file from attachment
 */
void LinkFromJPM::generateLinkToOrigin() {}

/**
 * different from LinkFromMain::getHtmlFileNamePrefix for same page type of link
 */
string LinkFromJPM::getHtmlFileNamePrefix() {
  string prefix = JPM_HTML_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = JPM_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_HTML_PREFIX;
  if (m_type == LINK_TYPE::MAIN)
    prefix = MAIN_HTML_PREFIX;
  return prefix;
}

bool LinkFromJPM::isTargetToAttachmentFile() {
  return m_type == LINK_TYPE::ATTACHMENT;
}

string LinkFromJPM::getPathOfReferenceFile() const {
  string result{emptyString};
  if (m_type == LINK_TYPE::IMAGE)
    result = pictureDirForLinkFromJPM;
  if (m_type == LINK_TYPE::MAIN or m_annotation == returnToContentTable)
    result = mainDirForLinkFromJPM;
  if (m_type == LINK_TYPE::ORIGINAL)
    result = originalDirForLinkFromJPM;
  if (m_type == LINK_TYPE::JPM)
    result = jpmDirForLinkFromJPM;
  return result;
}

/**
 * different from LinkFromMain::getHtmlFileNamePrefix for same page type of link
 */
string LinkFromJPM::getBodyTextFilePrefix() {
  string prefix = JPM_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = ATTACHMENT_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::MAIN)
    prefix = MAIN_BODYTEXT_PREFIX;
  return prefix;
}
