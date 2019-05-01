#include "numberingAndLinkFixing.hpp"

FileSet keyMissingChapters;
FileSet newAttachmentList;

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
void fixLinksFromMain(bool forceUpdateLink) {
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
    if (forceUpdateLink)
      bodyText.forceUpdateLink();
    bodyText.fixLinksFromFile(
        buildFileSet(minReferenceToMain, maxReferenceToMain),
        buildFileSet(minReferenceToOriginal, maxReferenceToOriginal),
        buildFileSet(minReferenceToJPM, maxReferenceToJPM));
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
    CoupledContainer attachmentContainer(FILE_TYPE::ATTACHMENT);
    attachmentContainer.setFileAndAttachmentNumber(file);
    for (const auto &attNo : attachmentContainer.getAttachmentFileList()) {
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

void fixLinksFromAttachment(bool forceUpdateLink) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToMain = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToMain = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToOriginal = MAIN_MIN_CHAPTER_NUMBER,
      maxReferenceToOriginal = MAIN_MAX_CHAPTER_NUMBER;
  int minReferenceToJPM = JPM_MIN_CHAPTER_NUMBER,
      maxReferenceToJPM = JPM_MAX_CHAPTER_NUMBER;
  int minAttachNo = MIN_ATTACHMENT_NUMBER, maxAttachNo = MAX_ATTACHMENT_NUMBER;
  // load files from output back to src
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();

  // load known reference attachment list
  LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();

  // dissemble html files
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledContainer container(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.dissembleFromHTM();
    }
  }

  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledContainer container(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    // if to fix all attachments
    //  set minAttachNo = 0, maxAttachNo = 0;
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      CoupledBodyTextWithLink bodyText;
      bodyText.setFilePrefixFromFileType(FILE_TYPE::ATTACHMENT);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      if (forceUpdateLink)
        bodyText.forceUpdateLink();
      bodyText.fixLinksFromFile(
          buildFileSet(minReferenceToMain, maxReferenceToMain),
          buildFileSet(minReferenceToOriginal, maxReferenceToOriginal),
          buildFileSet(minReferenceToJPM, maxReferenceToJPM));
    }
  }

  // load fixed body texts back to output directory
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();

  // dissemble html files
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledContainer container(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.assembleBackToHTM();
    }
  }
  // statistics of links fixing
  LinkFromAttachment::outPutStatisticsToFiles();
  FUNCTION_OUTPUT << "fixLinksFromAttachment finished. " << endl;
}

void Numbering::execute() {
  switch (m_command) {
  case 1:
    m_bodyText.validateFormatForNumbering();
    break;
  case 2:
    if (not m_disableAutoNumbering)
      m_bodyText.validateParaSize();
    break;
  case 3:
    if (m_disableAutoNumbering)
      m_bodyText.disableAutoNumbering();
    if (m_forceUpdateLineNumber)
      m_bodyText.forceUpdateLineNumber();
    if (m_hideParaHeader)
      m_bodyText.hideParaHeader();
    m_bodyText.addLineNumber();
    break;
  default:
    FUNCTION_OUTPUT << "no command executed." << endl;
  }
}

void Numbering::numberHtmls() {

  auto oldDebug = debug;
  if (m_command == 2) {
    debug = LOG_EXCEPTION;
  }

  auto fileType = getFileTypeFromString(m_kind);
  CoupledContainer container(fileType);

  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();

  auto m_fileSet = buildFileSet(m_minTarget, m_maxTarget, m_filenameDigit);

  dissembleHtmls(container);

  if (m_command == 3) {
    CoupledBodyTextWithLink::setReferFilePrefix(
        getFilePrefixFromFileType(fileType));
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        getStatisticsOutputFilePathFromString(m_kind));
  }

  m_bodyText.setFilePrefixFromFileType(fileType);
  runOverEachFile(container);

  CoupledBodyText::loadBodyTextsFromFixBackToOutput();

  assembleHtmls(container);

  if (m_command == 2) {
    debug = oldDebug;
  }

  FUNCTION_OUTPUT << "Numbering" << m_kind << " Html finished. " << endl;
}

void NumberingNonAttachment::dissembleHtmls(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
}

void NumberingNonAttachment::assembleHtmls(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
}

void NumberingNonAttachment::runOverEachFile(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    m_bodyText.setFileAndAttachmentNumber(file);
    execute();
  }
}

void NumberingAttachment::dissembleHtmls(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.dissembleFromHTM();
    }
  }
}

void NumberingAttachment::assembleHtmls(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.assembleBackToHTM();
    }
  }
}

void NumberingAttachment::runOverEachFile(CoupledContainer &container) {
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_bodyText.setFileAndAttachmentNumber(file, attNo);
      m_bodyText.disableAutoNumbering();
      m_bodyText.disableNumberingStatistics();
      execute();
    }
  }
}

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(int num, bool forceUpdateLineNumber, bool hideParaHeader,
                     bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = num;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = MAIN;
  numbering.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberOriginalHtmls(int num, bool forceUpdateLineNumber,
                         bool hideParaHeader, bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = num;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = ORIGINAL;
  numbering.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberJPMHtmls(int num, bool forceUpdateLineNumber, bool hideParaHeader,
                    bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = num;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = JPM;
  numbering.m_filenameDigit = THREE_DIGIT_FILENAME;
  numbering.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberAttachmentHtmls(int num, bool forceUpdateLineNumber,
                           bool hideParaHeader, bool disableAutoNumbering) {

  NumberingAttachment numbering;
  numbering.m_command = num;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = ATTACHMENT;
  numbering.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  //	numbering.m_minAttachNo = MIN_ATTACHMENT_NUMBER;
  //	numbering.m_maxAttachNo = MAX_ATTACHMENT_NUMBER;
  numbering.numberHtmls();
}


void refreshBodyTexts(const string &kind, int minTarget, int maxTarget) {
  CoupledContainer container(getFileTypeFromString(kind));
  auto digits = (kind == JPM) ? THREE_DIGIT_FILENAME : TWO_DIGIT_FILENAME;
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
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledContainer container(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.dissembleFromHTM();
    }
  }
}
