#include "link.hpp"
#include <regex>

extern fileSet keyMissingChapters;
extern fileSet newAttachmentList;

Link::LinksTable Link::linksTable;
string Link::outPutFilePath{""};
Link::AttachmentSet Link::refAttachmentTable;

LinkFromMain::AttachmentSet LinkFromMain::attachmentTable;

/**
 * a link type is got from the filename it's refer to
 * if no filename appears, it is a link to the same page
 * @param refereFileName the refer file name part of the link
 * @return link type
 */
LINK_TYPE getLinKTypeFromReferFileName(const string &refereFileName) {
  LINK_TYPE type = LINK_TYPE::SAMEPAGE;

  if (refereFileName.find(MAIN_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::MAIN;
  } else if (refereFileName.find(ATTACHMENT_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::ATTACHMENT;
  } else if (refereFileName.find(ORIGINAL_HTML_PREFIX) != string::npos) {
    type = LINK_TYPE::ORIGINAL;
  }
  return type;
}

/**
 * there is only one template for all display types supported
 * so this function is to change it into right hidden or unhidden type
 * @param linkString the link with <a unhidden" to replace
 * @param type the display type of the link, visible or not
 * @return the replaced link accordingly
 */
string replaceDisplayLink(string &linkString, LINK_DISPLAY_TYPE type) {
  if (type == LINK_DISPLAY_TYPE::UNHIDDEN)
    return linkString;

  string displayTag = "a unhidden href";
  auto displayTagBegin = linkString.find(displayTag);
  linkString.replace(displayTagBegin, displayTag.length(),
                     (type == LINK_DISPLAY_TYPE::DIRECT) ? "a href"
                                                         : "a hidden href");
  return linkString;
}

static const string linkToSameFile =
    R"(<a unhidden href="#YY" title="QQ"><i hidden>QQ</i>ZZ</a>)";
/**
 * generate real correct link within same file
 * by filling right "refer para" based on key searching
 * avoid literal strings of YY QQ ZZ appear in the original link
 * @param type display type
 * @param key the key to use in search within this same file
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToSameFile;
  link = replaceDisplayLink(link, type);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", "");
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<i hidden>QQ</i>)", key);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "ZZ", annotation);
  return link;
}

static const string linkToMainFile =
    R"(<a unhidden href="PPa0XX.htm#YY" title="QQ"><i hidden>QQ</i><sub unhidden>WW</sub>ZZ</a>)";
/**
 * generate real correct link to other main file
 * by filling right "refer para" based on key searching
 * avoid literal strings of PP XX YY QQ WW ZZ appear in the original link
 * to get a unexpected replacement
 * @param path relative path based on the link type, refer to
 * getPathOfReferenceFile()
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
  link = replaceDisplayLink(link, type);
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", "");
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<i hidden>QQ</i><sub unhidden>WW</sub>)", key);
    replacePart(link, R"(title="QQ")", "");
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  return link;
}

// now only support reverse link from main back to main
static const string reverseLinkToMainFile =
    R"(<a unhidden href="a0XX.htm#YY">被引用</a>)";

string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &annotation) {

  string link = reverseLinkToMainFile;
  link = replaceDisplayLink(link, type);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", "");
  } else
    replacePart(link, "YY", referPara);
  return link;
}

static const string linkToOriginalFile =
    R"(<a unhidden href="PPc0XX.htm#YY" title="QQ"><i hidden>QQ</i><sub unhidden>WW</sub>ZZ</a>)";
/**
 * generate real correct link to original file
 * by filling right "refer para" based on key searching
 * avoid literal strings of PP XX YY QQ appear in the original link
 * to get a unexpected replacement
 * @param path relative path based on the link type, refer to
 * getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param referPara the target place identified by line number
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
    replacePart(link, R"(#YY)", "");
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<i hidden>QQ</i><sub unhidden>WW</sub>)", key);
    replacePart(link, R"(title="QQ")", "");
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  return link;
}

static const string linkToJPMFile =
    R"(<a unhidden href="PPdXXX.htm#YY" title="QQ"><i hidden>QQ</i><sub unhidden>WW</sub>ZZ</a>)";
/**
 * generate real correct link to original file
 * by filling right "refer para" based on key searching
 * avoid literal strings of PP XX YY QQ appear in the original link
 * to get a unexpected replacement
 * @param path relative path based on the link type, refer to
 * getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param referPara the target place identified by line number
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
    replacePart(link, R"(#YY)", "");
  } else
    replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    replacePart(link, R"(<i hidden>QQ</i><sub unhidden>WW</sub>)", key);
    replacePart(link, R"(title="QQ")", "");
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  return link;
}

static const string linkToAttachmentFile =
    R"(<a unhidden href="PPb0XX_YY.htm">ZZ</a>)";
/**
 * generate real correct link to attachment file
 * avoid literal strings of PP XX YY ZZ appear in the original link
 * to get a unexpected replacement
 * @param path relative path based on the link type, refer to
 * getPathOfReferenceFile()
 * @param filename the target attachment file's chapter name, e.g. 07 of b007_3
 * @param attachNo the target attachment file's attachment index, e.g. 3 of
 * b007_3
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
  return link;
}

/**
 * output linksTable to file HTML_OUTPUT_LINKS_LIST
 */
void Link::displayFixedLinks() {
  if (linksTable.empty())
    return;
  cout << outPutFilePath << " is created." << endl;
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
      cout << "not found info in refAttachmentTable about: " << num.first
           << attachmentFileMiddleChar << num.second << endl;
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
    cout << "file doesn't exist:" << HTML_SRC_REF_ATTACHMENT_LIST << endl;
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
      cout << "from:" << fromLine << " name:" << targetFile
           << " title:" << title << " type:" << type << endl;
    fromLine.erase(remove(fromLine.begin(), fromLine.end(), ' '),
                   fromLine.end());
    targetFile.erase(remove(targetFile.begin(), targetFile.end(), ' '),
                     targetFile.end());
    type.erase(remove(type.begin(), type.end(), ' '), type.end());
    // remove only leading and trailing blank for title
    title = std::regex_replace(title, std::regex("^ +"), "");
    title = std::regex_replace(title, std::regex(" +$"), "");
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
  cout << "links information are written into: " << outPutFilePath << endl;
}

string Link::getWholeString() { return asString(); }
string Link::getDisplayString() {
  if (m_displayType == LINK_DISPLAY_TYPE::HIDDEN)
    return "";
  else
    return m_displayText;
}

size_t Link::length() { return getWholeString().length(); }
size_t Link::displaySize() { return getDisplayString().length(); }

/**
 * generated the string of a link from its info like type, key, etc.
 * @return the string of the link
 */
string Link::asString() {
  string part0 = linkStartChars + " " + displayPropertyAsString();
  if (m_displayType != LINK_DISPLAY_TYPE::DIRECT)
    part0 += " ";
  if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty())
    part0 += titleStartChars + getKey() + titleEndChars;
  part0 += referFileMiddleChar;
  string part1{""}, part2{""}, part3{""};
  if (m_annotation == returnToContentTable) // type would be SAMEPAGE
  {
    part1 = getPathOfReferenceFile() + contentTableFilename;
  } else if (m_type != LINK_TYPE::SAMEPAGE) {
    part1 =
        getPathOfReferenceFile() + getHtmlFileNamePrefix() + getChapterName();
    if (m_attachmentNumber != 0)
      part2 = attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    part3 = HTML_SUFFIX;
  }
  string part4{""};
  if (m_type != LINK_TYPE::ATTACHMENT or m_referPara != invalidLineNumber) {
    if (m_annotation != returnToContentTable)
      part4 = referParaMiddleChar + m_referPara;
  }
  string part5 = referParaEndChar, part6{""}, part7{""};
  if (m_type != LINK_TYPE::ATTACHMENT and not m_usedKey.empty()) {
    part6 = keyStartChars + getKey() + keyEndChars;
    // easier to replace to <sub unhidden> if want to display this
    if (m_type == LINK_TYPE::MAIN or m_type == LINK_TYPE::ORIGINAL)
      part7 = citationStartChars + getReferSection() + citationEndChars;
  }
  string part8 = getAnnotation() + linkEndChars;
  return (part0 + part1 + part2 + part3 + part4 + part5 + part6 + part7 +
          part8 + getStringOfLinkToOrigin());
}

/**
 * get display type HIDDEN, UNHIDDEN, etc. from the link <a> tag attribute.
 * for example, HIDDEN would be returned for below link,
 * <a hidden href="a080.htm#P0L0">
 * and assign displayType member field correspondingly.
 * @param linkString the link to check
 */
void Link::readDisplayType(const string &linkString) {
  auto hrefStart = linkString.find(referFileMiddleChar);
  string containedPart = linkString.substr(0, hrefStart);
  if (debug >= LOG_INFO)
    cout << containedPart << endl;
  if (containedPart.find(unhiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::UNHIDDEN;
  } else if (containedPart.find(hiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::HIDDEN;
  } else {
    m_displayType = LINK_DISPLAY_TYPE::DIRECT;
  }
  if (debug >= LOG_INFO)
    cout << "display Type:" << displayPropertyAsString() << endl;
}

/**
 * judge a link is referring to main text, attachment or original text.
 * for example, LINK_TYPE::ATTACHMENT would be returned for below link,
 * <a unhidden href="b018_7.htm">happy</a>
 * and assign type member field correspondingly.
 * @param linkString the link to check
 */
void Link::readType(const string &linkString) {
  m_type = LINK_TYPE::SAMEPAGE;
  auto fileEnd = linkString.find(HTML_SUFFIX);
  if (fileEnd == string::npos) // no file to refer
  {
    return;
  }
  string start = referFileMiddleChar;
  auto fileBegin = linkString.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return;
  }
  string refereFileName = linkString.substr(
      fileBegin + start.length(), fileEnd - fileBegin - start.length());
  m_type = getLinKTypeFromReferFileName(refereFileName);
  if (debug >= LOG_INFO)
    cout << "type seen from prefix: " << refereFileName << endl;
}

/**
 * get the referred name in the target file
 * and assign referPara member field correspondingly.
 * for example, P8L2 would be returned for below link,
 * <a unhidden href="..\original\c080.htm#P8L2">
 * @param linkString the link to check
 */
void Link::readReferPara(const string &linkString) {
  string htmStart = HTML_SUFFIX + referParaMiddleChar;
  if (m_type == LINK_TYPE::SAMEPAGE)
    htmStart = referParaMiddleChar;
  auto processStart = linkString.find(htmStart);
  if (processStart == string::npos) // no file to refer
  {
    if (debug >= LOG_EXCEPTION and m_type != LINK_TYPE::ATTACHMENT)
      cout << "no # found to read referPara from link: " << linkString << endl;
    return;
  }
  string afterLink = linkString.substr(processStart + htmStart.length(),
                                       linkString.length() - processStart);
  string end = referParaEndChar;
  auto processEnd = afterLink.find(end);
  m_referPara = afterLink.substr(0, processEnd);
  if (debug >= LOG_INFO)
    cout << "referPara: " << m_referPara << endl;
}

/**
 * annotation is the meaning of the link.
 * it is the last part of a link normally, like the final 头一社 in below link,
 * <a href="a043.htm"><i hidden>头2社</i>头一社</a>
 * get this value and assign annotation member field correspondingly.
 * @param linkString the link to analysize
 * @return true if there is target specified
 */
bool Link::readAnnotation(const string &linkString) {
  string htmStart = HTML_SUFFIX;
  if (m_type == LINK_TYPE::SAMEPAGE)
    htmStart = referParaMiddleChar;
  auto processStart = linkString.find(htmStart);
  if (processStart == string::npos) // no file to refer
  {
    if (linkString.find(returnToContentTable) == string::npos) {
      cout << "no htm or # is found to read annotation from link: "
           << linkString << endl;
      return false;
    } else {
      // type would be ignored in this case
      m_annotation = returnToContentTable;
      return true;
    }
  }
  string afterLink =
      linkString.substr(processStart, linkString.length() - processStart);
  string end = linkEndChars;
  auto afterLinkEnd = afterLink.find(end);
  afterLink = afterLink.substr(0, afterLinkEnd);
  string start = charBeforeAnnotation;
  auto afterLinkBegin = afterLink.find_last_of(start);
  while (afterLinkBegin >= commentEndChars.length() - 1) {
    if (afterLink.substr(afterLinkBegin - commentEndChars.length() + 1,
                         commentEndChars.length()) != commentEndChars)
      break;
    else {
      // skip this whole comment
      auto commentStart = afterLink.find_last_of(
          commentBeginChars, afterLinkBegin - commentEndChars.length());
      afterLinkBegin = afterLink.find_last_of(
          start, commentStart - commentBeginChars.length());
    }
  }
  m_annotation =
      afterLink.substr(afterLinkBegin + start.length(),
                       afterLink.length() - afterLinkBegin - start.length());
  if (debug >= LOG_INFO)
    cout << "annotation: " << m_annotation << endl;
  return true;
}

/**
 * key is always in <i hidden></i> part of link.
 * it is one word should be in the target file,
 * thru which the line the word is in could be found
 * and its line number would be used to update referPara,
 * usedKey member field would also get updated.
 * for example, 菱角菱花 is the key to update P0L0 in below link to P8L2,
 * if the line 菱角菱花 is actually in a080.htm line P8L2
 * <a hidden href="..\a080.htm#P0L0"><i hidden>菱角菱花</i>
 * needChange member field would also get update if
 * 1. referPara needs update as above
 * 2. key itself needs update like changed to "KeyNotFound".
 * so if tag <i hidden></i> is missing, KeyNotFound would be returned
 * and search would not start.
 * else, if the key in this tag cannot be found in target file,
 * KeyNotFound + key would be returned means manual fix is also needed.
 * so if key is still in "KeyNotFound+key" format,
 * which means manual fix is not done, simply return and do nothing.
 * @param linkString the link to analysize
 */
void Link::readKey(const string &linkString) {
  string copy = linkString;
  // remove space in the input linkString, thus key cannot contain space
  copy.erase(remove(copy.begin(), copy.end(), ' '), copy.end());
  string keyStart = referParaEndChar + keyStartChars;
  keyStart.erase(remove(keyStart.begin(), keyStart.end(), ' '), keyStart.end());
  string start = keyStart;
  auto keyBegin = copy.find(start);
  if (keyBegin == string::npos) {
    // not a link to top/bottom and no key was found, add a "KeyNotFound" key
    // and return
    if (m_type != LINK_TYPE::ATTACHMENT and
        m_referPara != topParagraphIndicator and
        m_referPara != bottomParagraphIndicator) {
      m_needChange = true;
      m_usedKey = keyNotFound;
    }
    if (debug >= LOG_EXCEPTION and m_type != LINK_TYPE::ATTACHMENT) {
      string output = m_usedKey.empty() ? "EMPTY" : m_usedKey;
      cout << "key string not found, so use key: " << output << endl;
    }
    return;
  }
  auto keyEnd = copy.find(keyEndChars);
  string stringForSearch = copy.substr(keyBegin + start.length(),
                                       keyEnd - keyBegin - start.length());
  if (stringForSearch.find(keyNotFound) !=
      string::npos) // still need manual change to a search-able key
  {
    m_needChange = false;
    m_usedKey = stringForSearch;
    return;
  }
  CoupledBodyText bodyText(getBodyTextFilePrefix());
  bodyText.resetBeforeSearch();
  // special hack to ignore itself
  if (isTargetToSelfHtm()) {
    bodyText.addIgnoreLines(m_fromLine.asString());
  }
  bodyText.setFileAndAttachmentNumber(getChapterName(), m_attachmentNumber);
  bool found = bodyText.findKey(stringForSearch);
  if (not found) {
    m_usedKey = keyNotFound + " " + stringForSearch;
    fixReferPara(changeKey); // will set needChange if found line is different
  } else {
    m_usedKey = stringForSearch;
    // only use the first line found
    string lineNumber = bodyText.getFirstResultLine();
    if (debug >= LOG_INFO)
      cout << "line number found: " << lineNumber << endl;
    LineNumber ln(lineNumber);
    string expectedSection =
        citationChapterNo + TurnToString(m_chapterNumber) + citationChapter +
        TurnToString(ln.getParaNumber()) + citationChapterParaSeparator +
        TurnToString(ln.getlineNumber()) + citationPara;
    fixReferPara(
        ln.asString()); // will set needChange if found line is different
    fixReferSection(expectedSection);
  }
  if (debug >= LOG_INFO)
    cout << "key: " << m_usedKey << endl;
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
  cout << "attachment information are written into: "
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
      cout << "fromLine not found in attachmentTable about: " << num.first
           << attachmentFileMiddleChar << num.second << endl;
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
    cout << "create link to original main html thru key: " << m_usedKey << endl;
  auto reservedType = m_type;   // only LINK_TYPE::MAIN has origin member
  m_type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to = fixLinkFromOriginalTemplate(
      getPathOfReferenceFile(), getChapterName(), m_usedKey, "", m_referPara);
  m_linkPtrToOrigin = std::make_unique<LinkFromMain>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  m_needChange = true;
  m_type = reservedType;
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
bool LinkFromMain::readReferFileName(const string &link) {
  string linkString = link;
  string refereFileName = m_fromFile;
  if (m_type != LINK_TYPE::SAMEPAGE) {
    // remove space in the input linkString
    linkString.erase(remove(linkString.begin(), linkString.end(), ' '),
                     linkString.end());
    auto fileEnd = linkString.find(HTML_SUFFIX);
    if (fileEnd == string::npos) // no file to refer
    {
      cout << "there is no file to refer in link: " << linkString << endl;
      return false;
    }
    string start = referFileMiddleChar;
    auto fileBegin = linkString.find(start);
    if (fileBegin == string::npos) // referred file not found
    {
      cout << "there is no file to refer in link: " << linkString << endl;
    }
    refereFileName = linkString.substr(fileBegin + start.length(),
                                       fileEnd - fileBegin - start.length());
    fileBegin = refereFileName.find(getHtmlFileNamePrefix());
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getHtmlFileNamePrefix().length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (m_type == LINK_TYPE::ATTACHMENT) {
    auto attachmentNumberStart = refereFileName.find(attachmentFileMiddleChar);
    if (attachmentNumberStart == string::npos) {
      cout << "no attachment number in link: " << linkString << endl;
      return false;
    }
    if (debug >= LOG_INFO)
      cout << "chapterNumber: "
           << refereFileName.substr(0, attachmentNumberStart)
           << ", attachmentNumber: "
           << refereFileName.substr(attachmentNumberStart + 1) << endl;
    m_attachmentNumber =
        TurnToInt(refereFileName.substr(attachmentNumberStart + 1));
    m_chapterNumber =
        TurnToInt(refereFileName.substr(0, attachmentNumberStart));
  } else {
    m_chapterNumber = TurnToInt(refereFileName);
  }
  if (debug >= LOG_INFO)
    cout << "chapterNumber: " << m_chapterNumber
         << ", attachmentNumber: " << m_attachmentNumber << endl;
  return true;
}

/**
 *  the directory structure is like below
 *  refer to utf8HTML/src
 *  \              <-link from main to access main
 *  \attachment\   <-link from attachment to access main to access other
 *  \original\     <- no link in text here
 *  \a*.htm        <- main texts
 * @param linkString the link to check
 * @return the level difference between a link and its target
 */
string LinkFromMain::getPathOfReferenceFile() const {
  string result{""};
  if (m_type == LINK_TYPE::ATTACHMENT)
    result = attachmentDirForLinkFromMain;
  if (m_type == LINK_TYPE::ORIGINAL)
    result = originalDirForLinkFromMain;
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
        cout << "entry.size: " << entry.size()
             << " more reference to link: " << getChapterName() << " "
             << m_referPara << " " << m_usedKey << endl;
    } catch (exception &) {
      if (debug >= LOG_INFO)
        cout << "create vector for : " << getChapterName() << " " << m_referPara
             << endl;
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
      cout << m_fromFile << " has a link to " << targetFile
           << " with annotation: " << getAnnotation()
           << " differs from title: " << title << endl;
    }
  }
}

/**
 * from the type of link to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of link
 * @return filename prefix of target file
 */
string LinkFromMain::getHtmlFileNamePrefix() {
  //	  if (m_type == LINK_TYPE::MAIN or m_type == LINK_TYPE::SAMEPAGE)
  string prefix = MAIN_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_HTML_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = ATTACHMENT_HTML_PREFIX;
  return prefix;
}

/**
 * from the type of link to get the filename prefix of bodytext file
 * all main bodytext files are MainXX.txt
 * all attachment bodytext files are AttachXX.txt
 * all original bodytext files are OrgXX.txt
 * @param type type of link
 * @return filename prefix of bodytext file
 */
string LinkFromMain::getBodyTextFilePrefix() {
  //	  if (m_type == LINK_TYPE::MAIN or m_type == LINK_TYPE::SAMEPAGE)
  string prefix = MAIN_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT)
    prefix = ATTACHMENT_BODYTEXT_PREFIX;
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
    cout << "create link to original main html thru key: " << m_usedKey << endl;
  auto reservedType = m_type;
  m_type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to = fixLinkFromOriginalTemplate(
      getPathOfReferenceFile(), getChapterName(), m_usedKey, "", m_referPara);
  m_linkPtrToOrigin = std::make_unique<LinkFromAttachment>(m_fromFile, to);
  m_linkPtrToOrigin->readReferFileName(to);
  m_linkPtrToOrigin->fixFromString(to);
  m_needChange = true;
  m_type = reservedType;
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
bool LinkFromAttachment::readReferFileName(const string &link) {
  string linkString = link;
  string refereFileName = m_fromFile;
  if (m_type != LINK_TYPE::SAMEPAGE) {
    // remove space in the input linkString
    linkString.erase(remove(linkString.begin(), linkString.end(), ' '),
                     linkString.end());
    auto fileEnd = linkString.find(HTML_SUFFIX);
    if (fileEnd == string::npos) // no file to refer
    {
      cout << "there is no file to refer in link: " << linkString << endl;
      return false;
    }
    string start = referFileMiddleChar;
    auto fileBegin = linkString.find(start);
    if (fileBegin == string::npos) // referred file not found
    {
      cout << "there is no file to refer in link: " << linkString << endl;
    }
    refereFileName = linkString.substr(fileBegin + start.length(),
                                       fileEnd - fileBegin - start.length());
    fileBegin = refereFileName.find(getHtmlFileNamePrefix());
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getHtmlFileNamePrefix().length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (m_type == LINK_TYPE::SAMEPAGE or m_type == LINK_TYPE::ATTACHMENT) {
    auto attachmentNumberStart = refereFileName.find(attachmentFileMiddleChar);
    if (attachmentNumberStart == string::npos) {
      cout << "no attachment number in link: " << linkString << endl;
      return false;
    }
    if (debug >= LOG_INFO)
      cout << "chapterNumber: "
           << refereFileName.substr(0, attachmentNumberStart)
           << ", attachmentNumber: "
           << refereFileName.substr(attachmentNumberStart + 1) << endl;

    m_attachmentNumber =
        TurnToInt(refereFileName.substr(attachmentNumberStart + 1));
    m_chapterNumber =
        TurnToInt(refereFileName.substr(0, attachmentNumberStart));
  } else {
    m_chapterNumber = TurnToInt(refereFileName);
  }
  if (debug >= LOG_INFO)
    cout << "chapterNumber: " << m_chapterNumber
         << ", attachmentNumber: " << m_attachmentNumber << endl;
  return true;
}

/**
 *  the directory structure is like below
 *  refer to utf8HTML/src
 *  \              <-link from main to access main
 *  \attachment\   <-link from attachment to access main to access other
 *  \original\     <- no link in text here
 *  \a*.htm        <- main texts
 * @param linkString the link to check
 * @return the level difference between a link and its target
 */
string LinkFromAttachment::getPathOfReferenceFile() const {
  string result{""};
  if (m_type == LINK_TYPE::MAIN or m_annotation == returnToContentTable)
    result = mainDirForLinkFromAttachment;
  if (m_type == LINK_TYPE::ORIGINAL)
    result = originalDirForLinkFromAttachment;
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
        cout << "not found link for: "
             << getChapterName() + attachmentFileMiddleChar +
                    TurnToString(m_attachmentNumber)
             << " " << m_referPara << " " << m_usedKey << endl;
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
 * from the type of link to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of link
 * @return filename prefix of target file
 */
string LinkFromAttachment::getHtmlFileNamePrefix() {
  //	  if (m_type == LINK_TYPE::MAIN)
  string prefix = MAIN_HTML_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_HTML_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT or m_type == LINK_TYPE::SAMEPAGE)
    prefix = ATTACHMENT_HTML_PREFIX;
  return prefix;
}

/**
 * from the type of link to get the filename prefix of bodytext file
 * all main bodytext files are MainXX.txt
 * all attachment bodytext files are AttachXX.txt
 * all original bodytext files are OrgXX.txt
 * @param type type of link
 * @return filename prefix of bodytext file
 */
string LinkFromAttachment::getBodyTextFilePrefix() {
  //	  if (m_type == LINK_TYPE::MAIN)
  string prefix = MAIN_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ORIGINAL)
    prefix = ORIGINAL_BODYTEXT_PREFIX;
  if (m_type == LINK_TYPE::ATTACHMENT or m_type == LINK_TYPE::SAMEPAGE)
    prefix = ATTACHMENT_BODYTEXT_PREFIX;
  return prefix;
}

ObjectPtr createObjectFromType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return std::make_unique<LineNumber>();
  else if (type == OBJECT_TYPE::IMAGEREF)
    return std::make_unique<ImageReferText>();
  else if (type == OBJECT_TYPE::SPACE)
    return std::make_unique<Space>();
  else if (type == OBJECT_TYPE::POEM)
    return std::make_unique<Poem>();
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return std::make_unique<LinkFromMain>();
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return std::make_unique<PersonalComment>();
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return std::make_unique<PoemTranslation>();
  else if (type == OBJECT_TYPE::COMMENT)
    return std::make_unique<Comment>();
  return nullptr;
}

string getStartTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return UnhiddenLineNumberStart;
  else if (type == OBJECT_TYPE::IMAGEREF)
    return ImgRefBeginChars;
  else if (type == OBJECT_TYPE::SPACE)
    return space;
  else if (type == OBJECT_TYPE::POEM)
    return poemBeginChars;
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return linkStartChars;
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return personalCommentStartChars;
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return poemTranslationBeginChars;
  else if (type == OBJECT_TYPE::COMMENT)
    return commentBeginChars;
  return "";
}

string getEndTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINKFROMMAIN)
    return linkEndChars;
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return personalCommentEndChars;
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return poemTranslationEndChars;
  else if (type == OBJECT_TYPE::COMMENT)
    return commentEndChars;
  return "";
}

string scanForSubType(const string &original, OBJECT_TYPE subType) {
  string result;
  using SubStringOffsetTable =
      std::map<size_t, size_t>; // start offset -> end offset
  SubStringOffsetTable subStrings;
  auto offset = original.find(getStartTagOfObjectType(subType));
  do {
    if (offset == string::npos)
      break;
    subStrings[offset] = original.find(getEndTagOfObjectType(subType), offset);
    offset = original.find(getStartTagOfObjectType(subType), offset + 1);
  } while (true);
  auto endOfSubStringOffset = 0;
  for (const auto &link : subStrings) {
    result += original.substr(endOfSubStringOffset,
                              link.first - endOfSubStringOffset);
    auto current = createObjectFromType(subType);
    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset =
        link.second + getEndTagOfObjectType(subType).length();
  }
  result += original.substr(endOfSubStringOffset);
  return result;
}

/**
 * must ensure this is not a line number before calling this method
 */
size_t Link::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  auto linkBegin = containedLine.find(linkStartChars, after);
  if (linkBegin == string::npos) // no link any more, continue with next
                                 // line
    return string::npos;
  auto linkEnd = containedLine.find(linkEndChars, linkBegin);
  auto linkString = containedLine.substr(
      linkBegin, linkEnd + linkEndChars.length() - linkBegin);
  if (debug >= LOG_INFO)
    cout << "original length: " << linkString.length() << endl;
  readTypeAndAnnotation(linkString);
  readReferFileName(linkString); // second step of construction
  fixFromString(linkString);
  if (debug >= LOG_INFO)
    cout << "after fix length: " << length() << endl;
  m_bodyText = m_annotation;
  m_displayText = scanForSubType(m_bodyText, OBJECT_TYPE::COMMENT);

  return containedLine.find(linkStartChars, after);
}

