#include "coupledBodyTextWithLink.hpp"

CoupledBodyTextWithLink::LinesTable CoupledBodyTextWithLink::linesTable;
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
                      << "has totalLines: " << totalNumberOfLines << endl;
      totalNumberOfLines = 0;
      para++;
    } else
      totalNumberOfLines += 1;
  }
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
      LineDetails detail{0, false, getContainedObjectTypes(m_inLine)};
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
