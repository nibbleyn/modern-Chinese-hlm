#include "link.hpp"
#include <regex>

/**
 * get chapter number and attachment number from an attachment file name
 * for example with input b001_15 would return pair <1,15>
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return pair of chapter number and attachment number
 */
AttachmentNumber getAttachmentNumber(const string &filename) {
  AttachmentNumber num(0, 0);
  string start = getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT);
  auto fileBegin = filename.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return num;
  }
  auto chapter = filename.substr(fileBegin + start.length(), 2);
  num.first = TurnToInt(chapter);
  auto seqStart = filename.find(attachmentFileMiddleChar);
  if (seqStart == string::npos) // no file to refer
  {
    return num;
  }
  auto seq = filename.substr(seqStart + 1, filename.length() - seqStart);
  num.second = TurnToInt(seq);
  return num;
}

/**
 * find in <title>xxx</title> part of attachment file header
 * the title of the attachment
 * if the file or the title is not found
 * return corresponding error message as one of below,
 * file doesn't exist.
 * title doesn't exist.
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return the title found or error message
 */
string getAttachmentTitle(const string &filename) {
  string inputFile = HTML_SRC_ATTACHMENT + filename + HTML_SUFFIX;
  ifstream infile(inputFile);
  if (!infile) {
    return "file doesn't exist.";
  }
  string inLine{""};
  string start = htmlTitleStart;
  string end = htmlTitleEnd;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto titleBegin = inLine.find(start);
    if (titleBegin != string::npos) {
      auto titleEnd = inLine.find(end);
      if (titleEnd == string::npos)
        return "title doesn't exist.";
      auto res = inLine.substr(titleBegin + start.length(),
                               titleEnd - titleBegin - start.length());
      return res;
    }
    if (inLine.find(endOfHtmlHead) != string::npos)
      return "title doesn't exist.";
  }
  return "title doesn't exist.";
}

/**
 * get all attachment files for referFile under fromDir
 * for example, if there are b003_1.html b003_5.html and b003_15.html
 * this function would return {1,5,15} for referFile "03"
 * @param referFile the 2-digit string part of refer file
 * @param fromDir where its attachment files are under
 * @return the vector of attachment numbers
 */
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir) {
  vector<string> filenameList;
  vector<int> attList;
  Poco::File(fromDir).list(filenameList);
  for (const auto &file : filenameList) {
    if (file.find(getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile) !=
        string::npos) {
      string start = attachmentFileMiddleChar;
      auto keyBegin = file.find(start);
      auto keyEnd = file.find(".");
      string attNo = file.substr(keyBegin + start.length(),
                                 keyEnd - keyBegin - start.length());
      attList.push_back(TurnToInt(attNo));
    }
  }
  return attList;
}

/**
 * there is only one template for all display types supported
 * so this function is to change it into right hidden or unhidden type
 * @param linkString the link with <a unhidden" to replace
 * @param type the display type of the link, visible or not
 * @return the replaced link accordingly
 */
void replaceDisplayLink(string &linkString, LINK_DISPLAY_TYPE type) {
  if (type == LINK_DISPLAY_TYPE::UNHIDDEN)
    return;

  string displayTag = linkStartChars + displaySpace + unhiddenDisplayProperty;
  auto displayTagBegin = linkString.find(displayTag);
  linkString.replace(displayTagBegin, displayTag.length(),
                     (type == LINK_DISPLAY_TYPE::DIRECT)
                         ? linkStartChars
                         : linkStartChars + displaySpace +
                               hiddenDisplayProperty);
}

static const string linkToSameFile =
    R"(<a unhidden title="QQ" href="#YY"><sub hidden>WW</sub>ZZ</a>)";

