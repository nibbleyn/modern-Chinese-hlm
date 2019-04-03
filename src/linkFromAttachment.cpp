#include "link.hpp"

/**
 * generate the link to original file after a link to main file from attachment
 */
void LinkFromAttachment::generateLinkToOrigin() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "create link to original main html thru key: " << m_usedKey
                  << endl;
  auto reservedType = m_type;
  m_type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to =
      fixLinkFromOriginalTemplate(getPathOfReferenceFile(), getChapterName(),
                                  m_usedKey, emptyString, m_referPara);
  m_linkPtrToOrigin = std::make_unique<LinkFromAttachment>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  m_needChange = true;
  m_type = reservedType;
  m_displayText += bracketStartChars + annotationToOriginal + bracketEndChars;
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

/**
 * get the chapter number of target file of the link
 * and its attachment number,
 * if this is a link to an attachment or a link to its embedded file.
 * and assign chapterNumber and attachmentNumber member field correspondingly.
 * if this is a LINK_TYPE::SAMEPAGE link to its embedded file,
 * get chapter and attachment number as its embedded file's.
 * for example, chapter number would be 18
 * and attachment number would be 7 for below link from b003_6.htm,
 * <a unhidden href="b018_7.htm">happy</a>
 * or a link below from within b018_7.htm
 * <a hidden href="#top">happy</a>
 * @param linkString the link to check
 * @return true only if a right chapter number and attachment number are gotten
 */
bool LinkFromAttachment::readReferFileName(const string &linkString) {
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
  // or LINK_TYPE::SAMEPAGE
  if (m_type == LINK_TYPE::SAMEPAGE or m_type == LINK_TYPE::ATTACHMENT) {
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
 * change target type thru specifying a type prefix
 * @param prefix type prefix
 */
void LinkFromAttachment::setTypeThruFileNamePrefix(const string &prefix) {
  if (prefix == "main")
    m_type = LINK_TYPE::MAIN;
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
