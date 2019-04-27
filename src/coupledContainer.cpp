#include "coupledContainer.hpp"

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
 * from the type of link to get the filename prefix of bodytext file
 * all main bodytext files are MainXX.txt
 * all attachment bodytext files are AttachXX.txt
 * all original bodytext files are OrgXX.txt
 * never get called for type SAMEPAGE
 * @param type type of file
 * @return filename prefix of bodytext file
 */
string CoupledContainer::getBodyTextFilePrefix() {
  if (m_fileType == FILE_TYPE::ORIGINAL)
    return ORIGINAL_BODYTEXT_PREFIX;
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_BODYTEXT_PREFIX;
  if (m_fileType == FILE_TYPE::JPM)
    return JPM_BODYTEXT_PREFIX;
  return MAIN_BODYTEXT_PREFIX;
}

string CoupledContainer::getInputHtmlFilePath() {
  string attachmentPart{emptyString};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart =
        attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  return m_htmlInputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
         attachmentPart + HTML_SUFFIX;
}

string CoupledContainer::getoutputHtmlFilepath() {
  string attachmentPart{emptyString};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart =
        attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  return m_htmlOutputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
         attachmentPart + HTML_SUFFIX;
}

string CoupledContainer::getBodyTextFilePath() {
  string attachmentPart{emptyString};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart =
        attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  return m_bodyTextInputFilePath + getBodyTextFilePrefix() + m_file +
         attachmentPart + BODY_TEXT_SUFFIX;
}

void CoupledContainer::assembleBackToHTM(const string &title,
                                         const string &displayTitle) {

  string inputHtmlFile = getInputHtmlFilePath();
  string inputBodyTextFile = getBodyTextFilePath();
  string outputFile = getoutputHtmlFilepath();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    METHOD_OUTPUT << "HTM file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) // doesn't exist
  {
    METHOD_OUTPUT << "Body text file doesn't exist:" << inputBodyTextFile
                  << endl;
    return;
  }

  ofstream outfile(outputFile);
  string line{""};
  bool started = false;

  string start = topIdBeginChars;  // first line
  string end = bottomIdBeginChars; // last line
  while (!inHtmlFile.eof())        // To get you all the lines.
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
    METHOD_OUTPUT << "source .htm: " << inputBodyTextFile
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
    METHOD_OUTPUT << "source body text file: " << inputBodyTextFile
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
      if (line.find(htmlFileFinalLine) != string::npos) {
        break; // including this final line
      }
    }
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "assemble finished for " << outputFile << endl;
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
  string dir = HTML_SRC_MAIN.substr(0, HTML_SRC_MAIN.find_last_of('/'));
  string BACKUP = dir + currentTimeStamp();
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "backup of current src is created under : " << BACKUP
                    << endl;

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

void CoupledContainer::dissembleFromHTM() {
  string inputHtmlFile = getInputHtmlFilePath();
  string outputBodyTextFile = getBodyTextFilePath();

  ifstream infile(inputHtmlFile);
  if (!infile) // doesn't exist
  {
    METHOD_OUTPUT << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outfile(outputBodyTextFile);
  string line{""};
  bool started = false, ended = false;

  string start = topIdBeginChars;
  string end = bottomIdBeginChars;
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
    METHOD_OUTPUT << "no top paragraph found for " << inputHtmlFile << "as "
                  << start << endl;
  else if (not ended)
    METHOD_OUTPUT << "no end paragraph found for " << inputHtmlFile << "as "
                  << end << endl;
  else if (debug >= LOG_INFO)
    METHOD_OUTPUT << "dissemble finished for " << inputHtmlFile << endl;
}

/**
 * check lineNumber from refAttachmentList about a link to attachment
 * and put that lineNumber in that attachment file header
 * @param filename
 */
void CoupledContainer::fixReturnLinkForAttachmentFile() {
  string inputHtmlFile = getInputHtmlFilePath();
  string outputFile = getoutputHtmlFilepath();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    METHOD_OUTPUT << "HTM file doesn't exist:" << inputHtmlFile << endl;
    return;
  }

  ofstream outfile(outputFile);
  string line{""};
  string referFile =
      m_file + attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto textBegin = line.find(returnLinkFromAttachmentHeader);
    if (textBegin == string::npos) {
      outfile << line << endl;
      continue;
    } else {
      auto orgLine = line; // inLine would change in loop below
      string link{""};
      while (true) {
        auto linkBegin = line.find(linkStartChars);
        if (linkBegin == string::npos) // no link any more, continue with next
                                       // line
          break;
        link = getWholeStringBetweenTags(line, linkStartChars, linkEndChars);
        LinkFromAttachment lfm(referFile,
                               link); // get only type and annotation
        if (lfm.getAnnotation() == returnLinkFromAttachmentHeader)
          break;
        else
          line = line.substr(linkBegin +
                             lfm.length()); // find next link in the line
      }
      if (not link.empty()) {
        LinkFromAttachment lfm(referFile, link);
        // special hack to make sure using a0... as return file name
        lfm.setTypeThruFileNamePrefix("main"); // must return to main html
        lfm.fixReferFile(TurnToInt(m_file));
        AttachmentNumber num(TurnToInt(m_file), m_attachmentNumber);
        lfm.fixReferPara(LinkFromMain::getFromLineOfAttachment(num));
        if (lfm.needUpdate()) // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      outfile << orgLine << endl;
    }
  }
}

/**
 * get all attachment files for referFile under fromDir
 * for example, if there are b003_1.html b003_5.html and b003_15.html
 * this function would return {1,5,15} for referFile "03"
 * @param referFile the 2-digit string part of refer file
 * @param fromDir where its attachment files are under
 * @return the vector of attachment numbers
 */
vector<int>
CoupledContainer::getAttachmentFileListForChapter(const string &fromDir) {
  vector<string> filenameList;
  vector<int> attList;
  Poco::File(fromDir).list(filenameList);
  for (const auto &file : filenameList) {
    if (file.find(getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + m_file) !=
        string::npos) {
      string attNo = getIncludedStringBetweenTags(
          file, attachmentFileMiddleChar, HTML_SUFFIX);

      attList.push_back(TurnToInt(attNo));
    }
  }
  return attList;
}
