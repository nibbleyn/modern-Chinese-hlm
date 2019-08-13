#include "coupledLink.hpp"

string scanForSubObjects(const string &original, const string &fromFile,
                         bool forLink) {
  string result;
  // start offset -> end offset
  using SubStringOffsetTable = map<size_t, size_t>;
  SubStringOffsetTable subStrings;
  ObjectPtr current = nullptr;
  if (forLink)
    current = make_unique<LinkFromMain>(fromFile);
  else
    current = make_unique<Comment>(fromFile);
  string startTag = current->getStartTag();
  string endTag = current->getEndTag();
  auto offset = original.find(startTag);
  do {
    if (offset == string::npos)
      break;
    subStrings[offset] = original.find(endTag, offset);
    offset = original.find(startTag, offset + 1);
  } while (true);
  auto endOfSubStringOffset = 0;
  for (const auto &subString : subStrings) {
    result += original.substr(endOfSubStringOffset,
                              subString.first - endOfSubStringOffset);
    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset = subString.second + endTag.length();
  }
  result += original.substr(endOfSubStringOffset);
  return result;
}

string CoupledLink::getFormatedFullString() { return asString(); }

/**
 * must ensure this is not a lineNumber string, which is a normal link also
 * before calling this method
 */
size_t CoupledLink::loadFirstFromContainedLine(const string &containedLine,
                                               size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  // skip lineNumber link
  if (m_fullString.find(LineNumberIdentity) != string::npos)
    pos = getFullStringAndBodyTextFromContainedLine(containedLine, pos + 1);
  if (pos != string::npos) {
    readTypeAndAnnotation(m_fullString);
    readReferFileName(m_fullString); // second step of construction
    fixFromString(m_fullString);
    if (debug >= LOG_INFO) {
      auto len = length();
      METHOD_OUTPUT << "after fix length: " << len << endl;
    }
  }
  return pos;
}

/**
 * generate the string of a link from its info like type, key, etc.
 * NOTE: should be consistent with template
 * @return the string of the link
 */
string CoupledLink::asString(bool ignoreOriginalPart) {
  if (m_type == LINK_TYPE::IMAGE) {
    if (m_imageReferFilename.empty())
      m_imageReferFilename = getHtmlFileNamePrefix() + m_fromFile + HTML_SUFFIX;
    return fixLinkFromImageTemplate(m_imageReferFilename, m_referPara,
                                    m_annotation, displayPropertyAsString());
  }
  // display property
  string part0 = linkStartChars + displaySpace + displayPropertyAsString();
  if (m_displayType != DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  // key
  if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty())
    part0 += titleStartChars + m_usedKey + titleEndChars + displaySpace;
  part0 += referFileMiddleChar;
  // default file name for same page
  string part1{emptyString}, part2{emptyString}, part3{emptyString};
  // refer file
  if (m_annotation == returnToContentTable) // should be LinkFromAttachment
  {
    part1 = getPathOfReferenceFile() + contentTableFilename + HTML_SUFFIX;
  } else {
    part1 =
        getPathOfReferenceFile() + getHtmlFileNamePrefix() + getChapterName();
    if (m_attachmentNumber != 0)
      part2 = attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    part3 = HTML_SUFFIX;
  }
  // refer para, not generated for attachment type of link now
  string part4, part5 = referParaEndChar;
  if (m_type != LINK_TYPE::ATTACHMENT or m_referPara != invalidLineNumber) {
    if (m_annotation != returnToContentTable)
      part4 = referParaMiddleChar + m_referPara;
  }
  // citation, not generated for attachment type of link now
  string part7{emptyString};
  if (isReverseLink()) {
    part7 = downArrow;
    if (m_referSection.isValid())
      part7 += m_referSection.getFormatedFullString();
  } else {
    if (isTargetToOtherMainHtm() or isTargetToOriginalHtm() or
        isTargetToJPMHtm())
      part7 = upArrow;
    if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty())
      part7 += m_referSection.getFormatedFullString();
  }
  // annotation
  string part8 = getAnnotation() + linkEndChars;
  if (not ignoreOriginalPart)
    part8 += getEnclosedStringOfLinkToOrigin();
  return (part0 + part1 + part2 + part3 + part4 + part5 + part7 + part8);
}

