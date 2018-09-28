#include "hlmFileType.hpp"

static const string final = R"(</html>)"; // last line of the html file
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
  for (int i = min; i <= max; i++) {
    fs.insert(formatIntoTwoDigitChapterNumber(i));
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
 * copy them to HTML_OUTPUT or HTML_OUTPUT_ATTACHMENT
 * and this function would backup current files
 * (incl. sub-dirs) under HTML_SRC
 * and load newly copied files from HTML_OUTPUT to HTML_SRC
 * then dissemble would happen from HTML_SRC afterwards
 */
void backupAndOverwriteSrcForHTML() {
  string BACKUP = "utf8HTML/src" + currentTimeStamp();
  cout << "backup of current src is created under : " << BACKUP << endl;

  Poco::File BackupPath(BACKUP);
  if (!BackupPath.exists())
    BackupPath.createDirectories();

  // backup whole src directory together with files to this directory
  Poco::File dirToCopy(HTML_SRC);
  dirToCopy.copyTo(BACKUP);

  // create a date file in this backup directory
  string outputFile = BACKUP + "/info.txt";
  ofstream outfile(outputFile);
  outfile << "backup created: " << currentDateTime();

  // save from output to src
  // just put attachment under this directory and would be copied together
  vector<string> filenameList;
  Poco::File(HTML_OUTPUT).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToClear(HTML_SRC + file);
    if (fileToClear.exists())
      fileToClear.remove(true);
    Poco::File fileToCopy(HTML_OUTPUT + file);
    fileToCopy.copyTo(HTML_SRC + file);
  }
}

/**
 * read a .htm file under HTML_SRC or HTML_SRC_ORIGINAL or HTML_SRC_ATTACHMENT
 * and get body text starting from the line containing name="top" inclusively
 * till the line containing  name="bottom" inclusively
 * and store that body texts into corresponding file under BODY_TEXT_OUTPUT
 * @param referFile 2-digit string chapter number
 * @param type target file type
 * main files are a0XX.htm
 * attachment files are b0XX.htm
 * original files are c0XX.htm
 * @param attachNo the attachmentNumber if file type is attachment
 */
void dissembleFromHTM(const string &inputHtmlFile,
                      const string &outputBodyTextFile) {
  ifstream infile(inputHtmlFile);
  if (!infile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outfile(outputBodyTextFile);
  string line{""};
  bool started = false, ended = false;

  string start = topTab;
  string end = bottomTab;
  while (!infile.eof()) // To get you all the lines.
  {
    getline(infile, line); // Saves the line in line.
    if (not started) {
      auto linkBegin = line.find(start);
      if (linkBegin != string::npos) {
        started = true;
        outfile << line << endl; // including the top paragraph
        continue;
      }
    } else // started
    {
      if (not ended) {
        auto linkEnd = line.find(end);
        if (linkEnd != string::npos) {
          ended = true;
          outfile << line << endl; // including the end paragraph
          break;
        } else
          outfile << line << endl; // including all lines in between
        continue;
      }
    }
  }
  if (not started)
    cout << "no top paragraph found for " << inputHtmlFile << "as " << start
         << endl;
  else if (not ended)
    cout << "no end paragraph found for " << inputHtmlFile << "as " << end
         << endl;
  else if (debug >= LOG_INFO)
    cout << "dissemble finished for " << inputHtmlFile << endl;
}

/**
 * read header and tail of a .htm file under HTML_SRC or HTML_SRC_ORIGINAL or
 * HTML_SRC_ATTACHMENT and put its corresponding body text in-between to form a
 * new .htm file and store under HTML_OUTPUT
 * @param referFile 2-digit string chapter number
 * @param type target file type
 * main files are a0XX.htm
 * attachment files are b0XX.htm
 * original files are c0XX.htm
 * @param attachNo the attachmentNumber if file type is attachment
 */
void assembleBackToHTM(const string &inputHtmlFile,
                       const string &inputBodyTextFile,
                       const string &outputFile) {

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "HTM file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) // doesn't exist
  {
    cout << "Body text file doesn't exist:" << inputBodyTextFile << endl;
    return;
  }

  ofstream outfile(outputFile);
  string line{""};
  bool started = false;

  string start = topTab;    // first line
  string end = bottomTab;   // last line
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    if (not started) {
      auto linkBegin = line.find(start);
      if (linkBegin != string::npos) {
        started = true;
        break;
      }
      outfile << line << endl; // excluding start line
    }
  }
  if (inHtmlFile.eof() and not started) {
    cout << "source .htm: " << inputBodyTextFile
         << " has no start mark:" << start << endl;
    return;
  }
  bool ended = false;
  while (!inBodyTextFile.eof()) // To get you all the lines.
  {
    getline(inBodyTextFile, line); // Saves the line in line.
    if (not ended) {
      outfile << line << endl; // including end line
      auto linkEnd = line.find(end);
      if (linkEnd != string::npos) {
        ended = true;
        break;
      }
    }
  }
  if (inBodyTextFile.eof() and not ended) {
    cout << "source body text file: " << inputBodyTextFile
         << " has no end mark:" << end << endl;
    return;
  }
  ended = false;
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.

    if (not ended) {
      auto linkEnd = line.find(end);
      if (linkEnd != string::npos) {
        ended = true;
        continue;
      }
    } else {
      outfile << line << endl; // excluding end line
      if (line.find(final) != string::npos) {
        break; // including this final line
      }
    }
  }
  if (debug >= LOG_INFO)
    cout << "assemble finished for " << outputFile << endl;
}