/**
 * generate real correct link within same file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of YY QQ WW ZZ in the original link
 * @param type display type
 * @param key the key to use in search within this same file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToSameFile;
  replaceDisplayLink(link, type);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"(title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToMainFile =
    R"(<a unhidden title="QQ" href="PPa0XX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

/**
 * generate real correct link to other main file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target main file's chapter name, e.g. 07
 * @param type display type
 * @param key the key to use in search over target main file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromMainTemplate(const string &path, const string &filename,
                               LINK_DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara) {
  string link = linkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // e.g. use top/bottom as reference name
  {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"(title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

// now only support reverse link from main back to main
static const string reverseLinkToMainFile =
    R"(<a unhidden href="a0XX.htm#YY"><sub hidden>WW</sub>被引用</a>)";

string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &citation,
                                      const string &annotation) {
  string link = reverseLinkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  replacePart(link, "WW", citation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToOriginalFile =
    R"(<a unhidden title="QQ" href="PPc0XX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

/**
 * generate real correct link to original file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromOriginalTemplate(const string &path, const string &filename,
                                   const string &key, const string &citation,
                                   const string &referPara,
                                   const string &annotation) {
  auto link = linkToOriginalFile;
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"(title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToJPMFile =
    R"(<a unhidden title="QQ" href="PPdXXX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

/**
 * generate real correct link to jpm file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XXX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromJPMTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &referPara,
                              const string &annotation) {
  auto link = linkToJPMFile;
  replacePart(link, "PP", path);
  replacePart(link, "XXX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"(title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToAttachmentFile =
    R"(<a unhidden href="PPb0XX_YY.htm">ZZ</a>)";

/**
 * generate real correct link to attachment file
 * NOTE: avoid literal strings of PP XX YY ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target attachment file's chapter name,
 * e.g. 07 of b007_3
 * @param attachNo the target attachment file's attachment index,
 * e.g. 3 of b007_3
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation) {
  auto link = linkToAttachmentFile;
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  replacePart(link, "YY", attachNo);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToImageFile =
    R"(<a unhidden title="IMAGE" href="XX#YY">（图示：ZZ）</a>)";

/**
 * Note: do not fix path now
 */
string fixLinkFromImageTemplate(const string &fullReferFilenameWithPathExt,
                                const string &picFilename,
                                const string &fullAnnotation,
                                const string &displayProperty) {
  auto link = linkToImageFile;
  if (displayProperty != unhiddenDisplayProperty) {
    if (displayProperty == hiddenDisplayProperty)
      replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    else // direct
      replacePart(link, unhiddenDisplayProperty + displaySpace, emptyString);
  }
  replacePart(link, "XX", fullReferFilenameWithPathExt);
  replacePart(link, "YY", picFilename);
  replacePart(link, "（图示：ZZ）", fullAnnotation);
  return link;
}

string Link::getWholeString() { return asString(); }

string Link::getDisplayString() {
  if (m_displayType == LINK_DISPLAY_TYPE::HIDDEN)
    return emptyString;
  else
    return m_displayText;
}

size_t Link::displaySize() { return getDisplayString().length(); }

/**
 * must ensure this is not a lineNumber string, which is a normal link also
 * before calling this method
 */
size_t Link::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, linkStartChars,
                                           linkEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  readTypeAndAnnotation(m_fullString);
  readReferFileName(m_fullString); // second step of construction
  fixFromString(m_fullString);
  if (debug >= LOG_INFO) {
    auto len = length();
    METHOD_OUTPUT << "after fix length: " << len << endl;
  }
  return containedLine.find(linkStartChars, after);
}

/**
 * generate the string of a link from its info like type, key, etc.
 * NOTE: should be consistent with template
 * @return the string of the link
 */
