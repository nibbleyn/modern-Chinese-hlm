#include "hlmLink.hpp"
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
    auto key = link.first;
    auto fromList = link.second;
    // link itself
    outfile << "link:" << endl
            << GetTupleElement(key, 0) << "," << GetTupleElement(key, 1) << ":"
            << GetTupleElement(key, 2) << endl
            << "linked from :" << endl;
    for (const auto &from : fromList) {
      outfile << "    " << GetTupleElement(from, 0) << ","
              << GetTupleElement(from, 1) << ":" << endl
              << "    " << GetTupleElement(from, 2) << endl;
    }
  }
}

/**
 * the string to write into file of a type
 * @param type ATTACHMENT_TYPE to convert
 * @return corresponding string
 */
string attachmentTypeAsString(ATTACHMENT_TYPE type) {
  return (type == ATTACHMENT_TYPE::PERSONAL) ? "1" : "0";
}

/**
 * convert string read from file back to type
 * @param str "0" or "1" from file
 * @return REFERENCE or PERSONAL
 */
ATTACHMENT_TYPE attachmentTypeFromString(const string &str) {
  return (str == "1") ? ATTACHMENT_TYPE::PERSONAL : ATTACHMENT_TYPE::REFERENCE;
}

/**
 * find the type of one attachment from refAttachmentTable
 * which is loaded from loadReferenceAttachmentList()
 * @param num pair of chapter number and attachment number
 * @return "1" if found in refAttachmentTable with value "1"
 *  otherwise, return "0" if founded with value "0"
 *  otherwise, return "2"
 *  "1" means a personal review could be removed thru removePersonalViewpoints()
 *  "0" means reference to other stories or about a person
 */
ATTACHMENT_TYPE Link::getAttachmentType(AttachmentNumber num) {
  ATTACHMENT_TYPE attachmentType = ATTACHMENT_TYPE::NON_EXISTED;
  try {
    auto entry = refAttachmentTable.at(num).second;
    attachmentType = GetTupleElement(entry, 2);
  } catch (exception &) {
    if (debug)
      cout << "not found info about: " << num.first << "_" << num.second
           << endl;
  }
  return attachmentType;
}

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
    string type = "0";
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
    if (debug)
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

void Link::resetStatisticsAndLoadReferenceAttachmentList() {
  linksTable.clear();
  loadReferenceAttachmentList();
}

void Link::outPutStatisticsToFiles() { displayFixedLinks(); }

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
  if (debug)
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
  auto fileEnd = linkString.find(".htm");
  if (fileEnd == string::npos) // no file to refer
  {
    return;
  }
  string start = R"(href=")";
  auto fileBegin = linkString.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return;
  }
  string refereFileName = linkString.substr(
      fileBegin + start.length(), fileEnd - fileBegin - start.length());
  type = getLinKTypeFromReferFileName(refereFileName);
  if (debug)
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
  string htmStart = R"(.htm#)";
  if (type == LINK_TYPE::SAMEPAGE)
    htmStart = R"(#)";
  auto processStart = linkString.find(htmStart);
  if (processStart == string::npos) // no file to refer
  {
    if (debug)
      cout << "no # found to read referPara from link: " << linkString << endl;
    return;
  }
  string afterLink = linkString.substr(processStart + htmStart.length(),
                                       linkString.length() - processStart);
  string htmEnd = R"(">)";
  auto processEnd = afterLink.find(htmEnd);
  referPara = afterLink.substr(0, processEnd);
  if (debug)
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
  string htmStart = R"(.htm)";
  if (type == LINK_TYPE::SAMEPAGE)
    htmStart = R"(#)";
  auto processStart = linkString.find(htmStart);
  if (processStart == string::npos) // no file to refer
  {
    cout << "no htm or # is found to read annotation from link: " << linkString
         << endl;
    return false;
  }
  string afterLink =
      linkString.substr(processStart, linkString.length() - processStart);
  string end = R"(</a>)";
  auto afterLinkEnd = afterLink.find(end);
  afterLink = afterLink.substr(0, afterLinkEnd);
  string start = R"(>)";
  auto afterLinkBegin = afterLink.find_last_of(start);
  annotation =
      afterLink.substr(afterLinkBegin + start.length(),
                       afterLink.length() - afterLinkBegin - start.length());
  if (debug)
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
 * KeyNotFound:key would be returned means manual fix is also needed.
 * so if key is still in "KeyNotFound:key" format,
 * which means manual fix is not done, simply return and do nothing.
 * @param linkString the link to analysize
 */
