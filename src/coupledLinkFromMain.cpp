#include "coupledLink.hpp"

FileSet LinkFromMain::keyMissingChapters;

void LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList() {
  keyMissingChapters.clear();
  attachmentTable.loadReferenceAttachmentList();
}

void LinkFromMain::updateReferenceAttachmentListIntoFile() {
  attachmentTable.saveAttachmentList();
}

void LinkFromMain::displayMainFilesOfMissingKey() {
  if (keyMissingChapters.empty())
    return;
  FUNCTION_OUTPUT << "files which has missing key links:" << endl;
  for (const auto &file : keyMissingChapters) {
    FUNCTION_OUTPUT << MAIN_HTML_PREFIX + file + HTML_SUFFIX << endl;
  }
}

/**
 * record this file as one who has links of wrong/un-found key
 */
void LinkFromMain::recordMissingKeyLink() {
  keyMissingChapters.insert(m_fromFile);
}

/**
 * generate the link to original file after a link to main file from main
 */
void LinkFromMain::generateLinkToOrigin() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "create link to original main html thru key: " << m_usedKey
                  << endl;
  // only LINK_TYPE::MAIN has origin member
  auto reservedType = m_type;
  // temporarily change type to get right path
  m_type = LINK_TYPE::ORIGINAL;
  string to = fixLinkFromOriginalTemplate(
      getPathOfReferenceFile(), getChapterName(), m_usedKey, emptyString,
      annotationToOriginal, m_referPara);
  m_linkPtrToOrigin = make_unique<LinkFromMain>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  m_needChange = true;
  m_type = reservedType;
  m_displayText +=
      bracketStartChars + upArrow + annotationToOriginal + bracketEndChars;
}

/**
 * from the type of link to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * all jpm files are dXXX.htm
 * @param type type of link
 * @return filename prefix of target file
 */
string LinkFromMain::getHtmlFileNamePrefix() {
  string prefix = MAIN_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_HTML_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = ATTACHMENT_HTML_PREFIX;
  if (m_type == LINK_TYPE::JPM)
    prefix = JPM_HTML_PREFIX;
  return prefix;
}

bool LinkFromMain::isTargetToAttachmentFile() {
  return m_type == LINK_TYPE::ATTACHMENT;
}

/**
 *  the directory structure is like below
 *  refer to utf8HTML/src
 *  \              <-link from main to access main
 *  \attachment\   <-link from attachment to access main to access other
 *  \original\     <- no link in text here
 *  \jpm\          <- link to jpm
 *  \pictures\     <- targets of image type of link
 *  \a*.htm        <- main texts
 * @param linkString the link to check
 * @return the level difference between a link and its target
 */
string LinkFromMain::getPathOfReferenceFile() const {
  string result{emptyString};
  if (m_type == LINK_TYPE::IMAGE)
    result = pictureDirForLinkFromMain;
  if (m_type == LINK_TYPE::ATTACHMENT)
    result = attachmentDirForLinkFromMain;
  if (m_type == LINK_TYPE::ORIGINAL)
    result = originalDirForLinkFromMain;
  if (m_type == LINK_TYPE::JPM)
    result = jpmDirForLinkFromMain;
  return result;
}

/**
 * log a valid link found by putting it into linksTable
 * which is sorted by ChapterName, referPara, usedKey
 */
/**
 * from the type of link to get the filename prefix of bodyText file
 * all main bodyText files are MainXX.txt
 * all attachment bodyText files are AttachXX.txt
 * all original bodyText files are OrgXX.txt
 * all jpm bodyText files are JpmXXX.txt
 * @param type type of link
 * @return filename prefix of bodytext file
 */
string LinkFromMain::getBodyTextFilePrefix() {
  string prefix = MAIN_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = ATTACHMENT_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::JPM)
    prefix = JPM_BODYTEXT_PREFIX;
  return prefix;
}
