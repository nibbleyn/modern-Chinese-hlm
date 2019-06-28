#include "coupledBodyTextContainer.hpp"

AttachmentList CoupledBodyTextContainer::refAttachmentTable;

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
  // get time now
  time_t t = time(0);
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
string CoupledBodyTextContainer::getBodyTextFilePrefix() {
  if (m_fileType == FILE_TYPE::ORIGINAL)
    return ORIGINAL_BODYTEXT_PREFIX;
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    return ATTACHMENT_BODYTEXT_PREFIX;
  if (m_fileType == FILE_TYPE::JPM)
    return JPM_BODYTEXT_PREFIX;
  return MAIN_BODYTEXT_PREFIX;
}

void CoupledBodyTextContainer::setInputHtmlFileName() {
  string attachmentPart{emptyString};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart =
        attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  m_inputHtmlFilename =
      getHtmlFileNamePrefix(m_fileType) + m_file + attachmentPart;
}

void CoupledBodyTextContainer::setBodyTextFileName() {
  string attachmentPart{emptyString};
  if (m_fileType == FILE_TYPE::ATTACHMENT)
    attachmentPart =
        attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  m_inputBodyTextFilename = getBodyTextFilePrefix() + m_file + attachmentPart;
}

/**
 * to numbering or linkfixing main or attachment files
 * just copy them to HTML_OUTPUT_MAIN or HTML_OUTPUT_ATTACHMENT
 * and this function would backup current files
 * (incl. sub-dirs) under HTML_SRC_MAIN
 * and load newly copied files from HTML_OUTPUT_MAIN to HTML_SRC_MAIN
 * then dissemble would happen from HTML_SRC_MAIN afterwards
 */
void CoupledBodyTextContainer::backupAndOverwriteInputHtmlFiles() {
  string dir = string(m_htmlInputFilePath)
                   .substr(0, string(m_htmlInputFilePath).find_last_of('/'));
  string BACKUP = dir + currentTimeStamp();
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "backup of " << m_htmlInputFilePath
                    << " is created under : " << BACKUP << endl;

  Poco::File BackupPath(BACKUP);
  if (!BackupPath.exists())
    BackupPath.createDirectories();

  // backup whole src directory together with files to this directory
  Poco::File dirToCopy(m_htmlInputFilePath);
  dirToCopy.copyTo(BACKUP);

  // create a date file in this backup directory
  ofstream outfile(BACKUP + "/info.txt");
  outfile << "backup created: " << currentDateTime();

  // save from output to src
  // just put attachment under this directory and would be copied together
  vector<string> filenameList;
  Poco::File(m_htmlOutputFilePath).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToClear(m_htmlInputFilePath + file);
    if (fileToClear.exists())
      fileToClear.remove();
    Poco::File fileToCopy(m_htmlOutputFilePath + file);
    fileToCopy.copyTo(m_htmlInputFilePath + file);
  }
}

string replaceReferPara(const string &linkString, const string &referFile,
                        const string &newReferpara) {
  auto orgString = linkString;
  auto oldReferPara = getIncludedStringBetweenTags(
      linkString, referFile + referParaMiddleChar, referParaEndChar);
  if (oldReferPara != newReferpara) {
    orgString.replace(orgString.find(oldReferPara), oldReferPara.length(),
                      newReferpara);
  }
  return orgString;
}

/**
 * check lineNumber from refAttachmentList about a link to attachment
 * and put that lineNumber in that attachment file header
 * @param filename
 */
void CoupledBodyTextContainer::fixReturnLinkForAttachmentFile() {
  ifstream inHtmlFile(getInputHtmlFilePath());
  if (!inHtmlFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << getInputHtmlFilePath() << endl;
    return;
  }
  ofstream outfile(getoutputHtmlFilepath());
  string line{emptyString};
  string referFile =
      m_file + attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    auto textBegin = line.find(returnLinkFromAttachmentHeader);
    if (textBegin == string::npos) {
      outfile << line << endl;
      continue;
    } else {
      // varible line would change in loop below
      auto orgLine = line;
      string link{emptyString};
      while (true) {
        auto linkBegin = line.find(linkStartChars);
        // no link any more, continue with next line
        if (linkBegin == string::npos)
          break;
        link = getWholeStringBetweenTags(line, linkStartChars, linkEndChars);
        // get only type and annotation
        Link lfm(referFile, link);
        if (lfm.getAnnotation() == returnLinkFromAttachmentHeader)
          break;
        else
          // find next link in the line
          line = line.substr(linkBegin + link.length());
      }
      if (not link.empty()) {
        AttachmentNumber num(TurnToInt(m_file), m_attachmentNumber);
        orgLine.replace(
            orgLine.find(link), link.length(),
            replaceReferPara(link, m_file + HTML_SUFFIX,
                             refAttachmentTable.getFromLineOfAttachment(num)));
      }
      outfile << orgLine << endl;
    }
  }
}

AttachmentNumberList
CoupledBodyTextContainer::getAttachmentFileList(int minAttachNo,
                                                int maxAttachNo) {
  return getAttachmentFileListForChapter(m_htmlInputFilePath, TurnToInt(m_file),
                                         minAttachNo, maxAttachNo);
}

void CoupledBodyTextContainer::fetchOriginalAndTranslatedTitles() {
  ifstream inHtmlFile(getInputHtmlFilePath());
  if (!inHtmlFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << getInputHtmlFilePath() << endl;
    return;
  }
  string line{emptyString};
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << line << endl;
    }
    if (line.find(topIdBeginChars) != string::npos) {
      break;
    }
    if (line.find(strongTitleBeginChars) != string::npos) {
      m_originalTitle = getIncludedStringBetweenTags(
          line, strongTitleBeginChars, strongTitleEndChars);
    }
    if (line.find(sampTitleBeginChars) != string::npos) {
      m_translatedTitle = getIncludedStringBetweenTags(
          line, sampTitleBeginChars, sampTitleEndChars);
    }
  }
}
