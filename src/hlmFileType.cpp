#include "hlmFileType.hpp"

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
 * seconds from EPOCH as the timestamp
 * used for unique name in backup etc.
 * @return current timestamp from EPOCH
 */
string currentTimeStamp() {
  return TurnToString(chrono::duration_cast<chrono::milliseconds>(
                          chrono::system_clock::now().time_since_epoch())
                          .count());
}

/**
 * current Date and Time
 * to log for backup time
 * @return current Date and Time
 */
string currentDateTime() {
  time_t t = time(0); // get time now
  tm *now = localtime(&t);

  ostringstream ss_msg;
  ss_msg << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
         << now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << ":"
         << now->tm_sec << "\n";
  return ss_msg.str();
}

/**
 * load files under "to" directory with files under "from"
 * currently only called from BODY_TEXT_FIX to BODY_TEXT_OUTPUT
 * i.e. from afterFix to output
 * for continue link fixing after numbering..
 * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for it
 * @param from the source directory to load files
 * @param to the target directory to load files
 */
void loadBodyTexts(const string &from, const string &to) {
  vector<string> filenameList;
  Poco::File(from).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    if (debug >= LOG_INFO)
      cout << "loading " << file << ".. " << endl;
    Poco::File fileToCopy(from + file);
    fileToCopy.copyTo(to + file);
  }
}

/**
 * to numbering or linkfixing main or attachment files
 * copy them to HTML_OUTPUT_MAIN or HTML_OUTPUT_ATTACHMENT
 * and this function would backup current files
 * (incl. sub-dirs) under HTML_SRC_MAIN
 * and load newly copied files from HTML_OUTPUT_MAIN to HTML_SRC_MAIN
 * then dissemble would happen from HTML_SRC_MAIN afterwards
 */
void backupAndOverwriteSrcForHTML() {
  string BACKUP = "utf8HTML/src" + currentTimeStamp();
  cout << "backup of current src is created under : " << BACKUP << endl;

  Poco::File BackupPath(BACKUP);
  if (!BackupPath.exists())
    BackupPath.createDirectories();

  // backup whole src directory together with files to this directory
  Poco::File dirToCopy(HTML_SRC_MAIN);
  dirToCopy.copyTo(BACKUP);

  // create a date file in this backup directory
  string outputFile = BACKUP + "/info.txt";
  ofstream outfile(outputFile);
  outfile << "backup created: " << currentDateTime();

  // save from output to src
  // just put attachment under this directory and would be copied together
  vector<string> filenameList;
  Poco::File(HTML_OUTPUT_MAIN).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToClear(HTML_SRC_MAIN + file);
    if (fileToClear.exists())
      fileToClear.remove(true);
    Poco::File fileToCopy(HTML_OUTPUT_MAIN + file);
    fileToCopy.copyTo(HTML_SRC_MAIN + file);
  }
}

bool KeyStartAndCommentStartNotFound(const string &testStr, const string &key) {
  if (debug >= LOG_INFO)
    cout << testStr << " and key: " << key << endl;
  auto keyStartBegin = testStr.find(keyStartChars);
  auto commentStartBegin = testStr.find(commentBeginChars);
  if (keyStartBegin == string::npos and commentStartBegin == string::npos)
    return true;
  return false;
}

bool isOnlyPartOfOtherKeys(const string &orgLine, const string &key) {
  string line = orgLine;
  while (true) {
    auto keyBegin = line.find(key);
    if (keyBegin == string::npos) // no key any more
      break;
    auto keyEnd = line.find(keyEndChars, keyBegin);
    if (keyEnd == string::npos) // no keyEnd any more
      return false;
    string testStr = line.substr(keyBegin, keyEnd - keyBegin);
    if (not KeyStartAndCommentStartNotFound(testStr, key))
      return false;
    else {
      string beforeKey = line.substr(0, keyBegin);
      if (debug >= LOG_INFO)
        cout << beforeKey << endl;
      // if only part of comment
      if (beforeKey.find(keyStartChars) == string::npos)
        return false;
    }
    line = line.substr(keyEnd + keyEndChars.length());
    if (debug >= LOG_INFO)
      cout << line << endl;
  }
  return true;
}
/**
 * find a keyword in a numbered bodytext file
 * whose name is specified as fullPath
 * @param key the keyword used to search
 * @param fullPath the fullpath of the bodytext file
 * @param lineNumber the return value of the line's number like P8L2
 * @param needChange return value indicating a valid line is found
 * and thus need to change original link
 * @return changed key (to KeyNotFound + key + reason) if it is not found,
 * otherwise the original key
 */
