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
  string filenamePrefix[] = {"a0", "b0", "c0"};
  LINK_TYPE type = LINK_TYPE::SAMEPAGE;

  if (refereFileName.find(filenamePrefix[0]) != string::npos) {
    type = LINK_TYPE::MAIN;
  } else if (refereFileName.find(filenamePrefix[1]) != string::npos) {
    type = LINK_TYPE::ATTACHMENT;
  } else if (refereFileName.find(filenamePrefix[2]) != string::npos) {
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
    R"(<a unhidden href="#YY"><i hidden>QQ</i>ZZ</a>)";
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
  link = replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    link = replacePart(link, R"(<i hidden>QQ</i>)", key);
  } else {
    link = replacePart(link, "QQ", key);
  }
  link = replacePart(link, "ZZ", annotation);
  return link;
}

static const string linkToMainFile =
    R"(<a unhidden href="PPa0XX.htm#YY"><i hidden>QQ</i><b unhidden>WW</b>ZZ</a>)";
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
  link = replacePart(link, "PP", path);
  link = replacePart(link, "XX", filename);
  link = replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    link = replacePart(link, R"(<i hidden>QQ</i><b unhidden>WW</b>)", key);
  } else {
    link = replacePart(link, "QQ", key);
  }
  link = replacePart(link, "WW", citation);
  link = replacePart(link, "ZZ", annotation);
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
  link = replacePart(link, "XX", filename);
  link = replacePart(link, "YY", referPara);
  return link;
}

static const string linkToOriginalFile =
    R"(<a unhidden href="PPc0XX.htm#YY"><i hidden>QQ</i>原文</a>)";
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
                                   const string &key, const string &referPara) {
  auto link = linkToOriginalFile;
  link = replacePart(link, "PP", path);
  link = replacePart(link, "XX", filename);
  link = replacePart(link, "YY", referPara);
  if (key.empty()) // use top/bottom as reference name
  {
    link = replacePart(link, R"(<i hidden>QQ</i>)", key);
  } else {
    link = replacePart(link, "QQ", key);
  }
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
  link = replacePart(link, "PP", path);
  link = replacePart(link, "XX", filename);
  link = replacePart(link, "YY", attachNo);
  link = replacePart(link, "ZZ", annotation);
  return link;
}

static const string charBeforeAnnotation = R"(>)";
static const string commentEndChars = R"(</i>）)";
static const string referFileMiddleChar = R"(href=")";
static const string referParaMiddleChar = R"(#)";
static const string referParaEndChar = R"(">)";
static const string changeKey = R"(changeKey)";
static const string citationStartChars = R"(<b hidden>)";
static const string citationChapterNo = R"(第)";
static const string citationChapter = R"(章)";
static const string citationChapterParaSeparator = R"(.)";
static const string citationPara = R"(节:)";
static const string citationEndChars = R"(</b>)";

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

/**
 * generated the string of a link from its info like type, key, etc.
 * @return the string of the link
 */
