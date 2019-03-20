#include "numbering.hpp"

/**
 * numbering lines a set of Original body text files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void addLineNumbers(int minTarget, int maxTarget, FILE_TYPE targetFileType,
                    bool forceUpdate = true, bool hideParaHeader = false) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string separatorColor = getSeparateLineColor(targetFileType);
    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(targetFileType);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.addLineNumber(separatorColor, forceUpdate, hideParaHeader);
  }
}

/**
 * numbering lines of a set of Attachment body text files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * Numbering all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void addLineNumbersForAttachmentHtml(int minTarget, int maxTarget,
                                     int minAttachNo, int maxAttachNo,
                                     bool forceUpdate = true,
                                     bool hideParaHeader = false) {

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
      FILE_TYPE targetFileType = FILE_TYPE::ATTACHMENT;
      string separatorColor = getSeparateLineColor(targetFileType);
      CoupledBodyText bodyText;
      bodyText.setFilePrefixFromFileType(targetFileType);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      bodyText.addLineNumber(separatorColor, forceUpdate, hideParaHeader);
    }
  }
}

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 5, maxTarget = 5;
  CoupledContainer container(FILE_TYPE::MAIN);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  addLineNumbers(minTarget, maxTarget, FILE_TYPE::MAIN, forceUpdate,
                 hideParaHeader);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  cout << "Numbering Main Html finished. " << endl;
}

void numberOriginalHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  addLineNumbers(minTarget, maxTarget, FILE_TYPE::ORIGINAL, forceUpdate,
                 hideParaHeader);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  cout << "Numbering Original Html finished. " << endl;
}

void numberJPMHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 100;
  CoupledContainer container(FILE_TYPE::JPM);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  addLineNumbers(minTarget, maxTarget, FILE_TYPE::JPM, forceUpdate,
                 hideParaHeader);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  cout << "Numbering JPM Html finished. " << endl;
}

void numberAttachmentHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 1, maxAttachNo = 50;
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  addLineNumbersForAttachmentHtml(
      minTarget, maxTarget, minAttachNo, maxAttachNo, forceUpdate,
      hideParaHeader); // reformat bodytext by adding line number
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  cout << "Numbering Attachment Html finished. " << endl;
}
