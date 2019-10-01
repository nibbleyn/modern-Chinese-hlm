#include "coupledLink.hpp"

CoupledLink::LinksTable CoupledLink::linksTable;
CoupledLink::ImageLinksTable CoupledLink::imageLinksTable;
bool CoupledLink::m_addSpecialDisplayText = false;
string CoupledLink::referFilePrefix{emptyString};
string CoupledLink::linkDetailFilePath{emptyString};
string CoupledLink::imageLinkDetailFilePath{emptyString};
string CoupledLink::keyDetailFilePath{emptyString};

AttachmentList LinkFromMain::attachmentTable;

static const string LINK_TARGET_STARTER = R"(link: )";
static const string FROM_LINK_STARTER = R"(key: )";
static const string FROM_LINK_KEY_SEPARATOR = R"(, linked from: )";
static const string FROM_LINK_STRING_SEPARATOR = R"( : )";

/**
 * reset the general data structure to log info about links
 */
void CoupledLink::clearLinkTable() {
  linksTable.clear();
  imageLinksTable.clear();
}

/**
 * output links to specified file
 * before calling this function, specifying output file
 */
void CoupledLink::outPutStatisticsToFiles() {
  if (linksTable.empty() and imageLinksTable.empty())
    return;
  if (not linksTable.empty()) {
    FUNCTION_OUTPUT << linkDetailFilePath << " is created." << endl;
    FUNCTION_OUTPUT << keyDetailFilePath << " is created." << endl;
    ofstream linkDetailOutfile(linkDetailFilePath);
    ofstream keyDetailOutfile(keyDetailFilePath);
    for (const auto &element : linksTable) {
      auto num = element.first.first;
      auto paraLine = element.first.second;
      LineNumber ln(paraLine.first, paraLine.second);
      auto fromLinks = element.second;
      // link itself
      linkDetailOutfile << LINK_TARGET_STARTER << referFilePrefix
                        << getFileNameFromAttachmentNumber(referFilePrefix, num)
                        << HTML_SUFFIX << referParaMiddleChar << ln.asString()
                        << endl;
      keyDetailOutfile << referFilePrefix
                       << getFileNameFromAttachmentNumber(referFilePrefix, num)
                       << HTML_SUFFIX << referParaMiddleChar << ln.asString()
                       << endl;
      using KeySet = set<string>;
      KeySet keySet;
      for (const auto &from : fromLinks) {
        auto fromNum = from.first.first;
        auto fromParaLine = from.first.second;
        LineNumber fromLn(fromParaLine.first, fromParaLine.second);
        auto detail = from.second;
        linkDetailOutfile << FROM_LINK_STARTER << detail.key
                          << FROM_LINK_KEY_SEPARATOR << referFilePrefix
                          << getFileNameFromAttachmentNumber(referFilePrefix,
                                                             fromNum)
                          << HTML_SUFFIX << referParaMiddleChar
                          << fromLn.asString() << FROM_LINK_STRING_SEPARATOR
                          << detail.link << endl;
        keySet.insert(detail.key);
      }
      for (const auto &key : keySet)
        keyDetailOutfile << key << displaySpace;
      keyDetailOutfile << endl;
    }
    FUNCTION_OUTPUT << "links information are written into: "
                    << linkDetailFilePath << " and " << keyDetailFilePath
                    << endl;
  }
  if (not imageLinksTable.empty()) {
    FUNCTION_OUTPUT << imageLinkDetailFilePath << " is created." << endl;
    ofstream linkDetailOutfile(imageLinkDetailFilePath);

    for (const auto &element : imageLinksTable) {
      auto num = element.first.first;
      auto links = element.second;
      auto numOfLinks = links.size();
      linkDetailOutfile << getFileNameFromAttachmentNumber(referFilePrefix, num)
                        << FieldSeparator << numOfLinks << FieldSeparator;
      for (const auto &link : links) {
        linkDetailOutfile << link << FieldSeparator;
      }
      linkDetailOutfile << endl;
    }
    FUNCTION_OUTPUT << "image links information are written into: "
                    << imageLinkDetailFilePath << endl;
  }
}

LinkStringSet CoupledLink::loadImagesLinksFromStatisticsFile() {
  LinkStringSet result;
  ifstream infile(imageLinkDetailFilePath);
  if (not fileExist(infile, imageLinkDetailFilePath))
    return result;
  while (!infile.eof()) {
    string inLine;
    getline(infile, inLine);
    auto targetFile =
        getIncludedStringBetweenTags(inLine, emptyString, FieldSeparator);
    if (targetFile.empty())
      break;
    auto numOfLinks =
        getIncludedStringBetweenTags(inLine, FieldSeparator, FieldSeparator);
    auto linkStart = inLine.find(numOfLinks) + numOfLinks.length() - 1;
    for (auto i = 0; i < TurnToInt(numOfLinks); i++) {
      auto fullPos =
          make_pair(make_pair(TurnToInt(targetFile), 0), make_pair(0, i));
      auto link = getIncludedStringBetweenTags(inLine, FieldSeparator,
                                               FieldSeparator, linkStart);
      result[fullPos] = link;
      linkStart += link.length() + FieldSeparator.length();
    }
  }
  return result;
}

