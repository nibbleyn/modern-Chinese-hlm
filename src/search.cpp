#include "search.hpp"

void Searcher::execute() {
  m_bodyText.resetBeforeSearch();
  m_bodyText.searchForAll();
  if (m_fileType == FILE_TYPE::MAIN)
    m_bodyText.ignorePersonalComments();
  auto num = m_bodyText.getFileAndAttachmentNumber();
  if (m_overSpecificObject)
    for (const auto &line :
         CoupledBodyTextWithLink::getLineNumberMissingObjectType(
             num, m_overObjects)) {
      m_bodyText.addIgnoreLines(line);
    }
  bool found = m_bodyText.findKey(m_key);
  if (found) {
    CoupledBodyText::lineNumberSet lineSet = m_bodyText.getResultLineSet();
    for (auto const &line : lineSet) {
      LineNumber ln(line);
      ParaLineNumber paraLine =
          make_pair(ln.getParaNumber(), ln.getlineNumber());
      string expectedSection = getExpectedSection(num, paraLine);
      switch (m_fileType) {
      case FILE_TYPE::MAIN:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromMainTemplate(
                emptyString,
                getChapterNameByTargetKind(MAIN_TYPE_HTML_TARGET, num.first),
                LINK_DISPLAY_TYPE::DIRECT, m_key, expectedSection,
                expectedSection, line),
            make_pair(ln.getParaNumber(), ln.getlineNumber()));
        break;
      case FILE_TYPE::ORIGINAL:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromOriginalTemplate(
                originalDirForLinkFromMain,
                getChapterNameByTargetKind(ORIGINAL_TYPE_HTML_TARGET,
                                           num.first),
                m_key, expectedSection, expectedSection, line),
            make_pair(ln.getParaNumber(), ln.getlineNumber()));
        break;
      case FILE_TYPE::JPM:
        m_containerPtr->addLinkToLinkStringSet(
            num,
            fixLinkFromJPMTemplate(
                jpmDirForLinkFromMain,
                getChapterNameByTargetKind(JPM_TYPE_HTML_TARGET, num.first),
                m_key, expectedSection, expectedSection, line),
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
  string defaultTile = "search for key: " + m_key + ", executed at " +
                       currentDateTime() + " in " + m_kind + " files";
  if (m_overSpecificObject)
    defaultTile += R"( (only among poems))";
  defaultTile += ".  --->  ";
  m_containerPtr->setTitle(R"(search  results)");
  m_containerPtr->setDisplayTitle(defaultTile + TurnToString(total) + " link" +
                                  verb + " found.");
  m_containerPtr->disableBodyTextWithEndMark();
  m_containerPtr->assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file "
                  << m_containerPtr->getoutputHtmlFilepath() << endl;
}

void Searcher::runSearchingOverFiles() {
  if (m_key.empty() and m_keyList.empty()) {
    METHOD_OUTPUT << "no key specified." << endl;
    return;
  }
  if (m_outputFilename.empty()) {
    if (not m_key.empty())
      m_outputFilename = m_key;
    else
      m_outputFilename = *(m_keyList.begin());
  }
  m_fileType = getFileTypeFromKind(m_kind);
  loadObjectList();
  if (m_asList) {
    m_containerPtr = make_unique<ListContainer>(m_outputFilename);
  } else {
    m_containerPtr = make_unique<TableContainer>(m_outputFilename);
  }
  m_containerPtr->clearLinkStringSet();
  m_fileSet = buildFileSet(m_minTarget, m_maxTarget, m_kind);

  m_bodyText.setFilePrefixFromFileType(m_fileType);
  runSearchingOverEachFile();
  outputSearchResult();
}

void Searcher::loadObjectList() {
  if (m_overSpecificObject) {
    CoupledBodyTextWithLink::setReferFilePrefix(
        getBodyTextFilePrefixFromFileType(m_fileType));
    CoupledBodyTextWithLink::setStatisticsOutputFilePath(
        getStatisticsOutputFilePathFromString(m_kind));
    CoupledBodyTextWithLink::loadNumberingStatistics();
  }
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

void search(const string &key, int targetKind, bool overSpecificObject,
            const string &outputFilename) {
  SEPERATE("HLM search", " started ");
  NonAttachmentSearcher searcher;
  searcher.m_key = key;
  searcher.m_overSpecificObject = overSpecificObject;
  searcher.m_outputFilename = outputFilename;
  switch (targetKind) {
  case SEARCH_IN_MAIN:
    searcher.m_kind = MAIN;
    searcher.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  case SEARCH_IN_ORIGINAL:
    searcher.m_kind = ORIGINAL;
    searcher.m_minTarget = MAIN_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = MAIN_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  case SEARCH_IN_JPM:
    searcher.m_kind = JPM;
    searcher.m_minTarget = JPM_MIN_CHAPTER_NUMBER;
    searcher.m_maxTarget = JPM_MAX_CHAPTER_NUMBER;
    searcher.runSearchingOverFiles();
    break;
  default:
    FUNCTION_OUTPUT << "invalid search." << endl;
  }
}
