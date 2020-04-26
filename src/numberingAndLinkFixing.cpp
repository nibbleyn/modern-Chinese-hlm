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
    //    m_bodyText.disableNumberingStatisticsCalculateLines();
    m_bodyText.addLineNumber();
    break;
  case COMMAND::appendReverseLinks:
    m_bodyText.appendReverseLinks();
    break;
  case COMMAND::fixLinksFromMainFile:
  case COMMAND::fixLinksFromJPMFile:
  case COMMAND::fixLinksFromAttachmentFile:
    if (m_forceUpdateLink)
      m_bodyText.forceUpdateLink();
    //    m_bodyText.forceRemoveDuplicatedOriginalLinks();
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
  m_fileType = getFileTypeFromKind(m_kind);
  m_container.setFileType(m_fileType);
  m_fileSet = buildFileSet(m_minTarget, m_maxTarget, m_kind);

  backupAndOverwriteSrcFiles();

  setupNumberingStatistics();
  setupLinkFixingStatistics();

  setupReferenceFileSet();

  dissembleHtmls();
  setupBodyTextFixDir();

  m_bodyText.setFilePrefixFromFileType(m_fileType);
  runCommandOverEachFile();

  LoadBodyTextFromFixDir();
  assembleHtmls();

  updateAttachmentListIntoFile();
  updateContentTableAndfixReturnLink();
  restoreDebugLevel();
  outputLinkFixingStatistics();
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

void Commander::updateContentTableAndfixReturnLink() {
  if (m_command == COMMAND::addLineNumber) {
    generateContentTable(TABLE_TYPE::POEMS);
  }
  if (m_command == COMMAND::fixLinksFromMainFile) {
    generateContentTable(TABLE_TYPE::IMAGE);
    CoupledBodyTextContainer::refAttachmentTable.setSourceFile(
        OUTPUT_REF_ATTACHMENT_LIST_PATH);
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();

    generateContentTable(TABLE_TYPE::ATTACHMENT, ATTACHMENT_TYPE::REFERENCE,
                         false);
    generateContentTable(TABLE_TYPE::ATTACHMENT, ATTACHMENT_TYPE::PERSONAL,
                         false);
    // fix return links of attachments to output directory
    if (m_fixReturnLink) {
      for (const auto &file : m_fileSet) {
        CoupledBodyTextContainer attachmentContainer;
        attachmentContainer.setFileType(FILE_TYPE::ATTACHMENT);
        attachmentContainer.setFileAndAttachmentNumber(file);
        for (const auto &attNo : attachmentContainer.getAttachmentFileList()) {
          attachmentContainer.setFileAndAttachmentNumber(file, attNo);
          attachmentContainer.fixReturnLinkForAttachmentFile();
        }
      }
    }
  }
}

void Commander::backupAndOverwriteSrcFiles() {
  bool attachmentRequired =
      (m_kind == MAIN and m_command == COMMAND::fixLinksFromMainFile);
  bool wholeFolder = (m_kind == MAIN or m_kind == ATTACHMENT) and
                     m_minReferenceToMain == MAIN_MIN_CHAPTER_NUMBER and
                     m_maxReferenceToMain == MAIN_MAX_CHAPTER_NUMBER;
  m_container.setBackupFilenameList(m_fileSet, wholeFolder, attachmentRequired);
  m_container.backupAndOverwriteInputHtmlFiles();
}

void Commander::setupNumberingStatistics() {
  if (m_command == COMMAND::addLineNumber) {
    CoupledBodyTextWithLink::setReferFilePrefix(
        getBodyTextFilePrefixFromFileType(m_fileType));
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        getStatisticsOutputFilePathFromString(m_kind));
    CoupledBodyTextWithLink::clearExistingNumberingStatistics();
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
  if (m_command == COMMAND::appendReverseLinks) {
    LinkFromMain::loadLinkTableFromFile();
  }
}

void Commander::setupBodyTextFixDir() {
  if (m_command != COMMAND::validateFormatForNumbering and
      m_command != COMMAND::validateParaSizeForAutoNumbering)
    m_container.clearFixedBodyTexts();
}

void Commander::LoadBodyTextFromFixDir() {
  if (m_command != COMMAND::validateFormatForNumbering and
      m_command != COMMAND::validateParaSizeForAutoNumbering)
    m_container.loadFixedBodyTexts();
}

