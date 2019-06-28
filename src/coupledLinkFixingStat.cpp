#include "coupledLink.hpp"

CoupledLink::LinksTable CoupledLink::linksTable;
string CoupledLink::referFilePrefix{emptyString};
string CoupledLink::linkDetailFilePath{emptyString};
string CoupledLink::keyDetailFilePath{emptyString};

AttachmentList LinkFromMain::attachmentTable;

/**
 * output linksTable to file specified in linkDetailFilePath and
 * keyDetailFilePath
 */
void CoupledLink::displayFixedLinks() {
  if (linksTable.empty())
    return;
  FUNCTION_OUTPUT << linkDetailFilePath << " is created." << endl;
  FUNCTION_OUTPUT << keyDetailFilePath << " is created." << endl;
  ofstream linkDetailOutfile(linkDetailFilePath);
  ofstream keyDetailOutfile(keyDetailFilePath);
  linkDetailOutfile << "all links processed:" << endl;
  linkDetailOutfile << "---------------------------------" << endl;
  for (const auto &link : linksTable) {
    auto target = link.first;
    auto fromList = link.second;
    // link itself
    linkDetailOutfile << "link:" << referFilePrefix << target.first << ","
                      << target.second << endl;
    keyDetailOutfile << referFilePrefix << target.first << ":" << target.second
                     << endl;
    using KeySet = set<string>;
    KeySet keySet;
    sort(fromList.begin(), fromList.end(),
         [](LinkDetails a, LinkDetails b) { return a.key < b.key; });
    for (const auto &from : fromList) {
      linkDetailOutfile << "key: " << from.key
                        << ",linked from:" << from.fromFile << ","
                        << from.fromLine << ":" << endl
                        << "    " << from.link << endl;
      keySet.insert(from.key);
    }
    for (const auto &key : keySet)
      keyDetailOutfile << key << " ";
    keyDetailOutfile << endl;
  }
}

/**
 * reset the general data structure to log info about links
 */
void CoupledLink::clearLinkTable() { linksTable.clear(); }

/**
 * output links to specified file
 * before calling this function, specifying output file
 */
void CoupledLink::outPutStatisticsToFiles() {
  displayFixedLinks();
  FUNCTION_OUTPUT << "links information are written into: "
                  << linkDetailFilePath << " and " << keyDetailFilePath << endl;
}

static const string HTML_OUTPUT_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/LinksFromMain.txt";
static const string HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/KeysOfLinksFromMain.txt";

/**
 * output statistics from link fixing of links from main files
 */
void LinkFromMain::outPutStatisticsToFiles() {
  referFilePrefix = MAIN_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_MAIN_LIST;
  keyDetailFilePath = HTML_OUTPUT_KEY_OF_LINKS_FROM_MAIN_LIST;
  CoupledLink::outPutStatisticsToFiles();
}

void LinkFromMain::logLink() {

  if (isTargetToOtherMainHtm()) {
    LinkDetails detail{m_usedKey, m_fromFile, m_fromLine.asString(),
                       asString()};
    try {
      auto &entry = linksTable.at(make_pair(getChapterName(), m_referPara));
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
      linksTable[make_pair(getChapterName(), m_referPara)] = list;
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
      attachmentTable.addOrUpdateOneItem(getAttachmentNumber(targetFile),
                                         detail);
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

/**
 * output statistics from link fixing of links from attachment files
 */
void LinkFromAttachment::outPutStatisticsToFiles() {
  referFilePrefix = ATTACHMENT_HTML_PREFIX;
  linkDetailFilePath = HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST;
  keyDetailFilePath = HTML_OUTPUT_KEY_OF_LINKS_FROM_ATTACHMENT_LIST;
  CoupledLink::outPutStatisticsToFiles();
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
      auto &entry =
          linksTable.at(make_pair(getChapterName() + attachmentFileMiddleChar +
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
      linksTable[make_pair(getChapterName() + attachmentFileMiddleChar +
                               TurnToString(m_attachmentNumber),
                           m_referPara)] = list;
    }
  }
}

void LinkFromJPM::logLink() {}
