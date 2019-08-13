#include "coupledBodyTextWithLink.hpp"

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
  if (not fileExist(referLinesFile, REFERENCE_LINES))
    return 0;

  auto totalSizes = 0;
  auto totalLines = 0;
  while (!referLinesFile.eof()) {
    string line{emptyString};
    getline(referLinesFile, line);
    line = regex_replace(line, regex("(?:\\r\\n|\\n|\\r)"), emptyString);
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
  m_SizeOfReferPage = 0;
  ifstream referPageFile(REFERENCE_PAGE);
  if (not fileExist(referPageFile, REFERENCE_PAGE))
    return;
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

void CoupledBodyTextWithLink::adjustParaHeaderStartNumber() {
  if (m_numberOfMiddleParaHeader > START_PARA_NUMBER) {
    if (debug >= LOG_INFO) {
      // excluding start line
      METHOD_OUTPUT << "number of middle paras are more than default: "
                    << START_PARA_NUMBER << endl;
      METHOD_OUTPUT << "adjust start number to: "
                    << (m_numberOfMiddleParaHeader / 10 + 1) * 10 << endl;
    }
    m_paraHeader.setStartNumber((m_numberOfMiddleParaHeader / 10 + 1) * 10);
  }
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
  if (not fileExist(infile, m_inputFile))
    return;
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
  if (not fileExist(infile, m_inputFile))
    return;

  size_t seqOfLines = 0;
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << m_inLine << endl;
    }
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isPartOfParagraphHeader()) {
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
      getDisplayString(m_inLine.substr(0, lastBr));
      string dispLine = getResultDisplayString();
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
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);

  if (isAutoNumbering()) {
    getLinesofReferencePage();
    // first scan
    scanByRenderingLines();
    calculateParaHeaderPositions();
    adjustParaHeaderStartNumber();
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

void CoupledBodyTextWithLink::appendReverseLinks() {
  PersonalComment obj("");
  obj.setBodytext(R"(邢夫人)");
  obj.hide();
  cout << obj.getFormatedFullString() << endl;
};