string Link::asString() {
  if (m_type == LINK_TYPE::IMAGE) {
    return fixLinkFromImageTemplate(m_imageReferFilename, m_referPara,
                                    m_annotation, displayPropertyAsString());
  }
  // display property
  string part0 = linkStartChars + displaySpace + displayPropertyAsString();
  if (m_displayType != LINK_DISPLAY_TYPE::DIRECT)
    part0 += displaySpace;
  // key
  if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty())
    part0 += titleStartChars + getKey() + titleEndChars + displaySpace;
  part0 += referFileMiddleChar;
  // default file name for same page
  string part1{emptyString}, part2{emptyString}, part3{emptyString};
  // refer file
  if (m_annotation == returnToContentTable) // should be LinkFromAttachment
  {
    part1 = getPathOfReferenceFile() + contentTableFilename + HTML_SUFFIX;
  } else if (m_type != LINK_TYPE::SAMEPAGE) {
    part1 =
        getPathOfReferenceFile() + getHtmlFileNamePrefix() + getChapterName();
    if (m_attachmentNumber != 0)
      part2 = attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    part3 = HTML_SUFFIX;
  }
  // refer para, not generated for attachment type of link now
  string part4{emptyString}, part5{referParaEndChar};
  if (m_type != LINK_TYPE::ATTACHMENT or m_referPara != invalidLineNumber) {
    if (m_annotation != returnToContentTable)
      part4 = referParaMiddleChar + m_referPara;
  }
  // citation, not generated for attachment type of link now
  string part7{emptyString};
  if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty()) {
    // easier to replace to <sub unhidden> if want to display this like in link
    // ir-render-able media
    part7 = citationStartChars + getReferSection() + citationEndChars;
  }
  // annotation
  string part8 = getAnnotation() + linkEndChars;
  return (part0 + part1 + part2 + part3 + part4 + part5 + part7 + part8 +
          getStringOfLinkToOrigin());
}

/**
 * get display type HIDDEN, UNHIDDEN, etc. from the link begin tag attribute.
 * and assign m_displayType member field correspondingly.
 * NOTE: key should not be these keywords
 * @param linkString the link to check
 */
