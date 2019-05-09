#include "attachmentTable.hpp"

extern int debug;

static const string attachmentNotExisted = R"(file doesn't exist.)";
static const string titleNotExisted = R"(title doesn't exist.)";

string AttachmentList::getAttachmentTitleFromFile(AttachmentNumber num) {
  string inputFile =
      HTML_SRC_ATTACHMENT + ATTACHMENT_TYPE_HTML_TARGET +
      getChapterNameByTargetKind(MAIN_TYPE_HTML_TARGET, num.first) +
      attachmentFileMiddleChar + TurnToString(num.second) + HTML_SUFFIX;
  ifstream infile(inputFile);
  if (!infile) {
    return attachmentNotExisted;
  }
  string inLine{""};
  while (!infile.eof()) {
    getline(infile, inLine);
    if (inLine.find(htmlTitleStart) != string::npos) {
      return getIncludedStringBetweenTags(inLine, htmlTitleStart, htmlTitleEnd);
    }
    if (inLine.find(endOfHtmlHead) != string::npos)
      return titleNotExisted;
  }
  return titleNotExisted;
}

bool AttachmentList::isTitleMatch(string annotation, string title) {
  if (title == attachmentNotExisted or title == titleNotExisted)
    return true;
  return (annotation == title);
}

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

static const string fromParaOfReferenceAttachment = R"(from:)";
static const string fromFileOfReferenceAttachment = R"( name:)";
static const string titleOfReferenceAttachment = R"( title:)";
static const string typeOfReferenceAttachment = R"( type:)";

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
void AttachmentList::loadReferenceAttachmentList() {
  ifstream infile(m_sourceFile);
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << m_sourceFile << endl;
    return;
  }
  m_table.clear();
  string line;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    getline(infile, line);
    if (line.empty())
      break;

    string type = referenceAttachmentType;
    auto typeBegin = line.find(typeOfReferenceAttachment);
    if (typeBegin != string::npos) {
      type = line.substr(typeBegin + typeOfReferenceAttachment.length());
    }
    string title = getIncludedStringBetweenTags(
        line, titleOfReferenceAttachment, typeOfReferenceAttachment);
    string targetFile = getIncludedStringBetweenTags(
        line, fromFileOfReferenceAttachment, titleOfReferenceAttachment);
    string fromLine = getIncludedStringBetweenTags(
        line, fromParaOfReferenceAttachment, fromFileOfReferenceAttachment);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << fromParaOfReferenceAttachment << fromLine
                    << fromFileOfReferenceAttachment << targetFile
                    << titleOfReferenceAttachment << title
                    << typeOfReferenceAttachment << type << endl;
    type.erase(remove(type.begin(), type.end(), ' '), type.end());
    // remove only leading and trailing blank for title
    title = regex_replace(title, regex("^ +"), emptyString);
    title = regex_replace(title, regex(" +$"), emptyString);
    AttachmentDetails detail{targetFile, fromLine, title,
                             attachmentTypeFromString(type)};
    auto num = getAttachmentNumber(targetFile);
    m_table[num] = detail;
    m_notUpdatedAttachmentSet.insert(num);
  }
  m_newlyAddedAttachmentSet.clear();
}

void AttachmentList::addOrUpdateOneItem(AttachmentNumber num,
                                        AttachmentDetails &detail) {
  if (getAttachmentType(num) == ATTACHMENT_TYPE::NON_EXISTED)
    m_newlyAddedAttachmentSet.insert(num);
  else
    m_notUpdatedAttachmentSet.erase(num);

  m_table[num] = detail;
}

/**
 * output all attachment info into specified file
 */
void AttachmentList::saveAttachmentList() {
  if (m_table.empty())
    return;
  // remove not used attachments
  for (const auto &element : m_notUpdatedAttachmentSet) {
    m_table.erase(element);
  }
  ofstream outfile(m_outputFile);
  for (const auto &attachment : m_table) {
    auto entry = attachment.second;

    outfile << fromParaOfReferenceAttachment << entry.fromLine
            << fromFileOfReferenceAttachment << entry.fromfilename
            << titleOfReferenceAttachment << entry.title
            << typeOfReferenceAttachment << attachmentTypeAsString(entry.type)
            << endl;
  }
  METHOD_OUTPUT << "attachment information are written into: " << m_outputFile
                << endl;
}

/**
 * find fromLine of one attachment from attachmentTable
 * which is calculated during link fixing
 * @param num pair of chapter number and attachment number
 * @return the fromLine stored if existed, otherwise "top"
 */
string AttachmentList::getFromLineOfAttachment(AttachmentNumber num) {
  string result = topParagraphIndicator;
  try {
    result = m_table.at(num).fromLine;
  } catch (exception &) {
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "fromLine not found in attachmentTable about: "
                    << num.first << attachmentFileMiddleChar << num.second
                    << endl;
  }
  return result;
}

/**
 * find fromLine of one attachment from attachmentTable
 * which is calculated during link fixing
 * @param num pair of chapter number and attachment number
 * @return the fromLine stored if existed, otherwise "top"
 */
string AttachmentList::getAttachmentTitle(AttachmentNumber num) {
  string result = topParagraphIndicator;
  try {
    result = m_table.at(num).title;
  } catch (exception &) {
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "fromLine not found in attachmentTable about: "
                    << num.first << attachmentFileMiddleChar << num.second
                    << endl;
  }
  return result;
}

ATTACHMENT_TYPE AttachmentList::getAttachmentType(AttachmentNumber num) {
  ATTACHMENT_TYPE attachmentType = ATTACHMENT_TYPE::NON_EXISTED;
  try {
    auto entry = m_table.at(num);
    attachmentType = entry.type;
  } catch (exception &) {
    if (debug >= LOG_EXCEPTION)
      METHOD_OUTPUT << "not found info in refAttachmentTable about: "
                    << num.first << attachmentFileMiddleChar << num.second
                    << endl;
  }
  return attachmentType;
}

void AttachmentList::displayNewlyAddedAttachments() {
  if (m_newlyAddedAttachmentSet.empty())
    return;
  METHOD_OUTPUT << "Newly Added Attachments:" << endl;
  for (const auto &num : m_newlyAddedAttachmentSet) {
    METHOD_OUTPUT << num.first << attachmentFileMiddleChar << num.second
                  << HTML_SUFFIX << endl;
  }
}

LinkStringSet
AttachmentList::allAttachmentsAsLinksByType(ATTACHMENT_TYPE type) {
  LinkStringSet result;
  for (const auto &attachment : m_table) {
    auto num = attachment.first;
    auto fullPos = make_pair(num, make_pair(0, 0));
    auto entry = attachment.second;
    ATTACHMENT_TYPE attachmentType = entry.type;

    if (attachmentType == type) {
      string name = citationChapterNo + TurnToString(num.first) + defaultUnit +
                    citationChapterNo + TurnToString(num.second) +
                    attachmentUnit;
      result[fullPos] = fixLinkFromAttachmentTemplate(
          attachmentDirForLinkFromMain,
          getChapterNameByTargetKind(MAIN_TYPE_HTML_TARGET, num.first),
          TurnToString(num.second), name + displaySpace + entry.title);
    }
  }
  return result;
}
