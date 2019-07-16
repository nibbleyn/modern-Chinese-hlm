#include "fileUtil.hpp"

FILE_TYPE getFileTypeFromKind(const std::string &kind) {
  if (kind == ATTACHMENT)
    return FILE_TYPE::ATTACHMENT;
  if (kind == ORIGINAL)
    return FILE_TYPE::ORIGINAL;
  if (kind == JPM)
    return FILE_TYPE::JPM;
  return FILE_TYPE::MAIN;
}

/**
 * from the type of file to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of file
 * @return filename prefix of target file
 */

std::string getHtmlFileNamePrefixFromFileType(FILE_TYPE type) {
  if (type == FILE_TYPE::ORIGINAL)
    return ORIGINAL_HTML_PREFIX;
  if (type == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_HTML_PREFIX;
  if (type == FILE_TYPE::JPM)
    return JPM_HTML_PREFIX;
  return MAIN_HTML_PREFIX;
}

std::string getBodyTextFilePrefixFromFileType(FILE_TYPE type) {
  if (type == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_BODYTEXT_PREFIX;
  if (type == FILE_TYPE::ORIGINAL)
    return ORIGINAL_BODYTEXT_PREFIX;
  if (type == FILE_TYPE::JPM)
    return JPM_BODYTEXT_PREFIX;
  return MAIN_BODYTEXT_PREFIX;
}

static constexpr const char *ORIGINAL_SEPERATOR_COLOR = R"(004040)";

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

std::string getHtmlFileNameFromBodyTextFilePrefix(const std::string &prefix) {
  if (prefix == ORIGINAL_BODYTEXT_PREFIX)
    return ORIGINAL_HTML_PREFIX;
  if (prefix == ATTACHMENT_BODYTEXT_PREFIX)
    return ATTACHMENT_HTML_PREFIX;
  if (prefix == JPM_BODYTEXT_PREFIX)
    return JPM_HTML_PREFIX;
  return MAIN_HTML_PREFIX;
}

static constexpr const char *HTML_OUTPUT_LINES_OF_MAIN =
    R"(utf8HTML/output/LinesOfMain.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_JPM =
    R"(utf8HTML/output/LinesOfJPM.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_ORIGINAL =
    R"(utf8HTML/output/LinesOfOriginal.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_ATTACHMENTS =
    R"(utf8HTML/output/LinesOfAttachments.txt)";

std::string getStatisticsOutputFilePathFromString(const std::string &fileType) {
  if (fileType == ATTACHMENT)
    return HTML_OUTPUT_LINES_OF_ATTACHMENTS;
  if (fileType == ORIGINAL)
    return HTML_OUTPUT_LINES_OF_ORIGINAL;
  if (fileType == JPM)
    return HTML_OUTPUT_LINES_OF_JPM;
  return HTML_OUTPUT_LINES_OF_MAIN;
}
