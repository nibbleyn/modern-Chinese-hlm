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
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "Links fixing  finished. " << endl;
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    auto targetAttachments =
        container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    CoupledContainer attachmentContainer(FILE_TYPE::ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      attachmentContainer.setFileAndAttachmentNumber(file, attNo);
      attachmentContainer.fixReturnLinkForAttachmentFile();
    }
  }
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "Return Link fixing finished. " << endl;
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
    if (overAllAttachments == true) {
      container.setFileAndAttachmentNumber(file);
      targetAttachments =
          container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    }
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
