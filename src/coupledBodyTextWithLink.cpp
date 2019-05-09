#include "coupledBodyTextWithLink.hpp"

void CoupledBodyTextWithLink::fixLinksWithinOneLine(FileSet referMainFiles,
                                                    FileSet referOriginalFiles,
                                                    FileSet referJPMFiles) {

  LineNumber ln;
  ln.loadFirstFromContainedLine(m_inLine);
  string toProcess = m_inLine;
  // skip link of line number
  if (ln.valid())
    toProcess = toProcess.substr(ln.generateLinePrefix().length());
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << toProcess << endl;
  string targetFile{""};
  do {
    // no link any more, continue with next line
    if (toProcess.find(linkStartChars) == string::npos)
      break;
    auto linkEnd = toProcess.find(linkEndChars, toProcess.find(linkStartChars));
    auto link =
        getWholeStringBetweenTags(toProcess, linkStartChars, linkEndChars);
    if (m_filePrefix == MAIN_BODYTEXT_PREFIX) {
      m_linkPtr = make_unique<LinkFromMain>(m_file, link);
    } else if (m_filePrefix == JPM_BODYTEXT_PREFIX) {
      m_linkPtr = make_unique<LinkFromJPM>(m_file, link);
    } else if (m_filePrefix == ATTACHMENT_BODYTEXT_PREFIX) {
      m_linkPtr = make_unique<LinkFromAttachment>(
          m_file + attachmentFileMiddleChar + TurnToString(m_attachNumber),
          link);
    }
    // second step of construction, this is needed to check isTargetToSelfHtm
    m_linkPtr->readReferFileName(link);
    if (m_linkPtr->isTargetToOtherAttachmentHtm()) {
      // third step of construction
      m_linkPtr->fixFromString(link);
      m_linkPtr->setSourcePara(ln);
      m_linkPtr->doStatistics();
    }
    if (m_linkPtr->isTargetToSelfHtm()) {
      m_linkPtr->setSourcePara(ln);
      // third step of construction
      m_linkPtr->fixFromString(link);
      // replace old value
      if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
        auto orglinkBegin = m_inLine.find(link);
        m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
      }
    }
    if (m_linkPtr->isTargetToOtherMainHtm()) {
      auto e = find(referMainFiles.begin(), referMainFiles.end(),
                    getChapterNumberString(m_linkPtr->getchapterNumer()));
      // need to check and fix
      if (e != referMainFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        m_linkPtr->setSourcePara(ln);
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
        m_linkPtr->doStatistics();
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        }
      }
    }
    if (m_linkPtr->isTargetToJPMHtm()) {
      auto e = find(referJPMFiles.begin(), referJPMFiles.end(),
                    getChapterNumberString(m_linkPtr->getchapterNumer()));
      // need to check and fix
      if (e != referJPMFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link);
          if (debug >= LOG_INFO)
            SEPERATE("isTargetToJPMHtm", m_inLine + "\n" + link);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        }
      }
    }
    if (m_linkPtr->isTargetToOriginalHtm()) {
      auto e = find(referOriginalFiles.begin(), referOriginalFiles.end(),
                    getChapterNumberString(m_linkPtr->getchapterNumer()));
      // need to check and fix
      if (e != referOriginalFiles.end()) {
        // third step of construction
        m_linkPtr->fixFromString(link);
        // replace old value
        if (m_forceUpdateLink or m_linkPtr->needUpdate()) {
          auto orglinkBegin = m_inLine.find(link);
          if (debug >= LOG_INFO)
            SEPERATE("isTargetToOriginalHtm", m_inLine + "\n" + link);
          m_inLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        }
      }
    }
    // continue to find next link in the m_inLine
    toProcess = toProcess.substr(linkEnd + linkEndChars.length());
  } while (true);
}

/**
 * only based on length of string. So font must be same for all characters
 */
size_t
CoupledBodyTextWithLink::getLinesOfDisplayText(const string &dispString) {
  if (m_averageSizeOfOneLine == 0) {
    METHOD_OUTPUT
        << "getAverageLineLengthFromReferenceFile must be called before."
        << endl;
    return 0;
  }
  auto totalLines = 1;
  int size = utf8length(dispString);
  if (size != 0) {
    size_t end = -1;
    totalLines = 0;
    bool firstline = true;
    while (size > 0) {
      // should be +2 and 0 if more accurate
      // but to be conservative here
      // avoid to return lines less than really displayed
      auto cutSize = (firstline) ? (m_averageSizeOfOneLine + 1)
                                 : (m_averageSizeOfOneLine - 1);
      auto cutLine = utf8substr(dispString, end + 1, end, cutSize);
      size -= cutSize;
      if (firstline)
        firstline = false;
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << cutLine << endl;
        METHOD_OUTPUT << utf8length(cutLine) << endl;
        METHOD_OUTPUT << size << endl;
      }
      totalLines++;
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << totalLines << endl;
      }
    }
  }
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << totalLines << endl;
  }
  return totalLines;
}