void CoupledLink::loadLinkTableFromStatisticsFile() {
  linksTable.clear();
  ifstream infile(linkDetailFilePath);
  if (not fileExist(infile, linkDetailFilePath))
    return;

  string inLine;
  LinkDetailSet entry;
  AttachmentNumber num;
  ParaLineNumber paraLine;
  AttachmentNumber fromNum;
  ParaLineNumber fromParaLine;
  bool firstLink = true;
  while (!infile.eof()) {
    getline(infile, inLine);
    if (inLine.find(LINK_TARGET_STARTER) != string::npos) {
      if (firstLink)
        firstLink = false;
      else
        // output last link
        linksTable[make_pair(num, paraLine)] = entry;
      // update num paraLine
      auto targetFile = getIncludedStringBetweenTags(
          inLine, LINK_TARGET_STARTER + referFilePrefix, HTML_SUFFIX);
      auto targetLine = getIncludedStringBetweenTags(
          inLine, referParaMiddleChar, emptyString);
      num = getAttachmentNumber(targetFile);
      LineNumber ln(targetLine);
      paraLine = ln.getParaLineNumber();
      // clear entry
      entry.clear();
    }
    if (inLine.find(FROM_LINK_STARTER) != string::npos) {
      auto key = getIncludedStringBetweenTags(inLine, FROM_LINK_STARTER,
                                              FROM_LINK_KEY_SEPARATOR);
      auto fromFile = getIncludedStringBetweenTags(
          inLine, FROM_LINK_KEY_SEPARATOR + referFilePrefix, HTML_SUFFIX);
      auto fromLine = getIncludedStringBetweenTags(inLine, referParaMiddleChar,
                                                   FROM_LINK_STRING_SEPARATOR);
      fromNum = getAttachmentNumber(fromFile);
      LineNumber ln(fromLine);
      fromParaLine = ln.getParaLineNumber();
      auto link = getIncludedStringBetweenTags(
          inLine, FROM_LINK_STRING_SEPARATOR, emptyString);
      LinkDetails detail{key, link};
      entry[make_pair(fromNum, fromParaLine)] = detail;
    }
  }
  // output last link
  if (not firstLink)
    linksTable[make_pair(num, paraLine)] = entry;
  // verify loading success by writing into another file
  //	  linkDetailFilePath = "utf8HTML/output/LinksFromMain1.txt";
  //	  CoupledLink::outPutStatisticsToFiles();
}

CoupledLink::LinkDetailSet
CoupledLink::getLinkDetailSet(AttachmentNumber num, ParaLineNumber paraLine) {
  LinkDetailSet entry;
  try {
    entry = linksTable.at(make_pair(num, paraLine));
  } catch (exception &) {
  }
  return entry;
}

static const string HTML_OUTPUT_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/LinksFromMain.txt";
static const string HTML_OUTPUT_IMAGE_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/ImageLinksFromMain.txt";
static const string HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/KeysOfLinksFromMain.txt";

void LinkFromMain::setupStatisticsParameters() {
  referFilePrefix = MAIN_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_MAIN_LIST;
  imageLinkDetailFilePath = HTML_OUTPUT_IMAGE_LINKS_FROM_MAIN_LIST;
  keyDetailFilePath = HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST;
}

/**
 * output statistics from link fixing of links from main files
 */
void LinkFromMain::outPutStatisticsToFiles() {
  setupStatisticsParameters();
  CoupledLink::outPutStatisticsToFiles();
}

void LinkFromMain::loadLinkTableFromFile() {
  setupStatisticsParameters();
  loadLinkTableFromStatisticsFile();
}

string shrinkImageLinkAnnotation(const string &imageLink) {
  string link = imageLink;
  replacePart(link, ImageAnnotationStartChars, emptyString);
  replacePart(link, bracketEndChars, emptyString);
  return link;
}

