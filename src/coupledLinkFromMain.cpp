#include "coupledLink.hpp"

/**
 * generate the link to original file after a link to main file from main
 */
void LinkFromMain::generateLinkToOrigin() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "create link to original main html thru key: " << m_usedKey
                  << endl;
  auto reservedType = m_type;   // only LINK_TYPE::MAIN has origin member
  m_type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to =
      fixLinkFromOriginalTemplate(getPathOfReferenceFile(), getChapterName(),
                                  m_usedKey, emptyString, m_referPara);
  m_linkPtrToOrigin = std::make_unique<LinkFromMain>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  m_needChange = true;
  m_type = reservedType;
  m_displayText += bracketStartChars + annotationToOriginal + bracketEndChars;
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

/**
 * get the chapter number of target file of the link
 * and its attachment number, if this is a link to an attachment,
 * and assign chapterNumber and attachmentNumber member field correspondingly.
 * for example, chapter number would be 18
 * and attachment number would be 7 for below link,
 * <a unhidden href="b018_7.htm">happy</a>
 * if this is a LINK_TYPE::SAMEPAGE link to its embedded file,
 * assign chapter as its embedded file's.
 * @param linkString the link to check
 * @return true only if a right chapter number and attachment number are gotten
 */
bool LinkFromMain::readReferFileName(const string &linkString) {
  if (m_type == LINK_TYPE::IMAGE) {
    m_imageReferFilename = getIncludedStringBetweenTags(
        linkString, referFileMiddleChar, referParaMiddleChar);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "m_imageReferFilename: " << m_imageReferFilename << endl;
    }
    return true;
  }

  string refereFileName = m_fromFile;
  if (m_type != LINK_TYPE::SAMEPAGE) {
    refereFileName = getIncludedStringBetweenTags(
        linkString, referFileMiddleChar, HTML_SUFFIX);
    // in case there is a ..\ before file name
    auto fileBegin = refereFileName.find(getHtmlFileNamePrefix());
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      METHOD_OUTPUT << "unsupported type in refer file name in link: "
                    << refereFileName << endl;
      return false;
    }
    refereFileName = refereFileName.substr(fileBegin);
    refereFileName = refereFileName.substr(getHtmlFileNamePrefix().length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (m_type == LINK_TYPE::ATTACHMENT) {
    auto attachmentNumberStart = refereFileName.find(attachmentFileMiddleChar);
    if (attachmentNumberStart == string::npos) {
      METHOD_OUTPUT << "no attachment number in link: " << linkString << endl;
      return false;
    }
    m_attachmentNumber =
        TurnToInt(refereFileName.substr(attachmentNumberStart + 1));
    m_chapterNumber =
        TurnToInt(refereFileName.substr(0, attachmentNumberStart));
  } else {
    m_chapterNumber = TurnToInt(refereFileName);
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "chapterNumber: " << m_chapterNumber
                  << ", attachmentNumber: " << m_attachmentNumber << endl;
  return true;
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