static const string personalCommentTemplate =
    R"(<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">XX</u>)";

string fixPersonalCommentFromTemplate(const string &comment) {
  string result = personalCommentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string PersonalComment::getWholeString() {
  return fixPersonalCommentFromTemplate(m_bodyText);
}
string PersonalComment::getDisplayString() { return m_displayText; }

size_t PersonalComment::length() { return getWholeString().length(); }
size_t PersonalComment::displaySize() { return getDisplayString().length(); }

size_t PersonalComment::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  auto personalCommentBegin =
      containedLine.find(personalCommentStartChars, after);
  if (personalCommentBegin == string::npos) // no personalComment any more,
    return string::npos;
  auto personalCommentEnd =
      containedLine.find(personalCommentEndChars, personalCommentBegin);
  string part = containedLine.substr(personalCommentBegin,
                                     personalCommentEnd - personalCommentBegin);
  auto beginPos = part.find(endOfPersonalCommentBeginTag);
  m_bodyText = part.substr(beginPos + endOfPersonalCommentBeginTag.length());
  m_displayText = scanForSubType(m_bodyText, OBJECT_TYPE::LINKFROMMAIN);
  return containedLine.find(personalCommentStartChars, after);
}

static const string poemTranslationTemplate =
    R"(<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">XX</samp> )";