void Commander::setupReferenceFileSet() {
  if (m_command == COMMAND::fixLinksFromMainFile or
      m_command == COMMAND::fixLinksFromAttachmentFile) {
    m_referenceToMainfileSet =
        buildFileSet(m_minReferenceToMain, m_maxReferenceToMain, MAIN);
    m_referenceToOriginalfileSet = buildFileSet(
        m_minReferenceToOriginal, m_maxReferenceToOriginal, ORIGINAL);
    m_referenceToJPMfileSet =
        buildFileSet(m_minReferenceToJPM, m_maxReferenceToJPM, JPM);
  }

  if (m_command == COMMAND::fixLinksFromJPMFile) {
    m_referenceToJPMfileSet =
        buildFileSet(m_minReferenceToJPM, m_maxReferenceToJPM, JPM);
  }
}

void Commander::updateAttachmentListIntoFile() {
  if (m_command == COMMAND::fixLinksFromMainFile) {
    LinkFromMain::updateReferenceAttachmentListIntoFile();
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
  commander.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void numberMDTHtmls(bool forceUpdateLineNumber, bool hideParaHeader,
                    bool disableAutoNumbering) {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::addLineNumber;
  commander.m_disableAutoNumbering = disableAutoNumbering;
  commander.m_forceUpdateLineNumber = forceUpdateLineNumber;
  commander.m_hideParaHeader = hideParaHeader;
  commander.m_kind = MDT;
  commander.m_minTarget = MDT_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MDT_MAX_CHAPTER_NUMBER;
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
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
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
  CoupledBodyTextContainer container;
  auto fileSet = buildFileSet(minTarget, maxTarget, kind);
  container.setBackupFilenameList(fileSet);
  container.backupAndOverwriteInputHtmlFiles();
  for (const auto &file : fileSet) {
    container.setFileType(getFileTypeFromKind(kind));
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  FUNCTION_OUTPUT << "Refreshing " << kind << " BodyTexts finished. " << endl;
}

void renderingBodyText(const string &kind, int minTarget, int maxTarget) {
  CoupledBodyTextContainer container;
  auto fileSet = buildFileSet(minTarget, maxTarget, kind);
  for (const auto &file : fileSet) {
    container.setFileType(getFileTypeFromKind(kind));
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }

  bool hidePersonalComment = false;
  for (const auto &file : fileSet) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(getFileTypeFromKind(kind));
    bodyText.setFileAndAttachmentNumber(file);
    if (hidePersonalComment) {
      bodyText.addHiddenType(nameOfPersonalCommentType);
    } else {
      PersonalComment::enableAddSpecialDisplayText();
    }
    PoemTranslation::enableAddSpecialDisplayText();
    LinkFromMain::enableAddSpecialDisplayText();
    bodyText.hideParaHeader();
    bodyText.enablePostProcessLine();
    bodyText.addHiddenType(nameOfLineNumberType);
    bodyText.addHiddenType(nameOfLinkFromMainType);
    bodyText.render();
    FUNCTION_OUTPUT << "Rendering " << kind << " BodyTexts finished. " << endl;
    FUNCTION_OUTPUT << "Files are under "
                    << bodyText.getOutputBodyTextFilePath() << endl;
  }
}

void removePersonalViewpoints(const string &kind, int minTarget,
                              int maxTarget) {
  FUNCTION_OUTPUT << "to remove <u> pairs." << endl;
  FUNCTION_OUTPUT << "and to remove personal attachment link." << endl;
  LinkFromMain::attachmentTable.setSourceFile(OUTPUT_REF_ATTACHMENT_LIST_PATH);
  LinkFromMain::attachmentTable.loadReferenceAttachmentList();

  auto fileSet = buildFileSet(minTarget, maxTarget, kind);
  CoupledBodyTextContainer container;
  for (const auto &file : fileSet) {
    container.setFileType(getFileTypeFromKind(kind));
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  for (const auto &file : fileSet) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(getFileTypeFromKind(kind));
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.removePersonalCommentsOverNumberedFiles();
  }
}

void refreshAttachmentBodyTexts(int minTarget, int maxTarget, int minAttachNo,
                                int maxAttachNo) {
  CoupledBodyTextContainer container;
  auto fileSet = buildFileSet(minTarget, maxTarget);
  container.setBackupFilenameList(fileSet);
  container.backupAndOverwriteInputHtmlFiles();
  for (const auto &file : fileSet) {
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

void appendReverseLinksForMain() {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::appendReverseLinks;
  commander.m_kind = MAIN;
  commander.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  commander.runCommandOverFiles();
}

void fixLinksFromJPM(bool forceUpdateLink) {
  NonAttachmentCommander commander;
  commander.m_command = Commander::COMMAND::fixLinksFromJPMFile;
  commander.m_kind = JPM;
  commander.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
  commander.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
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
