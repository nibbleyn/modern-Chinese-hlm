#include "attachmentFiles.hpp"

/**
 * get chapter number and attachment number from an attachment file name
 * for example with input b001_15 would return pair <1,15>
 * @param filename the attachment file without .htm, e.g. b003_7
 * @return pair of chapter number and attachment number
 */
AttachmentNumber getAttachmentNumber(const string &filename) {
  AttachmentNumber num(0, 0);
  string start = getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT);
  auto fileBegin = filename.find(start);
  if (fileBegin == string::npos) // referred file not found
  {
    return num;
  }
  auto chapter = filename.substr(fileBegin + start.length(), 2);
  num.first = TurnToInt(chapter);
  auto seqStart = filename.find(attachmentFileMiddleChar);
  if (seqStart == string::npos) // no file to refer
  {
    return num;
  }
  auto seq = filename.substr(seqStart + 1, filename.length() - seqStart);
  num.second = TurnToInt(seq);
  return num;
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
  string start = htmlTitleStart;
  string end = htmlTitleEnd;
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
    if (inLine.find(endOfHtmlHead) != string::npos)
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
    if (file.find(getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile) !=
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
