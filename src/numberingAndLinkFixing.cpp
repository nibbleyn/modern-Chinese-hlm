#include "numberingAndLinkFixing.hpp"

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
    if (overAllAttachments == true) {
      CoupledContainer container(FILE_TYPE::ATTACHMENT);
      container.setFileAndAttachmentNumber(file);
      targetAttachments =
          container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    }
    for (const auto &attNo : targetAttachments) {
      FILE_TYPE targetFileType = FILE_TYPE::ATTACHMENT;
      CoupledBodyTextWithLink bodyText;
      bodyText.setFilePrefixFromFileType(targetFileType);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      bodyText.disableAutoNumbering();
      bodyText.disableNumberingStatistics();
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
    if (overAllAttachments == true) {
      container.setFileAndAttachmentNumber(file);
      targetAttachments =
          container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    }
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
    if (overAllAttachments == true) {
      container.setFileAndAttachmentNumber(file);
      targetAttachments =
          container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    }
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
    R"(utf8HTML/output/LinesOfMain.txt)";

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledContainer container(FILE_TYPE::MAIN);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  CoupledBodyTextWithLink::setReferFilePrefix(MAIN_BODYTEXT_PREFIX);
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      HTML_OUTPUT_LINES_OF_MAIN);
  CoupledBodyTextWithLink::clearExistingNumberingStatistics();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.disableAutoNumbering();
    bodyText.addLineNumber(forceUpdate, hideParaHeader);
  }
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering Main Html finished. " << endl;
}

static const string HTML_OUTPUT_LINES_OF_ORIGINAL =
    R"(utf8HTML/output/LinesOfOriginal.txt)";

void numberOriginalHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
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
    bodyText.disableAutoNumbering();
    bodyText.disableNumberingStatistics();
    bodyText.addLineNumber(forceUpdate, hideParaHeader);
  }
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering Original Html finished. " << endl;
}

static const string HTML_OUTPUT_LINES_OF_JPM =
    R"(utf8HTML/output/LinesOfJPM.txt)";

void numberJPMHtmls(int num, bool forceUpdate, bool hideParaHeader) {
  auto oldDebug = debug;
  if (num == 2) {
    debug = LOG_EXCEPTION;
  }

  int minTarget = JPM_MIN_CHAPTER_NUMBER, maxTarget = JPM_MAX_CHAPTER_NUMBER;
  CoupledContainer container(FILE_TYPE::JPM);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file :
       buildFileSet(minTarget, maxTarget, THREE_DIGIT_FILENAME)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  if (num == 3) {
    CoupledBodyTextWithLink::setReferFilePrefix(JPM_BODYTEXT_PREFIX);
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        HTML_OUTPUT_LINES_OF_JPM);
  }
  for (const auto &file :
       buildFileSet(minTarget, maxTarget, THREE_DIGIT_FILENAME)) {
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
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file :
       buildFileSet(minTarget, maxTarget, THREE_DIGIT_FILENAME)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "Numbering JPM Html finished. " << endl;
  if (num == 2) {
    debug = oldDebug;
  }
}

static const string HTML_OUTPUT_LINES_OF_ATTACHMENTS =
    R"(utf8HTML/output/LinesOfAttachments.txt)";

void numberAttachmentHtmls(bool forceUpdate, bool hideParaHeader) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  int minAttachNo = MIN_ATTACHMENT_NUMBER, maxAttachNo = MAX_ATTACHMENT_NUMBER;
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  CoupledBodyTextWithLink::setReferFilePrefix(ATTACHMENT_BODYTEXT_PREFIX);
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      HTML_OUTPUT_LINES_OF_ATTACHMENTS);
  // reformat bodytext by adding line number
  addLineNumbersForAttachmentHtml(minTarget, maxTarget, minAttachNo,
                                  maxAttachNo, forceUpdate, hideParaHeader);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  FUNCTION_OUTPUT << "Numbering Attachment Html finished. " << endl;
}

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
 */
void fixLinksFromMain(bool forceUpdate) {
  clearReport();

  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToMain = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToMain = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToOriginal = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToOriginal = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToJPM = JPM_MIN_CHAPTER_NUMBER,
      maxReferenceToJPM = JPM_MAX_CHAPTER_NUMBER;

  // load files from output back to src
  CoupledContainer container(FILE_TYPE::MAIN);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();

  // load known reference attachment list
  LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList();

  // dissemble html files
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }

  // fix links in body texts
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
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

  // load fixed body texts back to output directory
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();

  // assemble back htmls to output directory
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }

  // fix return links of attachments to output directory
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

  // statistics of links fixing
  LinkFromMain::outPutStatisticsToFiles();
  displayMainFilesOfMissingKey();
  displayNewlyAddedAttachments();
  FUNCTION_OUTPUT << "fixLinksFromMain finished. " << endl;
}

void fixLinksFromAttachment(bool forceUpdate) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToMain = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToMain = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToOriginal = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToOriginal = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToJPM = JPM_MIN_CHAPTER_NUMBER,
      maxReferenceToJPM = JPM_MAX_CHAPTER_NUMBER;
  int minAttachNo = MIN_ATTACHMENT_NUMBER, maxAttachNo = MAX_ATTACHMENT_NUMBER;
  // if to fix all attachments
  //  int minAttachNo = 0, maxAttachNo = 0;
  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (minAttachNo != 0 and maxAttachNo != 0 and minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  // load files from output back to src
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();

  // load known reference attachment list
  LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();

  // dissemble html files
  dissembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true) {
      container.setFileAndAttachmentNumber(file);
      targetAttachments =
          container.getAttachmentFileListForChapter(HTML_SRC_ATTACHMENT);
    }

    // fix links in body texts
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

  // load fixed body texts back to output directory
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();

  // dissemble html files
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
  // statistics of links fixing
  LinkFromAttachment::outPutStatisticsToFiles();
  FUNCTION_OUTPUT << "fixLinksFromAttachment finished. " << endl;
}