void LinkFromMain::logLink() {

  AttachmentNumber fromNum = getAttachmentNumber(m_fromFile);
  ParaLineNumber fromParaLine = m_fromLine.getParaLineNumber();
  if (isTargetToOtherMainHtm()) {
    auto num = make_pair(getchapterNumer(), getattachmentNumber());
    LinkDetails detail{m_usedKey, asString(true)};
    LineNumber ln(m_referPara);
    ParaLineNumber paraLine = ln.getParaLineNumber();
    try {
      auto &entry = linksTable.at(make_pair(num, paraLine));
      entry[make_pair(fromNum, fromParaLine)] = detail;
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "entry.size: " << entry.size()
                      << " more reference to link: " << getChapterName()
                      << displaySpace << m_referPara << displaySpace
                      << m_usedKey << endl;
    } catch (exception &) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "create map for : " << getChapterName() << displaySpace
                      << m_referPara << endl;
      LinkDetailSet entry;
      entry[make_pair(fromNum, fromParaLine)] = detail;
      linksTable[make_pair(num, paraLine)] = entry;
    }
  }
  if (isTargetToOtherAttachmentHtm()) {
    auto num = make_pair(getchapterNumer(), getattachmentNumber());
    auto targetFile = getHtmlFileNamePrefix() + getChapterName() +
                      attachmentFileMiddleChar +
                      TurnToString(getattachmentNumber());
    auto title = AttachmentList::getAttachmentTitleFromFile(num);
    auto type = attachmentTable.getAttachmentType(num);
    if (getSourceChapterName() == getChapterName()) {
      AttachmentDetails detail{targetFile, m_fromLine.asString(), title,
                               (type == ATTACHMENT_TYPE::PERSONAL)
                                   ? ATTACHMENT_TYPE::PERSONAL
                                   : ATTACHMENT_TYPE::REFERENCE};
      attachmentTable.addOrUpdateOneItem(
          getAttachmentNumber(targetFile, ATTACHMENT_HTML_PREFIX), detail);
    }
    if (not AttachmentList::isTitleMatch(getAnnotation(), title)) {
      METHOD_OUTPUT << m_fromFile << " has a link to " << targetFile
                    << " with annotation: " << getAnnotation()
                    << " differs from title: " << title << endl;
    }
  }
  if (isTargetToImage()) {
    // ignore fromParaLine
    ParaLineNumber paraLine{0, 0};
    string link = shrinkImageLinkAnnotation(asString(true));
    try {
      auto &entry = imageLinksTable.at(make_pair(fromNum, paraLine));
      entry.push_back(link);
    } catch (exception &) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "create vector for image link to: " << getAnnotation()
                      << endl;
      vector<string> entry;
      entry.push_back(link);
      imageLinksTable[make_pair(fromNum, paraLine)] = entry;
    }
  }
}

static const string HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/LinksFromAttachment.txt";
static const string HTML_OUTPUT_IMAGE_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/ImageLinksFromAttachment.txt";
static const string HTML_OUTPUT_KEY_OF_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/KeysOfLinksFromAttachment.txt";

/**
 * reset statistics data structure for re-do it during link fixing
 */
void LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList() {}

void LinkFromAttachment::setupStatisticsParameters() {
  referFilePrefix = ATTACHMENT_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST;
  imageLinkDetailFilePath = HTML_OUTPUT_IMAGE_LINKS_FROM_ATTACHMENT_LIST;
  keyDetailFilePath = HTML_OUTPUT_KEY_OF_LINKS_FROM_ATTACHMENT_LIST;
}
/**
 * output statistics from link fixing of links from attachment files
 */
void LinkFromAttachment::outPutStatisticsToFiles() {
  setupStatisticsParameters();
  CoupledLink::outPutStatisticsToFiles();
}

/**
 * log a valid link found by putting it into linksTable
 * which is sorted by ChapterName+attachmentNumber, referPara, usedKey
 */
void LinkFromAttachment::logLink() {
  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{m_usedKey, asString()};
    AttachmentNumber num = make_pair(m_chapterNumber, m_attachmentNumber);
    LineNumber ln(m_referPara);
    ParaLineNumber paraLine = ln.getParaLineNumber();
    AttachmentNumber fromNum = getAttachmentNumber(m_fromFile);
    ParaLineNumber fromParaLine = m_fromLine.getParaLineNumber();
    try {
      auto &entry = linksTable.at(make_pair(num, paraLine));
      entry[make_pair(fromNum, fromParaLine)] = detail;
    } catch (exception &) {
      if (debug >= LOG_EXCEPTION)
        METHOD_OUTPUT << "not found link for: "
                      << getChapterName() + attachmentFileMiddleChar +
                             TurnToString(m_attachmentNumber)
                      << displaySpace << m_referPara << displaySpace
                      << m_usedKey << endl;
      LinkDetailSet entry;
      entry[make_pair(fromNum, fromParaLine)] = detail;
      linksTable[make_pair(num, paraLine)] = entry;
    }
  }
}

void LinkFromJPM::logLink() {}