void Link::readKey(const string &linkString) {
  string copy = linkString;
  // remove space in the input linkString, thus key cannot contain space
  copy.erase(remove(copy.begin(), copy.end(), ' '), copy.end());
  string start = R"("><ihidden>)";
  auto keyBegin = copy.find(start);
  if (keyBegin == string::npos) {
    // not a link to top/bottom and no key was found, add a "KeyNotFound" key
    // and return
    if (type != LINK_TYPE::ATTACHMENT and referPara != "top" and
        referPara != "bottom") {
      needChange = true;
      usedKey = "KeyNotFound";
    }
    if (debug) {
      string output = usedKey.empty() ? "EMPTY" : usedKey;
      cout << "key string not found, so use key: " << output << endl;
    }
    return;
  }
  auto keyEnd = copy.find("</i>");
  string stringForSearch = copy.substr(keyBegin + start.length(),
                                       keyEnd - keyBegin - start.length());
  if (stringForSearch.find("KeyNotFound") !=
      string::npos) // still need manual change to a search-able key
  {
    needChange = false;
    usedKey = stringForSearch;
    return;
  }
  string lineNumber = "";
  // special hack to ignore itself
  if (type == LINK_TYPE::SAMEPAGE)
    lineNumber = fromLine.asString();

  needChange = false;
  string attachmentPart{""};
  if (attachmentNumber != 0) {
    attachmentPart = "_" + TurnToString(attachmentNumber);
  }
  // search key in referred file
  string referFile = BODY_TEXT_OUTPUT + getBodyTextFilePrefix(type) +
                     getChapterName() + attachmentPart + ".txt";
  string newKey =
      findKeyInFile(stringForSearch, referFile, lineNumber, needChange);
  if (debug)
    cout << "key found: " << newKey << endl;
  if (newKey.find("KeyNotFound") == string::npos) {
    usedKey = stringForSearch;
    if (debug)
      cout << "line number found: " << lineNumber << endl;
    LineNumber ln(lineNumber);
    string expectedSection =
        R"(第)" + TurnToString(chapterNumber) + R"(章)" +
        TurnToString(ln.getParaNumber()) + R"(.)" +
        TurnToString(ln.getlineNumber()) +
        R"(节:)";
    fixReferPara(ln.asString());
    fixReferSection(expectedSection);
  } else {
    usedKey = "KeyNotFound:" + stringForSearch;
    fixReferPara("changeKey");
    needChange = true;
  }

  if (debug)
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
  string start = getFileNamePrefix(FILE_TYPE::ATTACHMENT);
  auto fileBegin = filename.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return num;
  }
  auto chapter = filename.substr(fileBegin + start.length(), 2);
  //  cout << chapter;
  num.first = TurnToInt(chapter);
  auto seqStart = filename.find("_");
  if (seqStart == string::npos) // no file to refer
  {
    return num;
  }
  auto seq = filename.substr(seqStart + 1, filename.length() - seqStart);
  num.second = TurnToInt(seq);
  return num;
}

/**
 *
 */
void LinkFromMain::displayAttachments() {
  if (attachmentTable.empty())
    return;
  cout << HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST << " is created." << endl;
  ofstream outfile(HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST);
  for (const auto &attachment : attachmentTable) {
    AttachmentFileNameTitleAndType entry = attachment.second.second;

    outfile << "from:" << attachment.second.first
            << " name:" << GetTupleElement(entry, 0)
            << " title:" << GetTupleElement(entry, 1)
            << " type:" << attachmentTypeAsString(GetTupleElement(entry, 2))
            << endl;
  }
}

/**
 * find fromLine of one attachment from attachmentTable
 * which is calculated during link fixing
 * @param num pair of chapter number and attachment number
 * @return the fromLine stored if existed, otherwise "top"
 */
string LinkFromMain::getFromLineOfAttachment(AttachmentNumber num) {
  string result = R"(top)";
  try {
    result = attachmentTable.at(num).first;
  } catch (exception &) {
    if (debug)
      cout << "fromLine not found about: " << num.first << "_" << num.second
           << endl;
  }
  return result;
}

/**
 *
 */
void LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList() {
  Link::resetStatisticsAndLoadReferenceAttachmentList();
  attachmentTable.clear();
}

/**
 *
 */
void LinkFromMain::outPutStatisticsToFiles() {
  outPutFilePath = HTML_OUTPUT_LINKS_FROM_MAIN_LIST;
  Link::outPutStatisticsToFiles();
  displayAttachments();
}