string fixPoemTranslationFromTemplate(const string &translation) {
  string result = poemTranslationTemplate;
  replacePart(result, "XX", translation);
  return result;
}

string PoemTranslation::getWholeString() {
  return fixPoemTranslationFromTemplate(m_bodyText);
}
string PoemTranslation::getDisplayString() { return m_displayText; }

size_t PoemTranslation::length() { return getWholeString().length(); }
size_t PoemTranslation::displaySize() { return getDisplayString().length(); }

size_t PoemTranslation::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  auto poemTranslationBegin =
      containedLine.find(poemTranslationBeginChars, after);
  if (poemTranslationBegin == string::npos) // no poemTranslation any more,
    return string::npos;
  auto poemTranslationEnd =
      containedLine.find(poemTranslationEndChars, poemTranslationBegin);
  string part = containedLine.substr(poemTranslationBegin,
                                     poemTranslationEnd - poemTranslationBegin);
  auto beginPos = part.find(endOfPoemTranslationBeginTag);
  m_bodyText = part.substr(beginPos + endOfPoemTranslationBeginTag.length());
  m_displayText = scanForSubType(m_bodyText, OBJECT_TYPE::LINKFROMMAIN);
  return containedLine.find(poemTranslationBeginChars, after);
}

static const string commentTemplate =
    R"(<cite unhidden>XX</cite>)";