size_t CoupledBodyTextWithLink::getAverageLineLengthFromReferenceFile() {
  ifstream referLinesFile(REFERENCE_LINES);

  if (!referLinesFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << REFERENCE_LINES
                  << endl;
    return 0;
  }

  auto totalSizes = 0;
  auto totalLines = 0;

  while (!referLinesFile.eof()) {
    string line{""};
    getline(referLinesFile, line);
    line = regex_replace(line, regex("(?:\\r\\n|\\n|\\r)"), "");
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << line << endl;
      METHOD_OUTPUT << utf8length(line) << endl;
    }
    if (not line.empty()) {
      totalSizes += utf8length(line);
      totalLines++;
    }
  }
  return totalSizes / totalLines;
}

void CoupledBodyTextWithLink::getLinesofReferencePage() {
  m_averageSizeOfOneLine = getAverageLineLengthFromReferenceFile();
  ifstream referPageFile(REFERENCE_PAGE);
  if (!referPageFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << REFERENCE_PAGE << endl;
    m_SizeOfReferPage = 0;
  }
  auto totalLines = 0;
  string line;
  while (!referPageFile.eof()) {
    getline(referPageFile, line);
    totalLines += getLinesOfDisplayText(line);
    if (debug >= LOG_INFO) {
      // excluding start line
      METHOD_OUTPUT << line << endl;
      METHOD_OUTPUT << totalLines << endl;
    }
  }
  m_SizeOfReferPage = totalLines;
}

void CoupledBodyTextWithLink::calculateParaHeaderPositions() {

  m_numberOfMiddleParaHeader = 0;

  size_t totalLines = 0;
  size_t paraNo = 1;
  size_t lastAdded = 0;
  for (const auto &element : m_lineAttrTable) {
    if (totalLines + element.second.numberOfLines > m_SizeOfReferPage) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << "add para here" << endl;
      ParaHeaderInfo info{paraNo++, totalLines};
      m_paraHeaderTable[lastAdded] = info;
      lastAdded = element.first;
      totalLines = element.second.numberOfLines;
    } else {
      lastAdded = element.first;
      totalLines += element.second.numberOfLines;
    }
  }
  if (m_numberOfLastParaHeader != 0) {
    // reset this value
    m_numberOfMiddleParaHeader = paraNo - 1;
    ParaHeaderInfo info{paraNo, 0};
    m_paraHeaderTable[m_lastSeqNumberOfLine] = info;
  }
  if (debug >= LOG_INFO) {
    printParaHeaderTable();
    METHOD_OUTPUT << "m_numberOfMiddleParaHeader: "
                  << m_numberOfMiddleParaHeader << endl;
  }
}

void CoupledBodyTextWithLink::paraGeneratedNumbering() {
  ifstream infile(m_inputFile);
  ofstream outfile(m_outputFile);

  size_t seqOfLines = 0;
  m_para = 0;
  m_lineNo = 1;
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << seqOfLines << ": " << m_inLine << endl;
    }
    auto inLineTable = isInLineAttrTable(seqOfLines);
    // first and last para headers
    // pure empty line or non-last BRs after imageGroup
    if (inLineTable == false) {
      // only the first and last para headers
      if (isInParaHeaderTable(seqOfLines)) {
        addParaHeader(outfile);
        if (m_para == m_numberOfMiddleParaHeader + 1) {
          break;
        }
      } else
        outfile << m_inLine << endl;
    } else {
      // discard old BRs and para headers
      if (m_lineAttrTable[seqOfLines].numberOfLines != 0) {
        if (m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::IMAGE or
            m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::EMPTY) {
          outfile << m_inLine << endl;
        } else if (m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::TEXT)
          // needs numbering
          numberingLine(outfile);
      }
      // needs to append para header afterwards
      if (isInParaHeaderTable(seqOfLines)) {
        auto patchBrs =
            m_SizeOfReferPage - m_paraHeaderTable[seqOfLines].totalLinesAbove;
        while (patchBrs-- > 0)
          outfile << brTab << endl;
        addParaHeader(outfile);
      }
    }
    seqOfLines++;
  }
}

