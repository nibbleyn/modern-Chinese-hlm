#include "linkFix.hpp"

fileSet keyMissingChapters;
fileSet newAttachmentList;

void clearReport() {
  keyMissingChapters.clear();
  newAttachmentList.clear();
}

void displayMainFilesOfMissingKey() {
  if (keyMissingChapters.empty())
    return;
  FUNCTION_OUTPUT << "files which has missing key links:" << endl;
  for (const auto &file : keyMissingChapters) {
    FUNCTION_OUTPUT << getHtmlFileNamePrefix(FILE_TYPE::MAIN) + file + ".htm"
                    << endl;
  }
}

void displayNewlyAddedAttachments() {
  if (newAttachmentList.empty())
    return;
  FUNCTION_OUTPUT << "Newly Added Attachments:" << endl;
  for (const auto &file : newAttachmentList) {
    FUNCTION_OUTPUT << file + ".htm" << endl;
  }
}

/**
 * check lineNumber from refAttachmentList about a link to attachment
 * and put that lineNumber in that attachment file header
 * @param filename
 */
void fixReturnLinkForAttachmentFile(const string &referFile,
                                    const string &inputHtmlFile,
                                    const string &outputFile) {
  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    FUNCTION_OUTPUT << "HTM file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outfile(outputFile);
  string line{""};
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto textBegin = line.find(returnLinkFromAttachmentHeader);
    if (textBegin == string::npos) {
      outfile << line << endl;
      continue;
    } else {
      auto orgLine = line; // inLine would change in loop below
      auto start = linkStartChars;
      string link{""};
      while (true) {
        auto linkBegin = line.find(start);
        if (linkBegin == string::npos) // no link any more, continue with next
                                       // line
          break;
        auto linkEnd = line.find(linkEndChars, linkBegin);
        link = line.substr(linkBegin, linkEnd + 4 - linkBegin);
        LinkFromAttachment lfm(referFile,
                               link); // get only type and annotation
        if (lfm.getAnnotation() == returnLinkFromAttachmentHeader)
          break;
        else
          line = line.substr(linkEnd + 4); // find next link in the line
      }
      if (not link.empty()) {
        LinkFromAttachment lfm(referFile, link);
        auto num = getAttachmentNumber(
            getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile);
        // special hack to make sure using a0... as return file name
        lfm.setTypeThruFileNamePrefix("main"); // must return to main html
        lfm.fixReferFile(num.first);
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

void fixReturnLinkForAttachments(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    auto targetAttachments =
        getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      string inputHtmlFile =
          HTML_SRC_ATTACHMENT + getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      string outputFile = HTML_OUTPUT_ATTACHMENT +
                          getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + file +
                          attachmentFileMiddleChar + TurnToString(attNo) +
                          HTML_SUFFIX;
      fixReturnLinkForAttachmentFile(file + attachmentFileMiddleChar +
                                         TurnToString(attNo),
                                     inputHtmlFile, outputFile);
    }
  }
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "fix Return Link finished. " << endl;
}

/**
 * before this function to work, numbering all main, original files
 * no requirement for numbering attachment files.
 * however, attachment files must be put under HTML_SRC_ATTACHMENT
 * for getting titles of them.
 * and fixReturnLinkForAttachments would fix these attachment files
 * and save them into HTML_OUTPUT_ATTACHMENT just like assembleAttachments
 */
void fixLinksFromMainHtmls(bool forceUpdate) {
  int minTarget = 1, maxTarget = 80;
  int minReferenceToMain = 1, maxReferenceToMain = 80;
  int minReferenceToOriginal = 1, maxReferenceToOriginal = 80;
  int minReferenceToJPM = 1, maxReferenceToJPM = 100;
  CoupledContainer container(FILE_TYPE::MAIN);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.fixLinksFromFile(
        buildFileSet(minReferenceToMain, maxReferenceToMain),
        buildFileSet(minReferenceToOriginal, maxReferenceToOriginal),
        buildFileSet(minReferenceToJPM, maxReferenceToJPM), forceUpdate);
  }
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  fixReturnLinkForAttachments(minTarget, maxTarget);
}

void fixLinksFromMain(bool forceUpdate) {
  clearReport();
  LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList();
  fixLinksFromMainHtmls(forceUpdate);
  LinkFromMain::outPutStatisticsToFiles();
  displayMainFilesOfMissingKey();
  displayNewlyAddedAttachments();
  FUNCTION_OUTPUT << "fixLinksFromMain finished. " << endl;
}

void fixLinksFromAttachmentHtmls(bool forceUpdate) {
  int minTarget = 1, maxTarget = 80;
  int minReferenceToMain = 1, maxReferenceToMain = 80;
  int minReferenceToOriginal = 1, maxReferenceToOriginal = 80;
  int minReferenceToJPM = 1, maxReferenceToJPM = 100;
  int minAttachNo = 1, maxAttachNo = 50;
  // if to fix all attachments
  //  int minAttachNo = 0, maxAttachNo = 0;
  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (minAttachNo != 0 and maxAttachNo != 0 and minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      CoupledBodyTextWithLink bodyText;
      bodyText.setFilePrefixFromFileType(FILE_TYPE::ATTACHMENT);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      bodyText.fixLinksFromFile(
          buildFileSet(minReferenceToMain, maxReferenceToMain),
          buildFileSet(minReferenceToOriginal, maxReferenceToOriginal),
          buildFileSet(minReferenceToJPM, maxReferenceToJPM), forceUpdate);
    }
  }
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

void fixLinksFromAttachment(bool forceUpdate) {
  LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();
  fixLinksFromAttachmentHtmls(forceUpdate);
  LinkFromAttachment::outPutStatisticsToFiles();
  FUNCTION_OUTPUT << "fixLinksFromAttachment finished. " << endl;
}