string fixCommentFromTemplate(const string &comment) {
  string result = commentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string Comment::getWholeString() { return fixCommentFromTemplate(m_bodyText); }
string Comment::getDisplayString() { return m_displayText; }

size_t Comment::length() { return getWholeString().length(); }
size_t Comment::displaySize() { return getDisplayString().length(); }

size_t Comment::loadFirstFromContainedLine(const string &containedLine,
                                           size_t after) {
  auto commentBegin = containedLine.find(commentBeginChars, after);
  if (commentBegin == string::npos) // no comment any more,
    return string::npos;
  auto commentEnd = containedLine.find(commentEndChars, commentBegin);
  string part = containedLine.substr(commentBegin, commentEnd - commentBegin);
  auto beginPos = part.find(endOfCommentBeginTag);
  m_bodyText = part.substr(beginPos + endOfCommentBeginTag.length());
  m_displayText = scanForSubType(m_bodyText, OBJECT_TYPE::LINKFROMMAIN);
  return containedLine.find(commentBeginChars, after);
}

/**
 * this function needs numbering first to get a correct target refer line number
 * or copy .txt files under testData to bodyTexts\output
 */
void testLinkFromMain(string fromFile, string linkString,
                      bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;
  LinkFromMain lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLinkFromAttachment(string fromFile, string linkString,
                            bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;
  LinkFromAttachment lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  cout << lfm.getAnnotation() << endl;
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLink(Link &lfm, string linkString, bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;

  //
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLinkOperation() {
  string linkString =
      R"(<a hidden href="a080.htm#top">原是)" + commentStart +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(使唤的</a>)";
  cout << "original link: " << endl << linkString << endl;
  LinkFromMain lfm("75", linkString);
  lfm.readReferFileName(linkString); // second step of construction
  cout << "change to refer to file 57. " << endl;
  lfm.fixReferFile(57);
  lfm.fixFromString(linkString);
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
  SEPERATE("fixReferFile", " finished ");

  linkString =
      R"(<a hidden href="a080.htm#top">原是)" + commentStart +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(使唤的</a>)";
  testLinkFromMain("07", linkString, false);
  SEPERATE("#top", " finished ");

  testLinkFromMain(
      "07",
      R"(<a hidden href="attachment\b003_9.htm#P2L3">原是老奶奶（薛姨妈）使唤的</a>)",
      false);
  SEPERATE("WARNING:", " SUCH LINK'S REFERPARA WON'T BE FIXED AUTOMATICALLY. ");
  SEPERATE("attachment with referPara", " finished ");

  testLinkFromMain(
      "80",
      fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
                                  "原是老奶奶（薛姨妈）使唤的", "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  linkString = fixLinkFromMainTemplate(
      "", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  LinkFromMain link("07", linkString);
  testLink(link, linkString, false);

  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromMain("03",
                   fixLinkFromMainTemplate(
                       "", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
                       "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   true);
  SEPERATE("generate original link afterwards", " finished ");

  testLinkFromMain("07",
                   fixLinkFromOriginalTemplate(originalDirForLinkFromMain, "18",
                                               "happy",
                                               "第80章1.1节:", "90101"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromMain("07",
                   fixLinkFromAttachmentTemplate(attachmentDirForLinkFromMain,
                                                 "18", "7", "happy"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  cout << "original link: " << endl
       << fixLinkFromJPMTemplate(jpmDirForLinkFromMain, "018",
                                 R"(拆牌)", "第80章1.1节:", "90101");
  SEPERATE("fixLinkFromJPMTemplate", " finished ");

  SEPERATE("testLinkFromMain", " finished ");

  string linkString1 = fixLinkFromMainTemplate(
      R"(..\)", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  string linkString2 = fixLinkFromAttachmentTemplate("", "18", "7", "happy");
  cout << "original link: " << endl << linkString2 << endl;
  LinkFromAttachment lfm1("03_9", linkString2);
  lfm1.readReferFileName(linkString2); // second step of construction
  cout << "change to refer to file 55_3. " << endl;
  lfm1.fixReferFile(55, 3);
  lfm1.fixFromString(linkString2);
  auto fixed2 = lfm1.asString();
  cout << "need Update: " << lfm1.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed2 << endl;
  SEPERATE("fixReferFile", " finished ");
  testLinkFromAttachment("1_0",
                         R"(<a unhidden href="..\aindex.htm">回目录</a>)",
                         false);
  SEPERATE("回目录", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE::HIDDEN, "西北",
                                  "原是老奶奶（薛姨妈）使唤的", "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromMainTemplate(
          R"(..\)", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
          "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
      true);
  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromAttachment("03_9",
                         fixLinkFromOriginalTemplate(R"(..\original\)", "80",
                                                     "菱角菱花",
                                                     "第80章1.1节:", "94"),
                         false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromAttachment(
      "03_9", fixLinkFromAttachmentTemplate("", "18", "7", "happy"), false);
  SEPERATE("fixLinkFromAttachmentTemplate", " finished ");
  SEPERATE("testLinkFromAttachment", " finished ");
}
