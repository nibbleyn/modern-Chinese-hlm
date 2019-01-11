#include "fileUtil.hpp"

/**
 * from the type of file to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of file
 * @return filename prefix of target file
 */

string getFileNamePrefixFromFileType(FILE_TYPE type) {
  string filenamePrefix[] = {"a0", "b0", "c0"};
  if (type == FILE_TYPE::MAIN)
    return filenamePrefix[0];
  if (type == FILE_TYPE::ATTACHMENT)
    return filenamePrefix[1];
  if (type == FILE_TYPE::ORIGINAL)
    return filenamePrefix[2];
  return "unsupported";
}

/**
 * from the type of link to get the filename prefix of bodytext file
 * all main bodytext files are MainXX.txt
 * all attachment bodytext files are AttachXX.txt
 * all original bodytext files are OrgXX.txt
 * never get called for type SAMEPAGE
 * @param type type of file
 * @return filename prefix of bodytext file
 */
string getBodyTextFilePrefixFromFileType(FILE_TYPE type) {
  string bodyTextFilePrefix[] = {"Main", "Attach", "Org"};
  if (type == FILE_TYPE::MAIN)
    return bodyTextFilePrefix[0];
  if (type == FILE_TYPE::ATTACHMENT)
    return bodyTextFilePrefix[1];
  if (type == FILE_TYPE::ORIGINAL)
    return bodyTextFilePrefix[2];
  return "unsupported";
}

FILE_TYPE getFileTypeFromString(const string &fileType) {
  if (fileType == "attachment")
    return FILE_TYPE::ATTACHMENT;
  if (fileType == "original")
    return FILE_TYPE::ORIGINAL;
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
  string bodyTextFilePrefix[] = {"Main", "Attach", "Org"};
  if (type == FILE_TYPE::MAIN)
    return "F0BEC0";
  if (type == FILE_TYPE::ATTACHMENT)
    return "F0BEC0";
  if (type == FILE_TYPE::ORIGINAL)
    return "004040";
  return "unsupported";
}
