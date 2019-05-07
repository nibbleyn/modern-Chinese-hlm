#include "search.hpp"

void Searcher::execute() {
  m_bodyText.resetBeforeSearch();
  m_bodyText.searchForAll();
  if (m_fileType == FILE_TYPE::MAIN)
    m_bodyText.ignorePersonalComments();
  bool found = m_bodyText.findKey(m_key);
  if (found) {
    auto num = m_bodyText.getFileAndAttachmentNumber();
    CoupledBodyText::lineNumberSet lineSet = m_bodyText.getResultLineSet();
    for (auto const &line : lineSet) {
      LineNumber ln(line);
      string expectedSection =
          R"(第)" + TurnToString(num.first) + R"(章)" +
          TurnToString(ln.getParaNumber()) + R"(.)" +
          TurnToString(ln.getlineNumber()) + R"(节:)";
      switch (m_fileType) {
      case FILE_TYPE::MAIN:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromMainTemplate(
                "",
                formatIntoZeroPatchedChapterNumber(num.first, m_filenameDigit),
                LINK_DISPLAY_TYPE::DIRECT, m_key, expectedSection, line,
                expectedSection),
            make_pair(ln.getParaNumber(), ln.getlineNumber()));
        break;
      case FILE_TYPE::ORIGINAL:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromOriginalTemplate(
                originalDirForLinkFromMain,
                formatIntoZeroPatchedChapterNumber(num.first, m_filenameDigit),
                m_key, expectedSection, line, expectedSection),
            make_pair(ln.getParaNumber(), ln.getlineNumber()));
        break;
      case FILE_TYPE::JPM:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromJPMTemplate(
                jpmDirForLinkFromMain,
                formatIntoZeroPatchedChapterNumber(num.first, m_filenameDigit),
                m_key, expectedSection, line, expectedSection),
            make_pair(ln.getParaNumber(), ln.getlineNumber()));
        break;
      case FILE_TYPE::ATTACHMENT:
      default:
        FUNCTION_OUTPUT << "not supported yet." << endl;
      }
    }
  }
}

void Searcher::outputSearchResult() {
  m_containerPtr->printParaHeaderTable();
  auto total = m_containerPtr->getLinkStringSetSize();
  m_containerPtr->setMaxTargetAsSetSize();
  m_containerPtr->createParaListFrom(18, 22);
  m_containerPtr->outputToBodyTextFromLinkList(searchUnit);
  string verb = (total > 1) ? "s are" : " is";
  m_containerPtr->assembleBackToHTM(
      "search  results", "search for key: " + m_key + " in " + m_kind +
                             " files. " + TurnToString(total) + " link" + verb +
                             " found.");
  FUNCTION_OUTPUT << "result is in file " << m_containerPtr->getOutputFilePath()
                  << endl;
}

void Searcher::runSearchingOverFiles() {
  if (m_key.empty() and m_keyList.empty()) {
    METHOD_OUTPUT << "no key sepecified." << endl;
    return;
  }
  if (m_outputFilename.empty()) {
    if (not m_key.empty())
      m_outputFilename = m_key;
    else
      m_outputFilename = *(m_keyList.begin());
  }
  m_fileType = getFileTypeFromString(m_kind);
  if (m_asList) {
    m_containerPtr = make_unique<ListContainer>(m_outputFilename);
  } else {
    m_containerPtr = make_unique<TableContainer>(m_outputFilename);
  }
  m_containerPtr->clearLinkStringSet();
  m_fileSet = buildFileSet(m_minTarget, m_maxTarget, m_filenameDigit);

  m_bodyText.setFilePrefixFromFileType(m_fileType);
  runSearchingOverEachFile();
  outputSearchResult();
}

void NonAttachmentSearcher::runSearchingOverEachFile() {
  for (const auto &file : m_fileSet) {
    m_bodyText.setFileAndAttachmentNumber(file);
    execute();
  }
}

void AttachmentSearcher::runSearchingOverEachFile() {
  CoupledBodyTextContainer container;
  for (const auto &file : m_fileSet) {
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(m_minAttachNo, m_maxAttachNo)) {
      m_bodyText.setFileAndAttachmentNumber(file, attNo);
      execute();
    }
  }
}

void searchForImages() {}

void search(int num, const string &key, const string &outputFilename) {
  SEPERATE("HLM search", " started ");
  NonAttachmentSearcher searcher;
  switch (num) {
  case 1:
    searcher.m_kind = MAIN;
    searcher.m_key = key;
    searcher.m_outputFilename = outputFilename;
    searcher.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  case 2:
    searcher.m_kind = ORIGINAL;
    searcher.m_key = key;
    searcher.m_outputFilename = outputFilename;
    searcher.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  case 3:
    searcher.m_kind = JPM;
    searcher.m_key = key;
    searcher.m_outputFilename = outputFilename;
    searcher.m_filenameDigit = THREE_DIGIT_FILENAME;
    searcher.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  default:
    FUNCTION_OUTPUT << "invalid search." << endl;
  }
}
