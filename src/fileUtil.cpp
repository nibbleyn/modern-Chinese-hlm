#include "fileUtil.hpp"

static const string endOfHead = R"(/head)";
static const string titleStart = R"(<title>)";
static const string titleEnd = R"(</title>)";

/**
 * all file names like a077.htm, b003_8.htm
 * have a 2-digit string part "77", "03"
 * specifying chapter 77,3 etc.
 * this function return a 2-digit string from chapter number
 * for example, "07" from chapter 7
 * @param chapterNumber the index of the chapter
 * @return 2-digit string of chapter number
 */
string formatIntoTwoDigitChapterNumber(int chapterNumber) {
  stringstream formatedNumber;
  formatedNumber.fill('0');
  formatedNumber.width(2);
  formatedNumber.clear();
  formatedNumber << internal << chapterNumber;
  return formatedNumber.str();
}

/**
 * generate a set of strings
 * containing 2-digit string of chapters
 * starting from min
 * and ending in max
 * @param min the smallest chapter
 * @param max the largest chapter
 * @return a set of strings in fileSet
 */
fileSet buildFileSet(int min, int max) {
  fileSet fs;
  for (int i = min; i <= std::min(99, max); i++) {
    fs.insert(formatIntoTwoDigitChapterNumber(i));
  }
  for (int i = 100; i <= max; i++) {
    fs.insert(TurnToString(i));
  }
  return fs;
}

/**
 * from the type of file to get the filename prefix of target file
 * all main files are a0XX.htm
 * all attachment files are b0XX.htm
 * all original files are c0XX.htm
 * @param type type of file
 * @return filename prefix of target file
 */

string getFileNamePrefix(FILE_TYPE type) {
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
string getBodyTextFilePrefix(FILE_TYPE type) {
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
string getAttachmentTitle(const string &filename) {
  string inputFile = HTML_SRC_ATTACHMENT + filename + HTML_SUFFIX;
  ifstream infile(inputFile);
  if (!infile) {
    return "file doesn't exist.";
  }
  string inLine{""};
  string start = titleStart;
  string end = titleEnd;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto titleBegin = inLine.find(start);
    if (titleBegin != string::npos) {
      auto titleEnd = inLine.find(end);
      if (titleEnd == string::npos)
        return "title doesn't exist.";
      auto res = inLine.substr(titleBegin + start.length(),
                               titleEnd - titleBegin - start.length());
      return res;
    }
    if (inLine.find(endOfHead) != string::npos)
      return "title doesn't exist.";
  }
  return "title doesn't exist.";
}

/**
 * get all attachment files for referFile under fromDir
 * for example, if there are b003_1.html b003_5.html and b003_15.html
 * this function would return {1,5,15} for referFile "03"
 * @param referFile the 2-digit string part of refer file
 * @param fromDir where its attachment files are under
 * @return the vector of attachment numbers
 */
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir) {
  vector<string> filenameList;
  vector<int> attList;
  Poco::File(fromDir).list(filenameList);
  for (const auto &file : filenameList) {
    if (file.find(getFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile) !=
        string::npos) {
      string start = attachmentFileMiddleChar;
      auto keyBegin = file.find(start);
      auto keyEnd = file.find(".");
      string attNo = file.substr(keyBegin + start.length(),
                                 keyEnd - keyBegin - start.length());
      attList.push_back(TurnToInt(attNo));
    }
  }
  return attList;
}
