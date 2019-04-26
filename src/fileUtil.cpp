#include "fileUtil.hpp"

static const std::string attachmentNotExisted = R"(file doesn't exist.)";
static const std::string titleNotExisted = R"(title doesn't exist.)";

/**
 * find in <title>xxx</title> part of attachment file header
 * the title of the attachment
 * if the file or the title is not found
 * return corresponding error message as one of below,
 * file doesn't exist.
 * title doesn't exist.
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return the title found or error message
 */
std::string getAttachmentTitle(const std::string &filename) {
  std::string inputFile = HTML_SRC_ATTACHMENT + filename + HTML_SUFFIX;
  std::ifstream infile(inputFile);
  if (!infile) {
    return attachmentNotExisted;
  }
  std::string inLine{""};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto beginPos = inLine.find(htmlTitleStart);
    if (beginPos != std::string::npos) {
      auto endPos =
          inLine.find(htmlTitleEnd, beginPos + htmlTitleStart.length());
      if (endPos == std::string::npos)
        return titleNotExisted;
      return inLine.substr(beginPos + htmlTitleStart.length(),
                           endPos - htmlTitleStart.length() - beginPos);
    }
    if (inLine.find(endOfHtmlHead) != std::string::npos)
      return titleNotExisted;
  }
  return titleNotExisted;
}

/**
 * from the type of file to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of file
 * @return filename prefix of target file
 */

std::string getHtmlFileNamePrefix(FILE_TYPE type) {
  if (type == FILE_TYPE::ORIGINAL)
    return ORIGINAL_HTML_PREFIX;
  if (type == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_HTML_PREFIX;
  if (type == FILE_TYPE::JPM)
    return JPM_HTML_PREFIX;
  return MAIN_HTML_PREFIX;
}

FILE_TYPE getFileTypeFromString(const std::string &fileType) {
  if (fileType == ATTACHMENT)
    return FILE_TYPE::ATTACHMENT;
  if (fileType == ORIGINAL)
    return FILE_TYPE::ORIGINAL;
  if (fileType == JPM)
    return FILE_TYPE::JPM;
  return FILE_TYPE::MAIN;
}
/**
 * the background of original file is different from main file
 * and so their separator line between paragraphs
 * this returns right color of separator line
 * when numbering different files
 * @param type type of file
 * @return corresponding color of separator line between paragraphs
 */
std::string getSeparateLineColor(FILE_TYPE type) {
  if (type == FILE_TYPE::MAIN)
    return MAIN_SEPERATOR_COLOR;
  if (type == FILE_TYPE::JPM)
    return MAIN_SEPERATOR_COLOR;
  if (type == FILE_TYPE::ATTACHMENT)
    return MAIN_SEPERATOR_COLOR;
  if (type == FILE_TYPE::ORIGINAL)
    return ORIGINAL_SEPERATOR_COLOR;
  return "unsupported";
}
