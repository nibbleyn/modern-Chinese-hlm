#include "attachmentTable.hpp"

extern int debug;

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

static const string ATTACHMENT_TYPE_HTML_TARGET = R"(b0)";

/**
 * get chapter number and attachment number from an attachment file name
 * for example with input b001_15 would return pair <1,15>
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return pair of chapter number and attachment number
 */
AttachmentNumber getAttachmentNumber(const string &filename) {
  AttachmentNumber num(0, 0);
  // referred file not found
  if (filename.find(ATTACHMENT_TYPE_HTML_TARGET) == string::npos) {
    return num;
  }
  num.first = TurnToInt(getIncludedStringBetweenTags(
      filename, ATTACHMENT_TYPE_HTML_TARGET, attachmentFileMiddleChar));
  if (filename.find(attachmentFileMiddleChar) == string::npos) {
    return num;
  }
  num.second =
      TurnToInt(filename.substr(filename.find(attachmentFileMiddleChar) +
                                attachmentFileMiddleChar.length()));
  return num;
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
    FUNCTION_OUTPUT << "file doesn't exist:" << m_sourceFile << endl;
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
      FUNCTION_OUTPUT << fromParaOfReferenceAttachment << fromLine
                      << fromFileOfReferenceAttachment << targetFile
                      << titleOfReferenceAttachment << title
                      << typeOfReferenceAttachment << type << endl;
    type.erase(remove(type.begin(), type.end(), ' '), type.end());
    // remove only leading and trailing blank for title
    title = std::regex_replace(title, std::regex("^ +"), emptyString);
    title = std::regex_replace(title, std::regex(" +$"), emptyString);
    AttachmentDetails detail{targetFile, fromLine, title,
                             attachmentTypeFromString(type)};
    addOrUpdateOneItem(getAttachmentNumber(targetFile), detail);
  }
}

void AttachmentList::addOrUpdateOneItem(AttachmentNumber num,
                                        AttachmentDetails &detail) {
  m_table[num] = detail;
}

/**
 * output all attachment info into specified file
 */
void AttachmentList::saveAttachmentList() {
  if (m_table.empty())
    return;
  ofstream outfile(m_outputFile);
  for (const auto &attachment : m_table) {
    auto entry = attachment.second;

    outfile << fromParaOfReferenceAttachment << entry.fromLine
            << fromFileOfReferenceAttachment << entry.fromfilename
            << titleOfReferenceAttachment << entry.title
            << typeOfReferenceAttachment << attachmentTypeAsString(entry.type)
            << endl;
  }
  FUNCTION_OUTPUT << "attachment information are written into: " << m_outputFile
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
      FUNCTION_OUTPUT << "fromLine not found in attachmentTable about: "
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
      FUNCTION_OUTPUT << "not found info in refAttachmentTable about: "
                      << num.first << attachmentFileMiddleChar << num.second
                      << endl;
  }
  return attachmentType;
}
