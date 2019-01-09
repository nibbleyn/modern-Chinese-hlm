#include "container.hpp"

void CoupledContainer::assembleBackToHTM(const string &file, int attachNo,
                                         const string &title,
                                         const string &displayTitle) {
  string attachmentPart{""};
  if (fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

  string inputHtmlFile = htmlInputFilePath + getFileNamePrefix(fileType) +
                         file + attachmentPart + HTML_SUFFIX;
  string inputBodyTextFile = htmlInputFilePath + getFileNamePrefix(fileType) +
                             file + attachmentPart + BODY_TEXT_SUFFIX;
  string outputFile = htmlOutputFilePath + getFileNamePrefix(fileType) + file +
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

void CoupledContainer::dissembleFromHTM(const string &file, int attachNo) {
  string attachmentPart{""};
  if (fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

  string inputHtmlFile = htmlInputFilePath + getFileNamePrefix(fileType) +
                         file + attachmentPart + HTML_SUFFIX;
  string outputBodyTextFile = bodyTextInputFilePath +
                              getFileNamePrefix(fileType) + file +
                              attachmentPart + BODY_TEXT_SUFFIX;

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

void GenericContainer::assembleBackToHTM(const string &title,
                                         const string &displayTitle) {

  string inputHtmlFile = htmlInputFilePath + getInputFileName() + HTML_SUFFIX;
  string inputBodyTextFile =
      bodyTextInputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  string outputFile = htmlOutputFilePath + outputFilename + HTML_SUFFIX;

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
