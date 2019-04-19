#include "numbering.hpp"

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
      CoupledBodyTextWithLink bodyText;
      bodyText.setFilePrefixFromFileType(targetFileType);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      bodyText.addLineNumber(forceUpdate, hideParaHeader);
    }
  }
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
      container.setFileAndAttachmentNumber(file, attNo);
      container.dissembleFromHTM();
    }
  }
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "Attachments dissemble finished. " << endl;
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
      container.setFileAndAttachmentNumber(file, attNo);
      container.assembleBackToHTM();
    }
  }
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "assemble finished. " << endl;
}

void refreshBodyTexts(const string &kind, int minTarget, int maxTarget) {
  CoupledContainer container(getFileTypeFromString(kind));
  auto digits = (kind == JPM) ? 3 : 2;
  if (kind == MAIN)
    CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget, digits)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  FUNCTION_OUTPUT << "Refreshing " << kind << " BodyTexts finished. " << endl;
}

void refreshAttachmentBodyTexts(int minTarget, int maxTarget, int minAttachNo,
                                int maxAttachNo) {
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

static const string HTML_OUTPUT_LINES_OF_MAIN =
    "utf8HTML/output/LinesOfMain.txt";

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::MAIN);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  CoupledBodyTextWithLink::setReferFilePrefix(MAIN_BODYTEXT_PREFIX);
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      HTML_OUTPUT_LINES_OF_MAIN);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.disableAutoNumbering();
    bodyText.addLineNumber(forceUpdate, hideParaHeader);
  }
  CoupledBodyTextWithLink::displayNumberedLines();
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering Main Html finished. " << endl;
}

static const string HTML_OUTPUT_LINES_OF_ORIGINAL =
    "utf8HTML/output/LinesOfOriginal.txt";

void numberOriginalHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  CoupledBodyTextWithLink::setReferFilePrefix(ORIGINAL_BODYTEXT_PREFIX);
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      HTML_OUTPUT_LINES_OF_ORIGINAL);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::ORIGINAL);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.addLineNumber(forceUpdate, hideParaHeader);
  }
  CoupledBodyTextWithLink::displayNumberedLines();
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering Original Html finished. " << endl;
}

static const string HTML_OUTPUT_LINES_OF_JPM = "utf8HTML/output/LinesOfJPM.txt";

void numberJPMHtmls(int num, bool forceUpdate, bool hideParaHeader) {
  auto oldDebug = debug;
  if (num == 2) {
    debug = LOG_EXCEPTION;
  }

  int minTarget = 72, maxTarget = 72;
  CoupledContainer container(FILE_TYPE::JPM);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  if (num == 3) {
    CoupledBodyTextWithLink::setReferFilePrefix(JPM_BODYTEXT_PREFIX);
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        HTML_OUTPUT_LINES_OF_JPM);
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::JPM);
    bodyText.setFileAndAttachmentNumber(file);
    switch (num) {
    case 1:
      bodyText.validateFormatForNumbering();
      break;
    case 2:
      bodyText.validateParaSize();
      break;
    case 3:
      //      bodyText.disableAutoNumbering();
      bodyText.addLineNumber(forceUpdate, hideParaHeader);
      break;
    default:
      FUNCTION_OUTPUT << "no test executed." << endl;
    }
  }
  if (num == 3)
    CoupledBodyTextWithLink::displayNumberedLines();
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering JPM Html finished. " << endl;
  if (num == 2) {
    debug = oldDebug;
  }
}

static const string HTML_OUTPUT_LINES_OF_ATTACHMENTS =
    "utf8HTML/output/LinesOfAttachments.txt";

void numberAttachmentHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 1, maxAttachNo = 50;
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  CoupledBodyTextWithLink::setReferFilePrefix(ATTACHMENT_BODYTEXT_PREFIX);
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      HTML_OUTPUT_LINES_OF_ATTACHMENTS);
  addLineNumbersForAttachmentHtml(
      minTarget, maxTarget, minAttachNo, maxAttachNo, forceUpdate,
      hideParaHeader); // reformat bodytext by adding line number
  CoupledBodyTextWithLink::displayNumberedLines();
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  FUNCTION_OUTPUT << "Numbering Attachment Html finished. " << endl;
}
