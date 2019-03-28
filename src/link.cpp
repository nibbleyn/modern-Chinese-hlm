#include "link.hpp"
#include <regex>

extern fileSet keyMissingChapters;
extern fileSet newAttachmentList;

Link::LinksTable Link::linksTable;
string Link::outPutFilePath{emptyString};
Link::AttachmentSet Link::refAttachmentTable;

LinkFromMain::AttachmentSet LinkFromMain::attachmentTable;

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
    R"(<a unhidden title="IMAGE" href="#XX">（图示：ZZ）</a>)";

string fixLinkFromImageTemplate(const string &filename,
                                const string &fullAnnotation,
                                const string &displayProperty) {
  auto link = linkToImageFile;
  if (displayProperty != unhiddenDisplayProperty) {
    if (displayProperty == hiddenDisplayProperty)
      replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    else // direct
      replacePart(link, unhiddenDisplayProperty + displaySpace, emptyString);
  }
  replacePart(link, "XX", filename);
  replacePart(link, "（图示：ZZ）", fullAnnotation);
  return link;
}

/**
 * output linksTable to file HTML_OUTPUT_LINKS_LIST
 */
void Link::displayFixedLinks() {
  if (linksTable.empty())
    return;
  FUNCTION_OUTPUT << outPutFilePath << " is created." << endl;
  ofstream outfile(outPutFilePath);
  outfile << "all links processed:" << endl;
  outfile << "---------------------------------" << endl;
  for (const auto &link : linksTable) {
    auto target = link.first;
    auto fromList = link.second;
    // link itself
    outfile << "link:" << target.first << "," << target.second << endl;
    sort(fromList.begin(), fromList.end(),
         [](LinkDetails a, LinkDetails b) { return a.key < b.key; });
    for (const auto &from : fromList) {
      outfile << "key: " << from.key << ",linked from:" << from.fromFile << ","
              << from.fromLine << ":" << endl
              << "    " << from.link << endl;
    }
  }
}

/**
 * personalAttachmentType means a personal review could be removed thru
 * removePersonalViewpoints() referenceAttachmentType means reference to other
 * stories or about a person
 */
static const string personalAttachmentType = R"(1)";
static const string referenceAttachmentType = R"(0)";

/**
 * the string to write into file of a type
 * @param type ATTACHMENT_TYPE to convert
 * @return corresponding string
 */
string attachmentTypeAsString(ATTACHMENT_TYPE type) {
  return (type == ATTACHMENT_TYPE::PERSONAL) ? personalAttachmentType
                                             : referenceAttachmentType;
}

/**
 * convert string read from file back to type
 * @param str referenceAttachmentType or personalAttachmentType from file
 * @return REFERENCE or PERSONAL
 */
ATTACHMENT_TYPE attachmentTypeFromString(const string &str) {
  return (str == personalAttachmentType) ? ATTACHMENT_TYPE::PERSONAL
                                         : ATTACHMENT_TYPE::REFERENCE;
}

/**
 * find the type of one attachment from refAttachmentTable
 * which is loaded from loadReferenceAttachmentList()
 * @param num pair of chapter number and attachment number
 * @return personalAttachmentType if found in refAttachmentTable with value
 * personalAttachmentType otherwise, return referenceAttachmentType if founded
 * with value referenceAttachmentType otherwise, return "2"
 *  personalAttachmentType means a personal review could be removed thru
 * removePersonalViewpoints() referenceAttachmentType means reference to other
 * stories or about a person
 */
ATTACHMENT_TYPE Link::getAttachmentType(AttachmentNumber num) {
  ATTACHMENT_TYPE attachmentType = ATTACHMENT_TYPE::NON_EXISTED;
  try {
    auto entry = refAttachmentTable.at(num).second;
    attachmentType = GetTupleElement(entry, 2);
  } catch (exception &) {
    if (debug >= LOG_EXCEPTION)
      FUNCTION_OUTPUT << "not found info in refAttachmentTable about: "
                      << num.first << attachmentFileMiddleChar << num.second
                      << endl;
  }
  return attachmentType;
}

static const string HTML_SRC_REF_ATTACHMENT_LIST =
    "utf8HTML/src/RefAttachments.txt";

/**
 * load refAttachmentTable from HTML_SRC_REF_ATTACHMENT
 * read file and add entry of attachment found before into target list.
 * by using refAttachmentTable and attachmentList and doing below iteratively
 * we can keep old changes of type and only need to change added links
 * 1. loading from refAttachmentTable,
 * 2. output to attachmentList during link fixing
 * 3. manually changing type in the output attachmentList
 * 4. overwriting refAttachmentTable by this output attachmentList
 */