/**
 * dissemble a set of original files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void dissembleOriginalHtmls(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputHtmlFile = HTML_SRC_ORIGINAL +
                           getFileNamePrefix(FILE_TYPE::ORIGINAL) + file +
                           HTML_SUFFIX;
    string outputBodyTextFile = BODY_TEXT_OUTPUT +
                                getBodyTextFilePrefix(FILE_TYPE::ORIGINAL) +
                                file + BODY_TEXT_SUFFIX;
    dissembleFromHTM(inputHtmlFile, outputBodyTextFile);
  }
  if (debug >= LOG_INFO)
    cout << "Original Htmls dissemble finished. " << endl;
}

/**
 * assemble a set of original files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void assembleOriginalHtmls(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputHtmlFile = HTML_SRC_ORIGINAL +
                           getFileNamePrefix(FILE_TYPE::ORIGINAL) + file +
                           HTML_SUFFIX;
    string inputBodyTextFile = BODY_TEXT_OUTPUT +
                               getBodyTextFilePrefix(FILE_TYPE::ORIGINAL) +
                               file + BODY_TEXT_SUFFIX;
    string outputFile = HTML_OUTPUT_ORIGINAL +
                        getFileNamePrefix(FILE_TYPE::ORIGINAL) + file +
                        HTML_SUFFIX;
    assembleBackToHTM(inputHtmlFile, inputBodyTextFile, outputFile);
  }
  if (debug >= LOG_INFO)
    cout << "Original Htmls assemble finished. " << endl;
}

/**
 * dissemble a set of Main files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void dissembleMainHtmls(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputHtmlFile =
        HTML_SRC + getFileNamePrefix(FILE_TYPE::MAIN) + file + HTML_SUFFIX;
    string outputBodyTextFile = BODY_TEXT_OUTPUT +
                                getBodyTextFilePrefix(FILE_TYPE::MAIN) + file +
                                BODY_TEXT_SUFFIX;
    dissembleFromHTM(inputHtmlFile, outputBodyTextFile);
  }
  if (debug >= LOG_INFO)
    cout << "Main Htmls dissemble finished. " << endl;
}

/**
 * assemble a set of Main files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void assembleMainHtmls(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputHtmlFile =
        HTML_SRC + getFileNamePrefix(FILE_TYPE::MAIN) + file + HTML_SUFFIX;
    string inputBodyTextFile = BODY_TEXT_OUTPUT +
                               getBodyTextFilePrefix(FILE_TYPE::MAIN) + file +
                               BODY_TEXT_SUFFIX;
    string outputFile =
        HTML_OUTPUT + getFileNamePrefix(FILE_TYPE::MAIN) + file + HTML_SUFFIX;
    assembleBackToHTM(inputHtmlFile, inputBodyTextFile, outputFile);
  }
  if (debug >= LOG_INFO)
    cout << "Main Htmls assemble finished. " << endl;
}

/**
 * dissemble a set of Attachment files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * dissemble all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo) {
  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (not(minAttachNo == 0 and maxAttachNo == 0) and
      minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      string inputHtmlFile =
          HTML_SRC_ATTACHMENT + getFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      string outputBodyTextFile = BODY_TEXT_OUTPUT +
                                  getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) +
                                  file + attachmentFileMiddleChar +
                                  TurnToString(attNo) + BODY_TEXT_SUFFIX;

      dissembleFromHTM(inputHtmlFile, outputBodyTextFile);
    }
  }
  if (debug >= LOG_INFO)
    cout << "Attachments dissemble finished. " << endl;
}

/**
 * assemble a set of Attachment files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * assemble all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo) {
  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (not(minAttachNo == 0 and maxAttachNo == 0) and
      minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      string inputHtmlFile =
          HTML_SRC_ATTACHMENT + getFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      string inputBodyTextFile = BODY_TEXT_OUTPUT +
                                 getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) +
                                 file + attachmentFileMiddleChar +
                                 TurnToString(attNo) + BODY_TEXT_SUFFIX;
      string outputFile =
          HTML_OUTPUT_ATTACHMENT + getFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      assembleBackToHTM(inputHtmlFile, inputBodyTextFile, outputFile);
    }
  }
  if (debug >= LOG_INFO)
    cout << "assemble finished. " << endl;
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
                     string &lineNumber, bool &needChange) {
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
    // find the key in current line
    ln.loadFromContainedLine(line);
    if (not ln.valid()) {
      cout << "file doesn't get numbered:" << fullPath << " at line:" << line
           << endl;
      return keyNotFound + key + ": bodytext file doesn't get numbered";
    }
    // special hack for ignoring one lineNumber
    if (not lineNumber.empty() and ln.equal(lineNumber))
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