void LinkFromMain::generateLinkToOrigin() {
  if (debug)
    cout << "create link to original main html thru key: " << usedKey << endl;
  auto reservedType =  type; //only LINK_TYPE::MAIN has origin member
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
    auto fileEnd = linkString.find(".htm");
    if (fileEnd == string::npos) // no file to refer
    {
      cout << "there is no file to refer in link: " << linkString << endl;
      return false;
    }
    string start = R"(href=")";
    auto fileBegin = linkString.find(start);
    if (fileBegin == string::npos) // referred file not found
    {
      cout << "there is no file to refer in link: " << linkString << endl;
    }
    refereFileName = linkString.substr(fileBegin + start.length(),
                                       fileEnd - fileBegin - start.length());
    fileBegin = refereFileName.find(getFileNamePrefix(type));
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getFileNamePrefix(type).length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (type == LINK_TYPE::ATTACHMENT) {
    auto attachmentNumberStart = refereFileName.find("_");
    if (attachmentNumberStart == string::npos) {
      cout << "no attachment number in link: " << linkString << endl;
      return false;
    }
    if (debug)
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
  if (debug)
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
    try {
      auto &entry =
          linksTable.at(std::make_tuple(getChapterName(), referPara, usedKey));
      entry.push_back(
          std::make_tuple(fromFile, fromLine.asString(), asString()));
      if (debug)
        cout << "entry.size: " << entry.size()
             << " more reference to link: " << getChapterName() << " "
             << referPara << " " << usedKey << endl;
    } catch (exception &) {
      if (debug)
        cout << "not found link for: " << getChapterName() << " " << referPara
             << " " << usedKey << endl;
      vector<std::tuple<string, string, string>> list;
      list.push_back(
          std::make_tuple(fromFile, fromLine.asString(), asString()));
      linksTable[std::make_tuple(getChapterName(), referPara, usedKey)] = list;
    }
  }
  if (isTargetToOtherAttachmentHtm()) {
    auto targetFile = getFileNamePrefix(type) + getChapterName() +
                      R"(_)" + TurnToString(getattachmentNumber());
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
string LinkFromMain::getFileNamePrefix(LINK_TYPE _type) {
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
string LinkFromMain::getBodyTextFilePrefix(LINK_TYPE _type) {
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
/**
 *
 */
void LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList() {
  Link::resetStatisticsAndLoadReferenceAttachmentList();
}

/**
 *
 */
void LinkFromAttachment::outPutStatisticsToFiles() {
  outPutFilePath = HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST;
  Link::outPutStatisticsToFiles();
}

void LinkFromAttachment::generateLinkToOrigin() {
  if (debug)
    cout << "create link to original main html thru key: " << usedKey << endl;
  auto reservedType =  type;
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
    auto fileEnd = linkString.find(".htm");
    if (fileEnd == string::npos) // no file to refer
    {
      cout << "there is no file to refer in link: " << linkString << endl;
      return false;
    }
    string start = R"(href=")";
    auto fileBegin = linkString.find(start);
    if (fileBegin == string::npos) // referred file not found
    {
      cout << "there is no file to refer in link: " << linkString << endl;
    }
    refereFileName = linkString.substr(fileBegin + start.length(),
                                       fileEnd - fileBegin - start.length());
    fileBegin = refereFileName.find(getFileNamePrefix(type));
    if (fileBegin == string::npos) // not find a right file type to refer
    {
      cout << "unsupported type in refer file name in link: " << linkString
           << endl;
      return false;
    }
    refereFileName = refereFileName.substr(
        fileBegin); // in case there is a ..\ before file name
    refereFileName = refereFileName.substr(getFileNamePrefix(type).length());
  }

  // get chapter number and attachment number if type is LINK_TYPE::ATTACHMENT
  if (type == LINK_TYPE::SAMEPAGE or type == LINK_TYPE::ATTACHMENT) {
    auto attachmentNumberStart = refereFileName.find("_");
    if (attachmentNumberStart == string::npos) {
      cout << "no attachment number in link: " << linkString << endl;
      return false;
    }
    if (debug)
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
  if (debug)
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
    try {
      auto &entry = linksTable.at(std::make_tuple(
          getChapterName() + "_" + TurnToString(attachmentNumber), referPara,
          usedKey));
      entry.push_back(
          std::make_tuple(fromFile, fromLine.asString(), asString()));
    } catch (exception &) {
      if (debug)
        cout << "not found link for: "
             << getChapterName() + "_" + TurnToString(attachmentNumber) << " "
             << referPara << " " << usedKey << endl;
      vector<std::tuple<string, string, string>> list;
      list.push_back(
          std::make_tuple(fromFile, fromLine.asString(), asString()));
      linksTable[std::make_tuple(getChapterName(), referPara, usedKey)] = list;
    }
  }
}

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
string LinkFromAttachment::getFileNamePrefix(LINK_TYPE type) {
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
string LinkFromAttachment::getBodyTextFilePrefix(LINK_TYPE type) {
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