void Link::loadReferenceAttachmentList() {
  ifstream infile(HTML_SRC_REF_ATTACHMENT_LIST);
  if (!infile) {
    FUNCTION_OUTPUT << "file doesn't exist:" << HTML_SRC_REF_ATTACHMENT_LIST
                    << endl;
    return;
  }
  refAttachmentTable.clear();
  string line;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    getline(infile, line); // Saves the line
    if (line.empty())
      break;

    string start = "type:";
    string type = referenceAttachmentType;
    auto typeBegin = line.find(start);
    if (typeBegin != string::npos) {
      type = line.substr(typeBegin + start.length());
    }
    start = "title:";
    auto titleBegin = line.find(start);
    string title = line.substr(titleBegin + start.length(),
                               typeBegin - titleBegin - start.length());
    start = "name:";
    auto nameBegin = line.find(start);
    string targetFile = line.substr(nameBegin + start.length(),
                                    titleBegin - nameBegin - start.length());

    start = "from:";
    auto fromBegin = line.find(start);
    string fromLine = line.substr(fromBegin + start.length(),
                                  nameBegin - fromBegin - start.length());
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << "from:" << fromLine << " name:" << targetFile
                      << " title:" << title << " type:" << type << endl;
    fromLine.erase(remove(fromLine.begin(), fromLine.end(), ' '),
                   fromLine.end());
    targetFile.erase(remove(targetFile.begin(), targetFile.end(), ' '),
                     targetFile.end());
    type.erase(remove(type.begin(), type.end(), ' '), type.end());
    // remove only leading and trailing blank for title
    title = std::regex_replace(title, std::regex("^ +"), emptyString);
    title = std::regex_replace(title, std::regex(" +$"), emptyString);
    refAttachmentTable[getAttachmentNumber(targetFile)] =
        make_pair(fromLine, make_tuple(targetFile, title,
                                       attachmentTypeFromString(type)));
  }
}
/**
 * reset the general data structure to log info about links
 */
void Link::resetStatisticsAndLoadReferenceAttachmentList() {
  linksTable.clear();
  loadReferenceAttachmentList();
}

/**
 * output links to specified file
 * before calling this function, specifying output file
 */
