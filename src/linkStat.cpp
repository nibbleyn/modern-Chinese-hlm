#include "link.hpp"
#include <regex>

extern fileSet keyMissingChapters;
extern fileSet newAttachmentList;

Link::LinksTable Link::linksTable;
string Link::outPutFilePath{emptyString};
Link::AttachmentSet Link::refAttachmentTable;

LinkFromMain::AttachmentSet LinkFromMain::attachmentTable;

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

static const string HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/LinksFromAttachment.txt";

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
