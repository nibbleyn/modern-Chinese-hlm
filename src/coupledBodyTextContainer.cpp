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
  m_bodyTextFilename = getBodyTextFilePrefix() + m_file + attachmentPart;
}

void CoupledBodyTextContainer::setBackupFilenameList(FileSet files,
                                                     bool wholeFolder,
                                                     bool attachmentRequired) {
  m_backupFilenameList.clear();
  if (m_fileType == FILE_TYPE::MAIN)
    m_backupFilenameList.insert(REF_ATTACHMENT_LIST_FILENAME);

  // fixing main files would fix return links from attachments
  if (m_fileType == FILE_TYPE::ATTACHMENT or
      (attachmentRequired and m_fileType == FILE_TYPE::MAIN)) {
    // assume output folder has more files always
    auto outputDir = (m_fileType == FILE_TYPE::ATTACHMENT)
                         ? m_outputHtmlDir
                         : m_outputHtmlDir + ATTACHMENT_DIR;
    vector<string> filenameList;
    Poco::File(outputDir).list(filenameList);
    for (const auto &file : filenameList) {
      if (wholeFolder or
          find(files.begin(), files.end(),
               getIncludedStringBetweenTags(
                   file, getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT),
                   attachmentFileMiddleChar)) != files.end()) {
        if (m_fileType == FILE_TYPE::ATTACHMENT)
          m_backupFilenameList.insert(file);
        else if (m_fileType == FILE_TYPE::MAIN)
          m_backupFilenameList.insert(ATTACHMENT_DIR + file);
      }
    }
  }
  if (m_fileType != FILE_TYPE::ATTACHMENT)
    for (const auto &file : files)
      m_backupFilenameList.insert(getHtmlFileNamePrefix(m_fileType) + file +
                                  HTML_SUFFIX);
}

static constexpr const char *PATH_SEPARATOR = R"(/)";

void CoupledBodyTextContainer::backupAndOverwriteInputHtmlFiles() {
  string dir =
      m_inputHtmlDir.substr(0, m_inputHtmlDir.find_last_of(PATH_SEPARATOR));
  string BACKUP = dir + currentTimeStamp() + PATH_SEPARATOR;
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "backup of " << m_inputHtmlDir
                    << " is created under : " << BACKUP << endl;

  // create a date file in this backup directory
  ofstream outfile(BACKUP + "info.txt");
  outfile << "backup created: " << currentDateTime();

  Poco::File BackupPath(BACKUP);
  if (!BackupPath.exists())
    BackupPath.createDirectories();

  for (const auto &file : m_backupFilenameList) {
    if (file.find(PATH_SEPARATOR) != string::npos) {
      dir = BACKUP + file.substr(0, file.find(PATH_SEPARATOR));
      Poco::File BackupPath(dir);
      if (!BackupPath.exists())
        BackupPath.createDirectories();
    }
    Poco::File fileUnderSrc(m_inputHtmlDir + file);
    if (fileUnderSrc.exists()) {
      fileUnderSrc.moveTo(BACKUP + file);
    }
    Poco::File fileToCopy(m_outputHtmlDir + file);
    fileToCopy.copyTo(m_inputHtmlDir + file);
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
  return getAttachmentFileListForChapter(m_inputHtmlDir, TurnToInt(m_file),
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