void Link::outPutStatisticsToFiles() {
  displayFixedLinks();
  FUNCTION_OUTPUT << "links information are written into: " << outPutFilePath
                  << endl;
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
    return fixLinkFromImageTemplate(m_imageFilename, m_annotation,
                                    displayPropertyAsString());
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
  m_type = LINK_TYPE::SAMEPAGE;
  // special image file without .htm suffix, use title to check
  if (linkString.find(HTML_SUFFIX) == string::npos) {
    if (linkString.find(titleStartChars + imageTypeChars + titleEndChars) !=
        string::npos) {
      m_type = LINK_TYPE::IMAGE;
    }
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

/**
 * record this file as one who has links of wrong/un-found key
 */
void Link::recordMissingKeyLink() { keyMissingChapters.insert(m_fromFile); }

/**
 * check whether the annotation of a link to attachment
 * matches the attachment file's tile
 * if the attachment file doesn't exist or have a title, skip this check
 * @param annotation of a link to attachment
 * @param title of the attachment file
 * @return true if matched
 */
bool isAnnotationMatch(string annotation, string title) {
  if (title == "file doesn't exist." or title == "title doesn't exist.")
    return true;
  return (annotation == title);
}

static const string HTML_OUTPUT_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/LinksFromMain.txt";
static const string HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST =
    "utf8HTML/output/AttachmentsFromMain.txt";

/**
 * output all attachment info into specified file
 */
void LinkFromMain::displayAttachments() {
  if (attachmentTable.empty())
    return;
  ofstream outfile(HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST);
  for (const auto &attachment : attachmentTable) {
    AttachmentFileNameTitleAndType entry = attachment.second.second;

    outfile << "from:" << attachment.second.first
            << " name:" << GetTupleElement(entry, 0)
            << " title:" << GetTupleElement(entry, 1)
            << " type:" << attachmentTypeAsString(GetTupleElement(entry, 2))
            << endl;
  }
  FUNCTION_OUTPUT << "attachment information are written into: "
                  << HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST << endl;
}

/**
 * find fromLine of one attachment from attachmentTable
 * which is calculated during link fixing
 * @param num pair of chapter number and attachment number
 * @return the fromLine stored if existed, otherwise "top"
 */
string LinkFromMain::getFromLineOfAttachment(AttachmentNumber num) {
  string result = topParagraphIndicator;
  try {
    result = attachmentTable.at(num).first;
  } catch (exception &) {
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << "fromLine not found in attachmentTable about: "
                      << num.first << attachmentFileMiddleChar << num.second
                      << endl;
  }
  return result;
}

/**
 * reset statistics data structure for re-do it during link fixing
 */
void LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList() {
  Link::resetStatisticsAndLoadReferenceAttachmentList();
  attachmentTable.clear();
}

/**
 * output statistics from link fixing of links from main files
 */
void LinkFromMain::outPutStatisticsToFiles() {
  outPutFilePath = HTML_OUTPUT_LINKS_FROM_MAIN_LIST;
  Link::outPutStatisticsToFiles();
  displayAttachments();
}

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
    m_imageFilename = getIncludedStringBetweenTags(
        linkString, referParaMiddleChar, referParaEndChar);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "m_imageFilename: " << m_imageFilename << endl;
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
void LinkFromMain::logLink() {

  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{m_usedKey, m_fromFile, m_fromLine.asString(),
                       asString()};
    try {
      auto &entry =
          linksTable.at(std::make_pair(getChapterName(), m_referPara));
      entry.push_back(detail);
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "entry.size: " << entry.size()
                      << " more reference to link: " << getChapterName()
                      << displaySpace << m_referPara << displaySpace
                      << m_usedKey << endl;
    } catch (exception &) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "create vector for : " << getChapterName()
                      << displaySpace << m_referPara << endl;
      vector<LinkDetails> list;
      list.push_back(detail);
      linksTable[std::make_pair(getChapterName(), m_referPara)] = list;
    }
  }
  if (isTargetToOtherAttachmentHtm()) {
    auto targetFile = getHtmlFileNamePrefix() + getChapterName() +
                      attachmentFileMiddleChar +
                      TurnToString(getattachmentNumber());
    auto num = make_pair(getchapterNumer(), getattachmentNumber());
    auto title = getAttachmentTitle(targetFile);
    auto type = getAttachmentType(num);
    if (getSourceChapterName() == getChapterName()) {
      if (type == ATTACHMENT_TYPE::NON_EXISTED)
        newAttachmentList.insert(targetFile);
      attachmentTable[num] = make_pair(
          m_fromLine.asString(), make_tuple(targetFile, title,
                                            (type == ATTACHMENT_TYPE::PERSONAL)
                                                ? ATTACHMENT_TYPE::PERSONAL
                                                : ATTACHMENT_TYPE::REFERENCE));
    }
    if (not isAnnotationMatch(getAnnotation(), title)) {
      METHOD_OUTPUT << m_fromFile << " has a link to " << targetFile
                    << " with annotation: " << getAnnotation()
                    << " differs from title: " << title << endl;
    }
  }
}

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

static const string HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/LinksFromAttachment.txt";
static const string HTML_OUTPUT_ATTACHMENT_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/AttachmentsFromAttachment.txt";

/**
 * reset statistics data structure for re-do it during link fixing
 */
void LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList() {
  Link::resetStatisticsAndLoadReferenceAttachmentList();
}

/**
 * output statistics from link fixing of links from attachment files
 */
void LinkFromAttachment::outPutStatisticsToFiles() {
  outPutFilePath = HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST;
  Link::outPutStatisticsToFiles();
}

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
    m_imageFilename = getIncludedStringBetweenTags(
        linkString, referParaMiddleChar, referParaEndChar);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "m_imageFilename: " << m_imageFilename << endl;
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
 * log a valid link found by putting it into linksTable
 * which is sorted by ChapterName+attachmentNumber, referPara, usedKey
 */
void LinkFromAttachment::logLink() {
  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{m_usedKey, m_fromFile, m_fromLine.asString(),
                       asString()};
    try {
      auto &entry = linksTable.at(
          std::make_pair(getChapterName() + attachmentFileMiddleChar +
                             TurnToString(m_attachmentNumber),
                         m_referPara));
      entry.push_back(detail);
    } catch (exception &) {
      if (debug >= LOG_EXCEPTION)
        METHOD_OUTPUT << "not found link for: "
                      << getChapterName() + attachmentFileMiddleChar +
                             TurnToString(m_attachmentNumber)
                      << displaySpace << m_referPara << displaySpace
                      << m_usedKey << endl;
      vector<LinkDetails> list;
      list.push_back(detail);
      linksTable[std::make_pair(getChapterName() + attachmentFileMiddleChar +
                                    TurnToString(m_attachmentNumber),
                                m_referPara)] = list;
    }
  }
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
