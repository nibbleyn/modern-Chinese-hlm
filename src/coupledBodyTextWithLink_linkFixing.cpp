#include "coupledBodyTextWithLink.hpp"

void CoupledBodyTextWithLink::fixLinksWithinOneLine(FileSet referMainFiles,
                                                    FileSet referOriginalFiles,
                                                    FileSet referJPMFiles) {
  LineNumberPlaceholderLink ln;
  ln.loadFirstFromContainedLine(m_inLine);
  string toProcess = m_inLine;
  auto processBegin = 0;
  // skip link of line number
  if (ln.get().valid()) {
    toProcess = toProcess.substr(ln.get().generateLinePrefix().length());
    processBegin = ln.get().generateLinePrefix().length();
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << toProcess << endl;
  if (m_filePrefix == MAIN_BODYTEXT_PREFIX) {
    m_linkPtr = make_unique<LinkFromMain>(m_file);
  } else if (m_filePrefix == JPM_BODYTEXT_PREFIX) {
    m_linkPtr = make_unique<LinkFromJPM>(m_file);
  } else if (m_filePrefix == ATTACHMENT_BODYTEXT_PREFIX) {
    m_linkPtr = make_unique<LinkFromAttachment>(
        m_file + attachmentFileMiddleChar + TurnToString(m_attachNumber));
  }

  string targetFile{emptyString};
  unsigned int endOfProcessedSubString = 0;
  do {
    // no link any more, continue with next line
    if (toProcess.find(linkStartChars) == string::npos)
      break;
    auto linkEnd = toProcess.find(linkEndChars, toProcess.find(linkStartChars));
    auto link =
        getWholeStringBetweenTags(toProcess, linkStartChars, linkEndChars);
    m_linkPtr->deleteLinkToOrigin();
    m_linkPtr->readTypeAndAnnotation(link);
    // second step of construction, this is needed to check isTargetToSelfHtm
    m_linkPtr->readReferFileName(link);
    if (m_linkPtr->isTargetToOtherAttachmentHtm()) {
      // third step of construction
      m_linkPtr->fixFromString(link);
      m_linkPtr->setSourcePara(ln.get());
      m_linkPtr->doFixStatistics();
    }
    if (m_linkPtr->isTargetToImage() or m_linkPtr->isTargetToSelfHtm()) {
      m_linkPtr->setSourcePara(ln.get());
      // third step of construction
      m_linkPtr->fixFromString(link);
      // replace old value
      if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
        auto orglinkBegin = m_inLine.find(link, endOfProcessedSubString);
        m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        endOfProcessedSubString =
            orglinkBegin + m_linkPtr->getFormatedFullString().length();
      }
    }
    if (m_linkPtr->isTargetToOtherMainHtm() and
        not m_linkPtr->isReverseLink()) {
      auto e = find(referMainFiles.begin(), referMainFiles.end(),
                    m_linkPtr->getChapterName());
      // need to check and fix
      if (e != referMainFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        m_linkPtr->setSourcePara(ln.get());
        string next = originalLinkStartChars + linkStartChars;
        bool needAddOrginalLink = true;
        // still have above "next" and </a>
        if (toProcess.length() >
            (link.length() + next.length() + linkEndChars.length())) {
          if (toProcess.substr(linkEnd + linkEndChars.length(),
                               next.length()) == next) {
            // skip </a> and first parenthesis of next
            auto followingLink = getWholeStringBetweenTags(
                toProcess, linkStartChars, linkEndChars, linkEnd);
            if (m_attachNumber == 0) {
              m_followingLinkPtr =
                  make_unique<LinkFromMain>(m_file, followingLink);
            } else {
              m_followingLinkPtr = make_unique<LinkFromAttachment>(
                  m_file + attachmentFileMiddleChar +
                      TurnToString(m_attachNumber),
                  followingLink);
            }
            if (m_followingLinkPtr->isTargetToOriginalHtm()) {
              needAddOrginalLink = false;
            }
          }
        }
        if (needAddOrginalLink)
          m_linkPtr->generateLinkToOrigin();
        m_linkPtr->doFixStatistics();
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link, endOfProcessedSubString);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          endOfProcessedSubString =
              orglinkBegin + m_linkPtr->getFormatedFullString().length();
        }
      }
    }
    if (m_linkPtr->isTargetToJPMHtm()) {
      auto e = find(referJPMFiles.begin(), referJPMFiles.end(),
                    m_linkPtr->getChapterName());
      // need to check and fix
      if (e != referJPMFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link, endOfProcessedSubString);
          if (debug >= LOG_INFO)
            SEPERATE("isTargetToJPMHtm", m_inLine + "\n" + link);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          endOfProcessedSubString =
              orglinkBegin + m_linkPtr->getFormatedFullString().length();
        }
      }
    }
    if (m_linkPtr->isTargetToOriginalHtm()) {
      auto e = find(referOriginalFiles.begin(), referOriginalFiles.end(),
                    m_linkPtr->getChapterName());
      // need to check and fix
      if (e != referOriginalFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link, endOfProcessedSubString);
          if (debug >= LOG_INFO)
            SEPERATE("isTargetToOriginalHtm", m_inLine + "\n" + link);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          endOfProcessedSubString =
              orglinkBegin + m_linkPtr->getFormatedFullString().length();
        }
      }
    }
    // continue to find next link in the m_inLine
    toProcess = toProcess.substr(linkEnd + linkEndChars.length());
  } while (true);
  if (m_forceRemoveDuplicatedOriginalLinks)
    removeDuplicatedOriginalLinks(processBegin);
}

void CoupledBodyTextWithLink::removeDuplicatedOriginalLinks(
    size_t processBegin) {
  using OrgLinkSet = set<string>;
  OrgLinkSet orgLinkSet;
  using ToDeleteOrgLinkOffsetTable = map<size_t, size_t, std::greater<size_t>>;
  ToDeleteOrgLinkOffsetTable toDelete;
  auto start = processBegin;
  while (true) {
    auto orglinkBegin = m_linkPtr->loadFirstFromContainedLine(m_inLine, start);
    if (orglinkBegin == string::npos)
      break;
    auto link = m_linkPtr->asString();
    start = orglinkBegin + link.length();
    if (not m_linkPtr->isTargetToOriginalHtm())
      continue;
    bool alreadyContainsThisLink = orgLinkSet.count(link) != 0;
    if (alreadyContainsThisLink) {
      toDelete[orglinkBegin] = link.length();
    } else
      orgLinkSet.insert(link);
  }
  for (const auto &offset : toDelete) {
    m_inLine.replace(offset.first, offset.second, emptyString);
  }
}

/**
 * fix links of certain type in file which refer to one of file in referFiles
 */
void CoupledBodyTextWithLink::fixLinksFromFile(FileSet referMainFiles,
                                               FileSet referOriginalFiles,
                                               FileSet referJPMFiles,
                                               int minPara, int maxPara,
                                               int minLine, int maxLine) {
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  while (!infile.eof()) {
    getline(infile, m_inLine);
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isPartOfParagraphHeader() or not ln.get().valid() or
        not ln.get().isWithinLineRange(minPara, maxPara, minLine, maxLine)) {
      outfile << m_inLine << endl;
      continue;
    }
    fixLinksWithinOneLine(referMainFiles, referOriginalFiles, referJPMFiles);
    outfile << m_inLine << endl;
  }
}
