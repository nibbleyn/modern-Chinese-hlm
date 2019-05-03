#include "numberingAndLinkFixing.hpp"
#include "contentTable.hpp"

void Commander::execute() {
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

void Commander::runCommandOverFiles() {

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

  outputLinkFixingStatistics();
  updateAttachmentContentTableAndfixReturnLink();
  restoreDebugLevel();
  METHOD_OUTPUT << "Done processing. " << endl;
}

void NonAttachmentCommander::dissembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    m_container.dissembleFromHTM();
  }
}

void NonAttachmentCommander::assembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    m_container.assembleBackToHTM();
  }
}

void NonAttachmentCommander::runCommandOverEachFile() {
  for (const auto &file : m_fileSet) {
    m_bodyText.setFileAndAttachmentNumber(file);
    execute();
  }
}

void AttachmentCommander::dissembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         m_container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_container.setFileAndAttachmentNumber(file, attNo);
      m_container.dissembleFromHTM();
    }
  }
}

void AttachmentCommander::assembleHtmls() {
  for (const auto &file : m_fileSet) {
    m_container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         m_container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_container.setFileAndAttachmentNumber(file, attNo);
      m_container.assembleBackToHTM();
    }
  }
}

void AttachmentCommander::runCommandOverEachFile() {
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
void Commander::updateAttachmentContentTableAndfixReturnLink() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    CoupledContainer::refAttachmentTable.loadReferenceAttachmentList();

    generateContentTableForReferenceAttachments(false);
    generateContentTableForPersonalAttachments(false);
    if (m_fixReturnLink)
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
}

void Commander::setupNumberingStatistics() {
  if (m_command == COMMAND::addLineNumber) {
    CoupledBodyTextWithLink::setReferFilePrefix(
        getFilePrefixFromFileType(m_fileType));
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        getStatisticsOutputFilePathFromString(m_kind));
  }
}

void Commander::setupLinkFixingStatistics() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    // load known reference attachment list
    LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList();
    LinkFromMain::clearLinkTable();
  }
  if (m_command == COMMAND::fixLinksFromAttachmentFile) {
    // load known reference attachment list
    LinkFromAttachment::clearLinkTable();
  }
}

void Commander::outputLinkFixingStatistics() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    // statistics of links fixing
    LinkFromMain::outPutStatisticsToFiles();
    LinkFromMain::displayMainFilesOfMissingKey();
    LinkFromMain::attachmentTable.displayNewlyAddedAttachments();
  }
  if (m_command == COMMAND::fixLinksFromAttachmentFile)
    // statistics of links fixing
    LinkFromAttachment::outPutStatisticsToFiles();
}

void Commander::increaseDebugLevel() {
  m_storedDebugLevel = debug;
  if (m_command == COMMAND::validateParaSizeForAutoNumbering) {
    debug = LOG_EXCEPTION;
  }
}

void Commander::restoreDebugLevel() {
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
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::addLineNumber;
  commander.m_disableAutoNumbering = disableAutoNumbering;
  commander.m_forceUpdateLineNumber = forceUpdateLineNumber;
  commander.m_hideParaHeader = hideParaHeader;
  commander.m_kind = MAIN;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void numberOriginalHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                         bool disableAutoNumbering) {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::addLineNumber;
  commander.m_disableAutoNumbering = disableAutoNumbering;
  commander.m_forceUpdateLineNumber = forceUpdateLineNumber;
  commander.m_hideParaHeader = hideParaHeader;
  commander.m_kind = ORIGINAL;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void numberJPMHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                    bool disableAutoNumbering) {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::addLineNumber;
  commander.m_disableAutoNumbering = disableAutoNumbering;
  commander.m_forceUpdateLineNumber = forceUpdateLineNumber;
  commander.m_hideParaHeader = hideParaHeader;
  commander.m_kind = JPM;
  commander.m_filenameDigit = THREE_DIGIT_FILENAME;
  commander.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void numberAttachmentHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                           bool disableAutoNumbering) {

  AttachmentCommander commander;
  commander.m_command = Commander::COMMAND::addLineNumber;
  commander.m_disableAutoNumbering = disableAutoNumbering;
  commander.m_forceUpdateLineNumber = forceUpdateLineNumber;
  commander.m_hideParaHeader = hideParaHeader;
  commander.m_kind = ATTACHMENT;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  //	commander.m_minAttachNo = MIN_ATTACHMENT_NUMBER;
  //	commander.m_maxAttachNo = MAX_ATTACHMENT_NUMBER;
  commander.runCommandOverFiles();
}

void validateMainHtmls() {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::validateFormatForNumbering;
  commander.m_kind = MAIN;
  commander.runCommandOverFiles();
}

void validateAttachmentHtmls() {
  AttachmentCommander commander;
  commander.m_command = Commander::COMMAND::validateFormatForNumbering;
  commander.m_kind = ATTACHMENT;
  commander.runCommandOverFiles();
}

void validateParaSizeForAutoNumberingJPMHtmls() {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::validateParaSizeForAutoNumbering;
  commander.m_kind = JPM;
  commander.runCommandOverFiles();
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
 * before this function to work, commander all main, original files
 * no requirement for commander attachment files.
 */
void fixLinksFromMain(bool forceUpdateLink) {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::fixLinksFromMainFile;
  commander.m_kind = MAIN;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void fixLinksFromAttachment(bool forceUpdateLink) {
  AttachmentCommander commander;
  commander.m_command = Commander::COMMAND::fixLinksFromAttachmentFile;
  commander.m_kind = ATTACHMENT;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  commander.m_fixReturnLink = false;
  commander.runCommandOverFiles();
}