string Link::asString() {
  string part0 = linkStartChars + " " + displayPropertyAsString() + " " +
                 referFileMiddleChar;
  string part1{""}, part2{""}, part3{""};
  if (annotation == returnToContentTable) // type would be SAMEPAGE
  {
    part1 = getPathOfReferenceFile() + contentTableFilename;
  } else if (type != LINK_TYPE::SAMEPAGE) {
    part1 =
        getPathOfReferenceFile() + getFileNamePrefixFromlinkType(type) + getChapterName();
    if (attachmentNumber != 0)
      part2 = attachmentFileMiddleChar + TurnToString(attachmentNumber);
    part3 = HTML_SUFFIX;
  }
  string part4{""};
  if (type != LINK_TYPE::ATTACHMENT or referPara != invalidLineNumber) {
    if (annotation != returnToContentTable)
      part4 = referParaMiddleChar + referPara;
  }
  string part5 = referParaEndChar, part6{""}, part7{""};
  if (type != LINK_TYPE::ATTACHMENT and not usedKey.empty()) {
    part6 = keyStartChars + getKey() + keyEndChars;
    // easier to replace to <b unhidden> if want to display this
    if (type == LINK_TYPE::MAIN)
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
  string copy = linkString;
  // remove space in the linkString
  copy.erase(remove(copy.begin(), copy.end(), ' '), copy.end());
  if (copy.find("aunhiddenhref") != string::npos) {
    displayType = LINK_DISPLAY_TYPE::UNHIDDEN;
  }
  if (copy.find("ahref") != string::npos) {
    displayType = LINK_DISPLAY_TYPE::DIRECT;
  }
  if (copy.find("ahiddenhref") != string::npos) {
    displayType = LINK_DISPLAY_TYPE::HIDDEN;
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
  type = LINK_TYPE::SAMEPAGE;
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
  type = getLinKTypeFromReferFileName(refereFileName);
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
  if (type == LINK_TYPE::SAMEPAGE)
    htmStart = referParaMiddleChar;
  auto processStart = linkString.find(htmStart);
  if (processStart == string::npos) // no file to refer
  {
    if (debug >= LOG_EXCEPTION and type != LINK_TYPE::ATTACHMENT)
      cout << "no # found to read referPara from link: " << linkString << endl;
    return;
  }
  string afterLink = linkString.substr(processStart + htmStart.length(),
                                       linkString.length() - processStart);
  string end = referParaEndChar;
  auto processEnd = afterLink.find(end);
  referPara = afterLink.substr(0, processEnd);
  if (debug >= LOG_INFO)
    cout << "referPara: " << referPara << endl;
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
  if (type == LINK_TYPE::SAMEPAGE)
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
      annotation = returnToContentTable;
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
  while (afterLinkBegin >= keyEndChars.length() - 1) {
    if (afterLink.substr(afterLinkBegin - keyEndChars.length() + 1,
                         commentEndChars.length()) != commentEndChars)
      break;
    else {
      // skip this whole comment
      auto commentStart = afterLink.find_last_of(
          commentBeginChars, afterLinkBegin - keyEndChars.length());
      afterLinkBegin = afterLink.find_last_of(
          start, commentStart - commentBeginChars.length());
    }
  }
  annotation =
      afterLink.substr(afterLinkBegin + start.length(),
                       afterLink.length() - afterLinkBegin - start.length());
  if (debug >= LOG_INFO)
    cout << "annotation: " << annotation << endl;
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
    if (type != LINK_TYPE::ATTACHMENT and referPara != topParagraphIndicator and
        referPara != bottomParagraphIndicator) {
      needChange = true;
      usedKey = keyNotFound;
    }
    if (debug >= LOG_EXCEPTION and type != LINK_TYPE::ATTACHMENT) {
      string output = usedKey.empty() ? "EMPTY" : usedKey;
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
    needChange = false;
    usedKey = stringForSearch;
    return;
  }
  BodyText bodyText(getBodyTextFilePrefixFromLinkType(type));
  bodyText.resetBeforeSearch();
  // special hack to ignore itself
  if (type == LINK_TYPE::SAMEPAGE) {
    bodyText.addIgnoreLines(fromLine.asString());
  }
  bool found =
      bodyText.findKey(stringForSearch, getChapterName(), attachmentNumber);
  if (not found) {
    usedKey = keyNotFound + " " + stringForSearch;
    fixReferPara(changeKey); // will set needChange if found line is different
  } else {
    usedKey = stringForSearch;
    // only use the first line found
    string lineNumber = bodyText.getFirstResultLine();
    if (debug >= LOG_INFO)
      cout << "line number found: " << lineNumber << endl;
    LineNumber ln(lineNumber);
    string expectedSection =
        citationChapterNo + TurnToString(chapterNumber) + citationChapter +
        TurnToString(ln.getParaNumber()) + citationChapterParaSeparator +
        TurnToString(ln.getlineNumber()) + citationPara;
    fixReferPara(
        ln.asString()); // will set needChange if found line is different
    fixReferSection(expectedSection);
  }
  if (debug >= LOG_INFO)
    cout << "key: " << usedKey << endl;
}

/**
 * record this file as one who has links of wrong/un-found key
 */
void Link::recordMissingKeyLink() { keyMissingChapters.insert(fromFile); }

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

/**
 * get chapter number and attachment number from an attachment file name
 * for example with input b001_15 would return pair <1,15>
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return pair of chapter number and attachment number
 */
AttachmentNumber getAttachmentNumber(const string &filename) {
  AttachmentNumber num(0, 0);
  string start = getFileNamePrefixFromFileType(FILE_TYPE::ATTACHMENT);
  auto fileBegin = filename.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return num;
  }
  auto chapter = filename.substr(fileBegin + start.length(), 2);
  //  cout << chapter;
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
    cout << "create link to original main html thru key: " << usedKey << endl;
  auto reservedType = type;   // only LINK_TYPE::MAIN has origin member
  type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to = fixLinkFromOriginalTemplate(getPathOfReferenceFile(),
                                          getChapterName(), usedKey, referPara);
  linkToOrigin = std::make_unique<LinkFromMain>(fromFile, to);
  linkToOrigin->readReferFileName(to);
  linkToOrigin->fixFromString(to);
  needChange = true;
  type = reservedType;
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
  string refereFileName = fromFile;
  if (type != LINK_TYPE::SAMEPAGE) {
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
    fileBegin = refereFileName.find(getFileNamePrefixFromlinkType(type));
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getFileNamePrefixFromlinkType(type).length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (type == LINK_TYPE::ATTACHMENT) {
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
    attachmentNumber =
        TurnToInt(refereFileName.substr(attachmentNumberStart + 1));
    chapterNumber = TurnToInt(refereFileName.substr(0, attachmentNumberStart));
  } else {
    chapterNumber = TurnToInt(refereFileName);
  }
  if (debug >= LOG_INFO)
    cout << "chapterNumber: " << chapterNumber
         << ", attachmentNumber: " << attachmentNumber << endl;
  return true;
}

/**
 * log a valid link found by putting it into linksTable
 * which is sorted by ChapterName, referPara, usedKey
 */
void LinkFromMain::logLink() {

  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{usedKey, fromFile, fromLine.asString(), asString()};
    try {
      auto &entry = linksTable.at(std::make_pair(getChapterName(), referPara));
      entry.push_back(detail);
      if (debug >= LOG_INFO)
        cout << "entry.size: " << entry.size()
             << " more reference to link: " << getChapterName() << " "
             << referPara << " " << usedKey << endl;
    } catch (exception &) {
      if (debug >= LOG_INFO)
        cout << "create vector for : " << getChapterName() << " " << referPara
             << endl;
      vector<LinkDetails> list;
      list.push_back(detail);
      linksTable[std::make_pair(getChapterName(), referPara)] = list;
    }
  }
  if (isTargetToOtherAttachmentHtm()) {
    auto targetFile = getFileNamePrefixFromlinkType(type) + getChapterName() +
                      attachmentFileMiddleChar +
                      TurnToString(getattachmentNumber());
    auto num = make_pair(getchapterNumer(), getattachmentNumber());
    auto title = getAttachmentTitle(targetFile);
    auto type = getAttachmentType(num);
    if (getSourceChapterName() == getChapterName()) {
      if (type == ATTACHMENT_TYPE::NON_EXISTED)
        newAttachmentList.insert(targetFile);
      attachmentTable[num] = make_pair(
          fromLine.asString(), make_tuple(targetFile, title,
                                          (type == ATTACHMENT_TYPE::PERSONAL)
                                              ? ATTACHMENT_TYPE::PERSONAL
                                              : ATTACHMENT_TYPE::REFERENCE));
    }
    if (not isAnnotationMatch(getAnnotation(), title)) {
      cout << fromFile << " has a link to " << targetFile
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
string LinkFromMain::getFileNamePrefixFromlinkType(LINK_TYPE _type) {
  string filenamePrefix[] = {"a0", "b0", "c0", "a0"};
  string prefix = "unsupported";
  if (type == LINK_TYPE::MAIN)
    prefix = filenamePrefix[0];
  if (type == LINK_TYPE::ATTACHMENT)
    prefix = filenamePrefix[1];
  if (type == LINK_TYPE::ORIGINAL)
    prefix = filenamePrefix[2];
  if (type == LINK_TYPE::SAMEPAGE)
    prefix = filenamePrefix[3];
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
string LinkFromMain::getBodyTextFilePrefixFromLinkType(LINK_TYPE _type) {
  string bodyTextFilePrefix[] = {"Main", "Attach", "Org", "Main"};
  string prefix = "unsupported";
  if (type == LINK_TYPE::MAIN)
    prefix = bodyTextFilePrefix[0];
  if (type == LINK_TYPE::ATTACHMENT)
    prefix = bodyTextFilePrefix[1];
  if (type == LINK_TYPE::ORIGINAL)
    prefix = bodyTextFilePrefix[2];
  if (type == LINK_TYPE::SAMEPAGE)
    prefix = bodyTextFilePrefix[3];
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
    cout << "create link to original main html thru key: " << usedKey << endl;
  auto reservedType = type;
  type = LINK_TYPE::ORIGINAL; // temporarily change type to get right path
  string to = fixLinkFromOriginalTemplate(getPathOfReferenceFile(),
                                          getChapterName(), usedKey, referPara);
  linkToOrigin = std::make_unique<LinkFromAttachment>(fromFile, to);
  linkToOrigin->readReferFileName(to);
  linkToOrigin->fixFromString(to);
  needChange = true;
  type = reservedType;
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
  string refereFileName = fromFile;
  if (type != LINK_TYPE::SAMEPAGE) {
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
    fileBegin = refereFileName.find(getFileNamePrefixFromlinkType(type));
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getFileNamePrefixFromlinkType(type).length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (type == LINK_TYPE::SAMEPAGE or type == LINK_TYPE::ATTACHMENT) {
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

    attachmentNumber =
        TurnToInt(refereFileName.substr(attachmentNumberStart + 1));
    chapterNumber = TurnToInt(refereFileName.substr(0, attachmentNumberStart));
  } else {
    chapterNumber = TurnToInt(refereFileName);
  }
  if (debug >= LOG_INFO)
    cout << "chapterNumber: " << chapterNumber
         << ", attachmentNumber: " << attachmentNumber << endl;
  return true;
}

/**
 * log a valid link found by putting it into linksTable
 * which is sorted by ChapterName+attachmentNumber, referPara, usedKey
 */
void LinkFromAttachment::logLink() {
  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{usedKey, fromFile, fromLine.asString(), asString()};
    try {
      auto &entry = linksTable.at(
          std::make_pair(getChapterName() + attachmentFileMiddleChar +
                             TurnToString(attachmentNumber),
                         referPara));
      entry.push_back(detail);
    } catch (exception &) {
      if (debug >= LOG_EXCEPTION)
        cout << "not found link for: "
             << getChapterName() + attachmentFileMiddleChar +
                    TurnToString(attachmentNumber)
             << " " << referPara << " " << usedKey << endl;
      vector<LinkDetails> list;
      list.push_back(detail);
      linksTable[std::make_pair(getChapterName() + attachmentFileMiddleChar +
                                    TurnToString(attachmentNumber),
                                referPara)] = list;
    }
  }
}

/**
 * change target type thru specifying a type prefix
 * @param prefix type prefix
 */
void LinkFromAttachment::setTypeThruFileNamePrefix(const string &prefix) {
  if (prefix == "main")
    type = LINK_TYPE::MAIN;
}

/**
 * from the type of link to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of link
 * @return filename prefix of target file
 */
string LinkFromAttachment::getFileNamePrefixFromlinkType(LINK_TYPE type) {
  string filenamePrefix[] = {"a0", "b0", "c0", "b0"};
  string prefix = "unsupported";
  if (type == LINK_TYPE::MAIN)
    prefix = filenamePrefix[0];
  if (type == LINK_TYPE::ATTACHMENT)
    prefix = filenamePrefix[1];
  if (type == LINK_TYPE::ORIGINAL)
    prefix = filenamePrefix[2];
  if (type == LINK_TYPE::SAMEPAGE)
    prefix = filenamePrefix[3];
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
string LinkFromAttachment::getBodyTextFilePrefixFromLinkType(LINK_TYPE type) {
  string bodyTextFilePrefix[] = {"Main", "Attach", "Org", "Attach"};
  string prefix = "unsupported";
  if (type == LINK_TYPE::MAIN)
    prefix = bodyTextFilePrefix[0];
  if (type == LINK_TYPE::ATTACHMENT)
    prefix = bodyTextFilePrefix[1];
  if (type == LINK_TYPE::ORIGINAL)
    prefix = bodyTextFilePrefix[2];
  if (type == LINK_TYPE::SAMEPAGE)
    prefix = bodyTextFilePrefix[3];
  return prefix;
}
