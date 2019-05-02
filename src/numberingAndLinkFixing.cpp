#include "numberingAndLinkFixing.hpp"

void Numbering::execute() {
  switch (m_command) {
  case COMMAND::validateFormatForNumbering:
    m_bodyText.validateFormatForNumbering();
    break;
  case COMMAND::validateParaSizeForAutoNumbering:
    if (not m_disableAutoNumbering)
      m_bodyText.validateParaSize();
    break;
  case COMMAND::addLineNumber:
    if (m_disableAutoNumbering)
      m_bodyText.disableAutoNumbering();
    if (m_forceUpdateLineNumber)
      m_bodyText.forceUpdateLineNumber();
    if (m_hideParaHeader)
      m_bodyText.hideParaHeader();
    m_bodyText.addLineNumber();
    break;
  case COMMAND::fixLinksFromMainFile:
  case COMMAND::fixLinksFromAttachmentFile:
    if (m_forceUpdateLink)
      m_bodyText.forceUpdateLink();
    m_bodyText.fixLinksFromFile(m_referenceToMainfileSet,
                                m_referenceToOriginalfileSet,
                                m_referenceToJPMfileSet);
    break;

  default:
    METHOD_OUTPUT << "no command executed." << endl;
  }
}

void Numbering::numberHtmls() {

  increaseDebugLevel();
  m_fileType = getFileTypeFromString(m_kind);
  m_container.setFileType(m_fileType);

  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();

  setupNumberingStatistics();
  setupLinkFixingStatistics();

  m_fileSet = buildFileSet(m_minTarget, m_maxTarget, m_filenameDigit);

  if (m_command == COMMAND::fixLinksFromMainFile or
      m_command == COMMAND::fixLinksFromAttachmentFile) {
    m_referenceToMainfileSet = buildFileSet(
        m_minReferenceToMain, m_maxReferenceToMain, TWO_DIGIT_FILENAME);
    m_referenceToOriginalfileSet = buildFileSet(
        m_minReferenceToOriginal, m_maxReferenceToOriginal, TWO_DIGIT_FILENAME);
    m_referenceToJPMfileSet = buildFileSet(
        m_minReferenceToJPM, m_maxReferenceToJPM, THREE_DIGIT_FILENAME);
  }

  dissembleHtmls();

  m_bodyText.setFilePrefixFromFileType(m_fileType);
  runCommandOverEachFile();

  CoupledBodyText::loadBodyTextsFromFixBackToOutput();

  assembleHtmls();

  fixReturnLink();
  outputLinkFixingStatistics();
  restoreDebugLevel();
  METHOD_OUTPUT << m_kind << " Html is done processing. " << endl;
}

void NumberingNonAttachment::dissembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    m_container.dissembleFromHTM();
  }
}

void NumberingNonAttachment::assembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    m_container.assembleBackToHTM();
  }
}

void NumberingNonAttachment::runCommandOverEachFile() {
  for (const auto &file : m_fileSet) {
    m_bodyText.setFileAndAttachmentNumber(file);
    execute();
  }
}

void NumberingAttachment::dissembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         m_container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_container.setFileAndAttachmentNumber(file, attNo);
      m_container.dissembleFromHTM();
    }
  }
}

void NumberingAttachment::assembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         m_container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_container.setFileAndAttachmentNumber(file, attNo);
      m_container.assembleBackToHTM();
    }
  }
}

void NumberingAttachment::runCommandOverEachFile() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         m_container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_bodyText.setFileAndAttachmentNumber(file, attNo);
      m_bodyText.disableAutoNumbering();
      m_bodyText.disableNumberingStatistics();
      execute();
    }
  }
}
void Numbering::fixReturnLink() {
  if (m_command == COMMAND::fixLinksFromMainFile and m_fixReturnLink)
    // fix return links of attachments to output directory
    for (const auto &file : m_fileSet) {
      CoupledContainer attachmentContainer;
      attachmentContainer.setFileType(FILE_TYPE::ATTACHMENT);
      attachmentContainer.setFileAndAttachmentNumber(file);
      for (const auto &attNo : attachmentContainer.getAttachmentFileList()) {
        attachmentContainer.setFileAndAttachmentNumber(file, attNo);
        attachmentContainer.fixReturnLinkForAttachmentFile();
      }
    }
}

