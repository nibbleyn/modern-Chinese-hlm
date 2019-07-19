#include "coupledLink.hpp"

CoupledLink::LinksTable CoupledLink::linksTable;
string CoupledLink::referFilePrefix{emptyString};
string CoupledLink::linkDetailFilePath{emptyString};
string CoupledLink::keyDetailFilePath{emptyString};

AttachmentList LinkFromMain::attachmentTable;

static const string LINK_TARGET_STARTER = R"(link: )";
static const string FROM_LINK_STARTER = R"(key: )";
static const string FROM_LINK_KEY_SEPARATOR = R"(, linked from: )";
static const string FROM_LINK_STRING_SEPARATOR = R"( : )";

/**
 * reset the general data structure to log info about links
 */
void CoupledLink::clearLinkTable() { linksTable.clear(); }

/**
 * output links to specified file
 * before calling this function, specifying output file
 */
void CoupledLink::outPutStatisticsToFiles() {
  if (linksTable.empty())
    return;
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
                  << linkDetailFilePath << " and " << keyDetailFilePath << endl;
}

void CoupledLink::loadLinkTableFromStatisticsFile() {
  linksTable.clear();
  ifstream infile(linkDetailFilePath);
  if (!infile) {
    FUNCTION_OUTPUT << ERROR_FILE_NOT_EXIST << linkDetailFilePath << endl;
    return;
  }

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

static const string HTML_OUTPUT_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/LinksFromMain.txt";
static const string HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/KeysOfLinksFromMain.txt";

void LinkFromMain::setupStatisticsParameters() {
  referFilePrefix = MAIN_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_MAIN_LIST;
  keyDetailFilePath = HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST;
}

/**
 * output statistics from link fixing of links from main files
 */
void LinkFromMain::outPutStatisticsToFiles() {
  setupStatisticsParameters();
  CoupledLink::outPutStatisticsToFiles();
}

void LinkFromMain::logLink() {
  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{m_usedKey, asString(true)};
    AttachmentNumber num = make_pair(m_chapterNumber, 0);
    LineNumber ln(m_referPara);
    ParaLineNumber paraLine = ln.getParaLineNumber();

    AttachmentNumber fromNum = getAttachmentNumber(m_fromFile);
    ParaLineNumber fromParaLine = m_fromLine.getParaLineNumber();
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
        METHOD_OUTPUT << "create vector for : " << getChapterName()
                      << displaySpace << m_referPara << endl;
      LinkDetailSet entry;
      entry[make_pair(fromNum, fromParaLine)] = detail;
      linksTable[make_pair(num, paraLine)] = entry;
    }
  }
  if (isTargetToOtherAttachmentHtm()) {
    auto targetFile = getHtmlFileNamePrefix() + getChapterName() +
                      attachmentFileMiddleChar +
                      TurnToString(getattachmentNumber());
    auto num = make_pair(getchapterNumer(), getattachmentNumber());
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
}

static const string HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/LinksFromAttachment.txt";
static const string HTML_OUTPUT_KEY_OF_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/KeysOfLinksFromAttachment.txt";

/**
 * reset statistics data structure for re-do it during link fixing
 */
void LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList() {}

void LinkFromAttachment::setupStatisticsParameters() {
  referFilePrefix = ATTACHMENT_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST;
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