string findKeyInFile(const string &key, const string &fullPath,
                     lineNumberSet ignorelineNumberSet, string &lineNumber,
                     bool &needChange) {
  ifstream infile(fullPath);
  if (!infile) {
    cout << "file doesn't exist:" << fullPath << endl;
    return keyNotFound + key + ": bodytext file doesn't exist";
  }
  string line{""};
  string lineName{""};
  bool found = false;
  LineNumber ln;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    getline(infile, line);              // Saves the line
    if (line.find(key) == string::npos) // not appear in this line
    {
      continue;
    }
    // if "key" is only part of the key of another link, skip this line
    if (isOnlyPartOfOtherKeys(line, key)) {
      continue;
    }
    // find the key in current line
    ln.loadFromContainedLine(line);
    if (not ln.valid()) {
      cout << "file doesn't get numbered:" << fullPath << " at line:" << line
           << endl;
      return keyNotFound + key + ": bodytext file doesn't get numbered";
    }

    // special hack for ignoring one lineNumber
    if (not ignorelineNumberSet.empty() and
        ignorelineNumberSet.find(ln.asString()) != ignorelineNumberSet.end())
      continue;
    found = true;
    break;
  }
  needChange = true;
  if (not found) {
    return keyNotFound + key;
  }
  // continue with lineName found
  lineNumber = ln.asString();
  return key;
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

int utf8length(std::string originalString) {
  size_t len = 0, byteIndex = 0;
  const char *aStr = originalString.c_str();
  for (; byteIndex < originalString.size(); byteIndex++) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
  }
  return len;
}

std::string utf8substr(std::string originalString, size_t begin, size_t &end,
                       size_t SubStrLength) {
  const char *aStr = originalString.c_str();
  size_t origSize = originalString.size();
  size_t byteIndex = begin;
  size_t len = 0;

  end = begin;
  for (; byteIndex < origSize; byteIndex++) {
    if ((aStr[byteIndex] & 0xc0) != 0x80)
      len += 1;
    if (len >= SubStrLength) {
      end = byteIndex - 1;
      break;
    }
  }
  return originalString.substr(begin, byteIndex - begin);
}

/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void clearLinksInContainerBodyText(int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile(outputFile);
}

/**
 * append a link string in the body text of final htm file
 * @param linkString the string to put into
 * @param containerNumber the selected container to put into
 */
void appendLinkInContainerBodyText(string linkString, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>" << linkString << "</br>" << endl;
}
/**
 * append a text string in the body text of final htm file
 * @param text the string to put into
 * @param containerNumber the selected container to put into
 */
void appendTextInContainerBodyText(string text, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>"
          << R"(<b>)" << text << "</b>"
          << "</br>" << endl;
}

void appendNumberLineInContainerBodyText(string line, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>" << endl;
  outfile << line << endl;
}

void addFirstParagraphInContainerBodyText(int startNumber,
                                          int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  //  outfile << firstParaHeader << endl;
}

void addParagraphInContainerBodyText(int startNumber, int paraNumber,
                                     int containerNumber) {}

void addLastParagraphInContainerBodyText(int startNumber, int paraNumber,
                                         int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + BODY_TEXT_SUFFIX;
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << R"(<hr color="#F0BEC0">)" << endl;
  //  outfile << lastParaHeader << endl;
}