void Numbering::setupNumberingStatistics() {
  if (m_command == COMMAND::addLineNumber) {
    CoupledBodyTextWithLink::setReferFilePrefix(
        getFilePrefixFromFileType(m_fileType));
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        getStatisticsOutputFilePathFromString(m_kind));
  }
}

void Numbering::setupLinkFixingStatistics() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    // load known reference attachment list
    LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList();
    LinkFromMain::clearReport();
  }
  if (m_command == COMMAND::fixLinksFromAttachmentFile) {
    // load known reference attachment list
    LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();
  }
}

void Numbering::outputLinkFixingStatistics() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    // statistics of links fixing
    LinkFromMain::outPutStatisticsToFiles();
    LinkFromMain::displayMainFilesOfMissingKey();
    LinkFromMain::displayNewlyAddedAttachments();
  }
  if (m_command == COMMAND::fixLinksFromAttachmentFile)
    // statistics of links fixing
    LinkFromAttachment::outPutStatisticsToFiles();
}

void Numbering::increaseDebugLevel() {
  m_storedDebugLevel = debug;
  if (m_command == COMMAND::validateParaSizeForAutoNumbering) {
    debug = LOG_EXCEPTION;
  }
}

void Numbering::restoreDebugLevel() {
  if (m_command == COMMAND::validateParaSizeForAutoNumbering) {
    debug = m_storedDebugLevel;
  }
}

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                     bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::addLineNumber;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = MAIN;
  numbering.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberOriginalHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                         bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::addLineNumber;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = ORIGINAL;
  numbering.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberJPMHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                    bool disableAutoNumbering) {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::addLineNumber;
  numbering.m_disableAutoNumbering = disableAutoNumbering;
  numbering.m_forceUpdateLineNumber = forceUpdateLineNumber;
  numbering.m_hideParaHeader = hideParaHeader;
  numbering.m_kind = JPM;
  numbering.m_filenameDigit = THREE_DIGIT_FILENAME;
  numbering.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
  numbering.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
  numbering.numberHtmls();
}

void numberAttachmentHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                           bool disableAutoNumbering) {

  NumberingAttachment numbering;
  numbering.m_command = Numbering::COMMAND::addLineNumber;
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

void validateMainHtmls() {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::validateFormatForNumbering;
  numbering.m_kind = MAIN;
  numbering.numberHtmls();
}

void validateAttachmentHtmls() {
  NumberingAttachment numbering;
  numbering.m_command = Numbering::COMMAND::validateFormatForNumbering;
  numbering.m_kind = ATTACHMENT;
  numbering.numberHtmls();
}

void validateParaSizeForAutoNumberingJPMHtmls() {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::validateParaSizeForAutoNumbering;
  numbering.m_kind = JPM;
  numbering.numberHtmls();
}

void refreshBodyTexts(const string &kind, int minTarget, int maxTarget) {

  if (kind == MAIN)
    CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  auto digits = (kind == JPM) ? THREE_DIGIT_FILENAME : TWO_DIGIT_FILENAME;
  for (const auto &file : buildFileSet(minTarget, maxTarget, digits)) {
    CoupledContainer container;
    container.setFileType(getFileTypeFromString(kind));
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  FUNCTION_OUTPUT << "Refreshing " << kind << " BodyTexts finished. " << endl;
}

void refreshAttachmentBodyTexts(int minTarget, int maxTarget, int minAttachNo,
                                int maxAttachNo) {

  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledContainer container;
    container.setFileType(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.dissembleFromHTM();
    }
  }
}

/**
 * before this function to work, numbering all main, original files
 * no requirement for numbering attachment files.
 */
void fixLinksFromMain(bool forceUpdateLink) {
  NumberingNonAttachment numbering;
  numbering.m_command = Numbering::COMMAND::fixLinksFromMainFile;
  numbering.m_kind = MAIN;
  numbering.numberHtmls();
}

void fixLinksFromAttachment(bool forceUpdateLink) {
  NumberingAttachment numbering;
  numbering.m_command = Numbering::COMMAND::fixLinksFromAttachmentFile;
  numbering.m_kind = ATTACHMENT;
  numbering.m_fixReturnLink = false;
  numbering.numberHtmls();
}
