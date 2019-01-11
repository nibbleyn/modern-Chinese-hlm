#include "container.hpp"

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

string CoupledContainer::getHtmlFileNamePrefix() {
  string filenamePrefix[] = {"a0", "b0", "c0"};
  if (m_fileType == FILE_TYPE::MAIN)
    return filenamePrefix[0];
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    return filenamePrefix[1];
  if (m_fileType == FILE_TYPE::ORIGINAL)
    return filenamePrefix[2];
  return "unsupported";
}

string CoupledContainer::getBodyTextFilePrefix() {
  string bodyTextFilePrefix[] = {"Main", "Attach", "Org"};
  if (m_fileType == FILE_TYPE::MAIN)
    return bodyTextFilePrefix[0];
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    return bodyTextFilePrefix[1];
  if (m_fileType == FILE_TYPE::ORIGINAL)
    return bodyTextFilePrefix[2];
  return "unsupported";
}
void CoupledContainer::assembleBackToHTM(const string &file, int attachNo,
                                         const string &title,
                                         const string &displayTitle) {
  string attachmentPart{""};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

  string inputHtmlFile = m_htmlInputFilePath + getHtmlFileNamePrefix() + file +
                         attachmentPart + HTML_SUFFIX;
  string inputBodyTextFile = m_bodyTextInputFilePath + getBodyTextFilePrefix() +
                             file + attachmentPart + BODY_TEXT_SUFFIX;
  string outputFile = m_htmlOutputFilePath + getHtmlFileNamePrefix() + file +
                      attachmentPart + HTML_SUFFIX;

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
      if (not title.empty()) {
        auto titleBegin = line.find(defaultTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultTitle.length(), title);
      }
      if (not displayTitle.empty()) {
        auto titleBegin = line.find(defaultDisplayTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultDisplayTitle.length(), displayTitle);
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
 * to numbering or linkfixing main or attachment files
 * just copy them to HTML_OUTPUT_MAIN or HTML_OUTPUT_ATTACHMENT
 * and this function would backup current files
 * (incl. sub-dirs) under HTML_SRC_MAIN
 * and load newly copied files from HTML_OUTPUT_MAIN to HTML_SRC_MAIN
 * then dissemble would happen from HTML_SRC_MAIN afterwards
 */
void CoupledContainer::backupAndOverwriteAllInputHtmlFiles() {
  std::string dir = HTML_SRC_MAIN.substr(0, HTML_SRC_MAIN.find_last_of('/'));
  string BACKUP = dir + currentTimeStamp();
  if (debug >= LOG_INFO)
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

void CoupledContainer::dissembleFromHTM(const string &file, int attachNo) {
  string attachmentPart{""};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

  string inputHtmlFile = m_htmlInputFilePath + getHtmlFileNamePrefix() + file +
                         attachmentPart + HTML_SUFFIX;
  string outputBodyTextFile = m_bodyTextInputFilePath +
                              getBodyTextFilePrefix() + file + attachmentPart +
                              BODY_TEXT_SUFFIX;

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
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
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
      container.dissembleFromHTM(file, attNo);
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
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
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
      container.assembleBackToHTM(file, attNo);
    }
  }
  if (debug >= LOG_INFO)
    cout << "assemble finished. " << endl;
}

/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void GenericContainer::clearBodyTextFile() {
  string inputBodyTextFile =
      m_bodyTextInputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  if (debug >= LOG_INFO)
    cout << "clear content in: " << inputBodyTextFile << endl;
  ofstream outfile(inputBodyTextFile);
}

/**
 * append a link string in the body text of final htm file
 * @param linkString the string to put into
 * @param containerNumber the selected container to put into
 */
void GenericContainer::appendParagraphInBodyText(const string &text) {
  string inputBodyTextFile =
      m_bodyTextInputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  if (debug >= LOG_INFO)
    cout << "append Paragraph In BodyText: " << inputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(inputBodyTextFile, std::ios_base::app);
  outfile << "<br>" << text << "</br>" << endl;
}

void GenericContainer::assembleBackToHTM(const string &title,
                                         const string &displayTitle) {

  string inputHtmlFile = m_htmlInputFilePath + getInputFileName() + HTML_SUFFIX;
  string inputBodyTextFile =
      m_bodyTextInputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  string outputFile = m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputBodyTextFile << endl;
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
      if (not title.empty()) {
        auto titleBegin = line.find(defaultTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultTitle.length(), title);
      }
      if (not displayTitle.empty()) {
        auto titleBegin = line.find(defaultDisplayTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultDisplayTitle.length(), displayTitle);
      }
      if (debug >= LOG_INFO)
        cout << line << endl;  // including end line
      outfile << line << endl; // excluding start line
    }
  }
  if (inHtmlFile.eof() and not started) {
    cout << "source htm" << inputBodyTextFile << "has no start mark:" << start
         << endl;
    return;
  }
  bool ended = false;
  while (!inBodyTextFile.eof()) // To get you all the lines.
  {
    getline(inBodyTextFile, line); // Saves the line in line.
    if (debug >= LOG_INFO)
      cout << line << endl;  // including end line
    outfile << line << endl; // including end line
  }
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
      if (debug >= LOG_INFO)
        cout << line << endl;  // including end line
      outfile << line << endl; // excluding end line
    }
  }
}
