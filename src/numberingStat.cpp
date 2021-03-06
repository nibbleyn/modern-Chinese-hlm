#include "coupledBodyTextWithLink.hpp"

CoupledBodyTextWithLink::LinesTable CoupledBodyTextWithLink::linesTable;
CoupledBodyTextWithLink::RangeTable CoupledBodyTextWithLink::rangeTable;
string CoupledBodyTextWithLink::referFilePrefix{emptyString};
string CoupledBodyTextWithLink::lineDetailFilePath{emptyString};

void CoupledBodyTextWithLink::setReferFilePrefix(const string &prefix) {
  referFilePrefix = prefix;
}

void CoupledBodyTextWithLink::setStatisticsOutputFilePath(const string &path) {
  lineDetailFilePath = path;
}

void CoupledBodyTextWithLink::clearExistingNumberingStatistics() {
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "clear content in: " << lineDetailFilePath << endl;
  ofstream outfile(lineDetailFilePath);
}

/**
 * output linesTable to file specified in lineDetailFilePath
 */
void CoupledBodyTextWithLink::appendNumberingStatistics() {
  if (linesTable.empty())
    return;
  FUNCTION_OUTPUT << lineDetailFilePath << " is created." << endl;
  ofstream lineDetailOutfile(lineDetailFilePath, ios_base::app);
  int para = 1;
  auto totalNumberOfLines = 0;
  for (const auto &element : linesTable) {
    auto num = element.first.first;
    auto paraLine = element.first.second;
    LineNumber ln(paraLine.first, paraLine.second);
    // summing up total lines and report over-sized para
    auto detail = element.second;
    string typeStr = (detail.isImgGroup) ? DISPLY_LINE_IMAGE : DISPLY_LINE_TEXT;
    // para itself
    lineDetailOutfile << getFileNameFromAttachmentNumber(num)
                      << referParaMiddleChar << ln.asString() << displaySpace
                      << typeStr << displaySpace
                      << detail.numberOfDisplayedLines << displaySpace
                      << Object::typeSetAsString(detail.objectContains) << endl;
    if (paraLine.first != para) {
      FUNCTION_OUTPUT << "para " << para
                      << " has totalLines: " << totalNumberOfLines << endl;
      totalNumberOfLines = detail.numberOfDisplayedLines + 1;
      para++;
    } else
      totalNumberOfLines += detail.numberOfDisplayedLines + 1;
  }
  FUNCTION_OUTPUT << "para " << para
                  << " has totalLines: " << totalNumberOfLines << endl;
}

void CoupledBodyTextWithLink::addEntriesInRangeTable(AttachmentNumber startNum,
                                                     AttachmentNumber endNum,
                                                     ParaLineNumber startPara,
                                                     ParaLineNumber endPara) {
  // only support non-attachment files now
  if (startNum.first == endNum.first)
    rangeTable[make_pair(startNum, startPara)] = make_pair(startPara, endPara);
  else {
    rangeTable[make_pair(startNum, startPara)] =
        make_pair(startPara, ParaLineNumber(0, 0));
    rangeTable[make_pair(endNum, ParaLineNumber(1, 1))] =
        make_pair(ParaLineNumber(1, 1), endPara);
    for (int i = startNum.first + 1; i < endNum.first; i++) {
      AttachmentNumber num(i, 0);
      rangeTable[make_pair(num, ParaLineNumber(1, 1))] =
          make_pair(ParaLineNumber(1, 1), ParaLineNumber(0, 0));
    }
  }
}

void CoupledBodyTextWithLink::loadRangeTableFromFile(
    const string &indexFilePath) {
  ifstream infile(indexFilePath);
  if (!infile) {
    FUNCTION_OUTPUT << ERROR_FILE_NOT_EXIST << indexFilePath << endl;
    return;
  }
  while (!infile.eof()) {
    string startChapter, startParaLine;
    getline(infile, startChapter, '#');
    if (startChapter.empty())
      break;
    getline(infile, startParaLine, ' ');
    string endChapter, endParaLine;
    getline(infile, endChapter, '#');
    getline(infile, endParaLine);
    if (debug >= LOG_INFO) {
      FUNCTION_OUTPUT << startChapter << displaySpace << startParaLine << endl;
      FUNCTION_OUTPUT << endChapter << displaySpace << endParaLine << endl;
    }
    AttachmentNumber startNum = getAttachmentNumber(startChapter, false),
                     endNum = getAttachmentNumber(endChapter, false);
    LineNumber startPara(startParaLine), endPara(endParaLine);
    ParaLineNumber start(startPara.getParaNumber(), startPara.getlineNumber());
    ParaLineNumber end(endPara.getParaNumber(), endPara.getlineNumber());
    addEntriesInRangeTable(startNum, endNum, start, end);
  }
  if (debug >= LOG_INFO)
    printRangeTable();
}

void CoupledBodyTextWithLink::doStatisticsByScanningLines(
    bool overFixedBodyText) {

  m_numberOfFirstParaHeader = 0;
  m_numberOfMiddleParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  linesTable.clear();

  ifstream infile;
  if (overFixedBodyText)
    infile.open(m_outputFile);
  else
    infile.open(m_inputFile);

  auto num = make_pair(TurnToInt(m_file), m_attachNumber);
  if (not m_disableNumberingStatisticsCalculateLines)
    getLinesofReferencePage();

  m_para = 0;
  m_lineNo = 1;
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
        m_para = 1;
        m_lineNo = 1;
      } else if (m_paraHeader.isLastParaHeader()) {
        break;
      } else if (m_paraHeader.isMiddleParaHeader()) {
        m_para++;
        m_lineNo = 1;
      }
    } else if (isImageGroupLine(m_inLine)) {
      // record the para it belongs to into linesTable
      LineDetails detail{0, true, Object::SET_OF_OBJECT_TYPES()};
      auto paraLine = make_pair(m_para, m_lineNo);
      linesTable[make_pair(num, paraLine)] = detail;
      m_lineNo++;
    } else if (hasEndingBr(m_inLine)) {
      // record the para it belongs to into linesTable
      size_t dispLines = 0;
      auto types = Object::SET_OF_OBJECT_TYPES();
      if (not m_disableNumberingStatisticsCalculateLines) {
        auto removeEndingBrStr = m_inLine.substr(0, m_inLine.find(brTab));
        dispLines = getLinesOfDisplayText(getDisplayString(removeEndingBrStr));
        types = getContainedObjectTypes(removeEndingBrStr);
      }
      LineDetails detail{dispLines, false, types};
      auto paraLine = make_pair(m_para, m_lineNo);
      linesTable[make_pair(num, paraLine)] = detail;
      m_lineNo++;
    }
  }

  if (debug >= LOG_INFO) {
    printLinesTable();
  }
  appendNumberingStatistics();
}
