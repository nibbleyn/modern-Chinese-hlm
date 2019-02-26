#include "fileUtil.hpp"

/**
 * from the type of file to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of file
 * @return filename prefix of target file
 */

string getHtmlFileNamePrefix(FILE_TYPE type) {
  if (type == FILE_TYPE::ORIGINAL)
    return ORIGINAL_HTML_PREFIX;
  if (type == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_HTML_PREFIX;
  if (type == FILE_TYPE::JPM)
    return JPM_HTML_PREFIX;
  return MAIN_HTML_PREFIX;
}

FILE_TYPE getFileTypeFromString(const string &fileType) {
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
string getSeparateLineColor(FILE_TYPE type) {
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