void CoupledBodyTextWithLink::scanByRenderingLines() {
  m_numberOfFirstParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  m_lineAttrTable.clear();
  m_paraHeaderTable.clear();

  ifstream infile(m_inputFile);

  size_t seqOfLines = 0;
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << m_inLine << endl;
    }
    LineNumber ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isParagraphHeader()) {
      m_paraHeader.loadFrom(m_inLine);
      if (m_paraHeader.isFirstParaHeader()) {
        m_numberOfFirstParaHeader++;
        ParaHeaderInfo info{0, 0};
        m_paraHeaderTable[seqOfLines] = info;
      } else if (m_paraHeader.isLastParaHeader()) {
        m_numberOfLastParaHeader++;
        m_lastSeqNumberOfLine = seqOfLines;
        break;
      } else {
        // any old middle para headers would not output
        LineInfo info{0, DISPLY_LINE_TYPE::PARA, "para"};
        m_lineAttrTable[seqOfLines] = info;
      }
    } else if (isLeadingBr(m_inLine)) {
      LineInfo info{0, DISPLY_LINE_TYPE::EMPTY, "<BR>"};
      m_lineAttrTable[seqOfLines] = info;
    } else if (isImageGroupLine(m_inLine)) {
      LineInfo info{m_SizeOfReferPage - 1, DISPLY_LINE_TYPE::IMAGE, "image"};
      m_lineAttrTable[seqOfLines] = info;
    } else if (not isEmptyLine(m_inLine)) {
      auto lastBr = m_inLine.find(brTab);
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << m_inLine.substr(0, lastBr) << endl;
      }
      string dispLine = getDisplayString(m_inLine.substr(0, lastBr));
      if (debug >= LOG_INFO) {
        // excluding start line
        METHOD_OUTPUT << dispLine << endl;
        METHOD_OUTPUT << utf8length(dispLine) << endl;
        METHOD_OUTPUT << getLinesOfDisplayText(dispLine) << endl;
      }
      size_t end = -1;
      LineInfo info{getLinesOfDisplayText(dispLine), DISPLY_LINE_TYPE::TEXT,
                    utf8substr(dispLine, 0, end, 5)};
      m_lineAttrTable[seqOfLines] = info;
      // try to merge leading BR of this line
      try {
        auto previousLine = m_lineAttrTable.at(seqOfLines - 1);
        // so that this leading BR would output
        if (previousLine.type == DISPLY_LINE_TYPE::EMPTY)
          m_lineAttrTable.erase(seqOfLines - 1);
        m_lineAttrTable[seqOfLines].numberOfLines++;
      } catch (exception &) {
        METHOD_OUTPUT << "no leading br at line" << seqOfLines - 1 << endl;
        exit(1);
      }
    }
    seqOfLines++;
  }

  using ToDeleteLines = set<size_t>;
  ToDeleteLines lineSet;

  bool deleteBR = false;
  size_t BRDeleted = 0;
  size_t lastDeleted = 0;
  for (const auto &element : m_lineAttrTable) {
    if (deleteBR == true) {
      if (element.second.type == DISPLY_LINE_TYPE::EMPTY) {
        lastDeleted = element.first;
        lineSet.insert(lastDeleted);
        BRDeleted++;
      } else {
        deleteBR = false;
        if (BRDeleted > 0) {
          // except for this line, all other leading BRs would not output
          lineSet.erase(lastDeleted);
          m_lineAttrTable[lastDeleted].numberOfLines = 1;
        }
      }
    }
    if (deleteBR == false and element.second.type == DISPLY_LINE_TYPE::IMAGE) {
      deleteBR = true;
      BRDeleted = 0;
    }
  }

  // delete all leading BRs after image group so that they would output
  for (const auto &element : lineSet) {
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << element << endl;
    m_lineAttrTable.erase(element);
  }

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "Result of scanByRenderingLines:" << endl;
    METHOD_OUTPUT << "m_numberOfFirstParaHeader: " << m_numberOfFirstParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfLastParaHeader: " << m_numberOfLastParaHeader
                  << endl;
    METHOD_OUTPUT << "m_lastSeqNumberOfLine: " << m_lastSeqNumberOfLine << endl;
    printLineAttrTable();
  }
}

void CoupledBodyTextWithLink::validateParaSize() {
  scanByRenderingLines();
  printOversizedLines();
}

void CoupledBodyTextWithLink::addLineNumber() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  if (!outfile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << m_outputFile << endl;
    return;
  }

  if (isAutoNumbering()) {
    getLinesofReferencePage();
    // first scan
    scanByRenderingLines();
    calculateParaHeaderPositions();
    paraGeneratedNumbering();
  } else {
    // first scan
    scanByLines();
    paraGuidedNumbering();
  }
  if (isNumberingStatistics())
    doStatisticsByScanningLines(true);

  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "numbering finished." << endl;
}

/**
 * fix links of certain type in file which refer to one of file in referFiles
 * @param file
 * @param referFiles
 */
void CoupledBodyTextWithLink::fixLinksFromFile(FileSet referMainFiles,
                                               FileSet referOriginalFiles,
                                               FileSet referJPMFiles,
                                               int minPara, int maxPara,
                                               int minLine, int maxLine) {
  setInputOutputFiles();

  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  while (!infile.eof()) {
    getline(infile, m_inLine);
    LineNumber ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isParagraphHeader() or not ln.valid() or
        not ln.isWithinLineRange(minPara, maxPara, minLine, maxLine)) {
      outfile << m_inLine << endl;
      continue;
    }
    fixLinksWithinOneLine(referMainFiles, referOriginalFiles, referJPMFiles);
    outfile << m_inLine << endl;
  }
}