/**
 * key is QQ included within title="QQ" part of link.
 * Be part of title make it a hint also when hovering over this link in browser.
 * it is one word could be found in the target file,
 * thru which its embedded line could be found.
 * that line's line number would be then used to update referPara.
 * needChange member field would also get update if
 * 1. referPara is different before and after finding;
 * 2. key itself needs update like changed to "KeyNotFound".
 * so if whole title="QQ" is missing, KeyNotFound would be returned
 * and search would not start.
 * else, if the key included in this tag cannot be found in target file,
 * KeyNotFound + key would be returned means manual fix of key to use is also
 * needed. so if key is still in "KeyNotFound+key" format, which means manual
 * fix is not done, simply return and do nothing.
 * NOTE: no key is used for image type link,
 * instead image file name would be
 * both the id of image and referPara of the link.
 * key must be kept as IMAGE as its type indicator.
 * @param linkString the link for analysis
 */
void CoupledLink::readKey(const string &linkString) {
  if (m_type == LINK_TYPE::ATTACHMENT)
    return;
  if (m_type == LINK_TYPE::IMAGE) {
    m_usedKey = imageTypeChars;
    return;
  }

  if (linkString.find(titleStartChars) == string::npos) {
    // not a link to top/bottom and no key was found
    // add a "KeyNotFound" key and return
    if (m_referPara != topParagraphIndicator and
        m_referPara != bottomParagraphIndicator) {
      needToChange();
      m_usedKey = keyNotFound;
    }
    if (debug >= LOG_EXCEPTION) {
      METHOD_OUTPUT << titleStartChars + " not found." << endl;
    }
    return;
  }

  string stringForSearch =
      getIncludedStringBetweenTags(linkString, titleStartChars, titleEndChars);
  // still need manual fix of this key, so abort
  if (stringForSearch.find(keyNotFound) != string::npos) {
    m_needChange = false;
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "no need to change" << endl;
    m_usedKey = stringForSearch;
    return;
  }

  // same page link would refer to different bodyText
  CoupledBodyText bodyText(getBodyTextFilePrefix());
  bodyText.resetBeforeSearch();
  // special hack to ignore itself
  if (isTargetToSelfHtm()) {
    bodyText.addIgnoreLines(m_fromLine.asString());
  }
  bodyText.setFileAndAttachmentNumber(m_chapterNumber, m_attachmentNumber);
  if (bodyText.isMainBodyText())
    bodyText.ignorePersonalComments();
  bool found = bodyText.findKey(stringForSearch);
  if (not found) {
    m_usedKey = keyNotFound + stringForSearch;
    fixReferPara(changeKey); // will set needChange if found line is different
  } else {
    m_usedKey = stringForSearch;
    // only use the first line found
    string lineNumber = bodyText.getFirstResultLine();
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "line number found: " << lineNumber << endl;
    LineNumber ln(lineNumber);
    AttachmentNumber num = make_pair(m_chapterNumber, m_attachmentNumber);
    ParaLineNumber paraLine = ln.getParaLineNumber();
    // will set needChange if found line is different
    fixReferPara(ln.asString());
    fixReferSection(num, paraLine);
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "key: " << m_usedKey << endl;
}

/*
 * the final step of construction
 * assuming readTypeAndAnnotation and readReferFileName()
 * was called before, refer to Link class definition
 */
void CoupledLink::fixFromString(const string &linkString) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << linkString << endl;
  m_fullString = linkString;
  readReferPara(linkString);
  fixReferSection(linkString);
  // no need for key for these links
  if (m_annotation != returnLinkFromAttachmentHeader and
      m_annotation != returnLink and m_annotation != returnToContentTable)
    readKey(linkString); // key would be searched here and replaced,
                         // m_needChange updated
  m_bodyText = m_annotation;
  if (m_type == LINK_TYPE::IMAGE and m_imageReferFilename.empty())
    needToChange();
  if (isReverseLink())
    m_displayText = downArrow;
  else if (isTargetToImage() or isTargetToOtherMainHtm() or
           isTargetToOriginalHtm() or isTargetToJPMHtm())
    m_displayText = upArrow;
  m_displayText += m_referSection.getDisplayString();
  m_displayText += scanForSubObjects(m_bodyText, m_fromFile, false);
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
bool CoupledLink::readReferFileName(const string &linkString) {
  m_chapterNumber = 0;
  m_attachmentNumber = 0;
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
    // not find a right file type to refer
    if (fileBegin == string::npos) {
      METHOD_OUTPUT << "unsupported type in refer file name in link: "
                    << refereFileName << endl;
      return false;
    }
    refereFileName = refereFileName.substr(fileBegin);
    refereFileName = refereFileName.substr(getHtmlFileNamePrefix().length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (isTargetToAttachmentFile()) {
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