void Link::readDisplayType(const string &linkString) {
  string containedPart = getIncludedStringBetweenTags(
      linkString, linkStartChars, referFileMiddleChar);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << containedPart << endl;
  if (containedPart.find(unhiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::UNHIDDEN;
  } else if (containedPart.find(hiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::HIDDEN;
  } else {
    m_displayType = LINK_DISPLAY_TYPE::DIRECT;
  }
  if (debug >= LOG_INFO) {
    if (m_displayType == LINK_DISPLAY_TYPE::DIRECT)
      METHOD_OUTPUT << "display Type: "
                    << "direct." << endl;
    else
      METHOD_OUTPUT << "display Type: " << displayPropertyAsString() << endl;
  }
}

/**
 * a link type is got from the filename it's refer to
 * if no filename appears, it is a link to the same page
 * NOTE: never called for an image type link
 * @param refereFileName the refer file name part of the link
 * @return link type
 */
LINK_TYPE getLinKTypeFromReferFileName(const string &refereFileName) {
  LINK_TYPE type = LINK_TYPE::SAMEPAGE;

  if (refereFileName.find(contentTableFilename) != string::npos or
      refereFileName.find(MAIN_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::MAIN;
  } else if (refereFileName.find(ATTACHMENT_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::ATTACHMENT;
  } else if (refereFileName.find(ORIGINAL_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::ORIGINAL;
  } else if (refereFileName.find(JPM_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::JPM;
  }
  return type;
}

/**
 * judge a link is referring to main text, attachment or original text.
 * and assign type member field correspondingly.
 * if .htm file extension is found, check prefix
 * else check title to "IMAGE"
 * else ignore file extension and viewed as link within samepage
 * @param linkString the link to check
 */
void Link::readType(const string &linkString) {
  // special link to image, use title to check
  if (linkString.find(titleStartChars + imageTypeChars + titleEndChars) !=
      string::npos) {
    m_type = LINK_TYPE::IMAGE;
    return;
  }

  m_type = LINK_TYPE::SAMEPAGE;
  if (linkString.find(HTML_SUFFIX) == string::npos) {
    if (debug >= LOG_EXCEPTION)
      METHOD_OUTPUT << "no .htm file extension found for " << linkString
                    << endl;
    return;
  }

  string refereFileName = getIncludedStringBetweenTags(
      linkString, referFileMiddleChar, HTML_SUFFIX);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "type seen from prefix: " << refereFileName << endl;
  m_type = getLinKTypeFromReferFileName(refereFileName);
}

/**
 * get the referred para like P4L2 in the target file
 * and assign referPara member field correspondingly.
 * @param linkString the link to check
 */
void Link::readReferPara(const string &linkString) {
  if (m_type == LINK_TYPE::IMAGE) {
    m_referPara = getIncludedStringBetweenTags(linkString, referParaMiddleChar,
                                               referParaEndChar);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "m_referPara: " << m_referPara << endl;
    }
    return;
  }
  string htmStart = HTML_SUFFIX + referParaMiddleChar;
  if (m_type == LINK_TYPE::SAMEPAGE)
    htmStart = referParaMiddleChar;
  if (linkString.find(htmStart) == string::npos) // no file to refer
  {
    if (debug >= LOG_EXCEPTION and m_type != LINK_TYPE::ATTACHMENT)
      METHOD_OUTPUT << "no # found to read referPara from link: " << linkString
                    << endl;
    return;
  }
  m_referPara =
      getIncludedStringBetweenTags(linkString, htmStart, titleEndChars);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "referPara: " << m_referPara << endl;
}

/**
 * annotation is the last part of a link normally, represents the meaning of the
 * link. get this value and assign annotation member field correspondingly.
 * @param linkString the link for analysis
 * @return true if there is target specified
 */
bool Link::readAnnotation(const string &linkString) {
  string htmStart = citationEndChars;
  if (linkString.find(htmStart) == string::npos) {
    // deprecated key end tag
    htmStart = keyEndChars;
    if (linkString.find(htmStart) == string::npos) {
      htmStart = referParaEndChar;
    }
  }
  m_annotation =
      getIncludedStringBetweenTags(linkString, htmStart, linkEndChars);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "annotation: " << m_annotation << endl;
  return true;
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
void Link::readKey(const string &linkString) {
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
      m_needChange = true;
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
  bodyText.setFileAndAttachmentNumber(getChapterName(), m_attachmentNumber);
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
    string unitString = defaultUnit;
    if (m_attachmentNumber != 0)
      unitString = attachmentFileMiddleChar + TurnToString(m_attachmentNumber) +
                   attachmentUnit;
    string expectedSection = citationChapterNo + TurnToString(m_chapterNumber) +
                             unitString + TurnToString(ln.getParaNumber()) +
                             citationChapterParaSeparator +
                             TurnToString(ln.getlineNumber()) + citationPara;
    // will set needChange if found line is different
    fixReferPara(ln.asString());
    fixReferSection(expectedSection);
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "key: " << m_usedKey << endl;
}

string scanForSubComments(const string &original, const string &fromFile) {
  string result;
  using SubStringOffsetTable =
      std::map<size_t, size_t>; // start offset -> end offset
  SubStringOffsetTable subStrings;
  string startTag = commentBeginChars;
  string endTag = commentEndChars;
  auto offset = original.find(startTag);
  do {
    if (offset == string::npos)
      break;
    subStrings[offset] = original.find(endTag, offset);
    offset = original.find(startTag, offset + 1);
  } while (true);
  // concatenate each comment's displayString with rest texts
  auto endOfSubStringOffset = 0;
  for (const auto &comment : subStrings) {
    result += original.substr(endOfSubStringOffset,
                              comment.first - endOfSubStringOffset);
    auto current = std::make_unique<Comment>(fromFile);
    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset = comment.second + endTag.length();
  }
  result += original.substr(endOfSubStringOffset);

  return result;
}

/*
 * the final step of construction
 * assuming readTypeAndAnnotation and readReferFileName()
 * was called before, refer to Link class definition
 */
void Link::fixFromString(const string &linkString) {
  readDisplayType(linkString);
  readReferPara(linkString);
  // no need for key for these links
  if (m_annotation != returnLinkFromAttachmentHeader and
      m_annotation != returnLink and m_annotation != returnToContentTable)
    readKey(linkString); // key would be searched here and replaced
  m_bodyText = m_annotation;
  m_displayText = scanForSubComments(m_bodyText, m_fromFile);
}
