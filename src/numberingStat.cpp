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
/**
 * output linesTable to file specified in lineDetailFilePath
 */
void CoupledBodyTextWithLink::displayNumberedLines() {
  if (linesTable.empty())
    return;
  FUNCTION_OUTPUT << lineDetailFilePath << " is created." << endl;
  ofstream lineDetailOutfile(lineDetailFilePath);
  for (const auto &para : linesTable) {
    auto paraPos = para.first;
    auto lineList = para.second;
    // para itself
    lineDetailOutfile << referFilePrefix << paraPos.first << ","
                      << paraPos.second << endl;
    // lines included

    // summing up total lines and report over-sized para
  }
}

void CoupledBodyTextWithLink::doStatisticsByScanningLines() {

  m_numberOfFirstParaHeader = 0;
  m_numberOfMiddleParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  linesTable.clear();

  ifstream infile(m_inputFile);
  LineNumber currentPara;

  while (!infile.eof()) // To get you all the lines.
  {
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
      } else if (m_paraHeader.isLastParaHeader()) {
        m_numberOfLastParaHeader++;
        break;
      } else if (m_paraHeader.isMiddleParaHeader())
        m_numberOfMiddleParaHeader++;
      currentPara = ln;
    } else if (isImageGroupLine(m_inLine)) {
      // record the para it belongs to into linesTable
      LineDetails detail{0, true, set<Object::OBJECT_TYPE>()};
      try {
        auto &entry = linesTable.at(
            std::make_pair(m_filePrefix + m_file, currentPara.asString()));
        entry.push_back(detail);
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "entry.size: " << entry.size()
                        << " more reference to para: " << m_filePrefix + m_file
                        << displaySpace << currentPara.asString() << endl;
      } catch (exception &) {
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "create vector for : " << m_filePrefix + m_file
                        << displaySpace << currentPara.asString() << endl;
        vector<LineDetails> list;
        list.push_back(detail);
        linesTable[std::make_pair(m_filePrefix + m_file,
                                  currentPara.asString())] = list;
      }
    } else if (not isEmptyLine(m_inLine)) {
      // record the para it belongs to into linesTable
      LineDetails detail{0, false, set<Object::OBJECT_TYPE>()};
      for (const auto &type : Object::listOfObjectTypes) {
        auto offset = m_inLine.find(Object::getStartTagOfObjectType(type));
        if (offset != string::npos) {
          detail.objectContains.insert(type);
        }
      }
      try {
        auto &entry = linesTable.at(
            std::make_pair(m_filePrefix + m_file, currentPara.asString()));
        entry.push_back(detail);
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "entry.size: " << entry.size()
                        << " more reference to para: " << m_filePrefix + m_file
                        << displaySpace << currentPara.asString() << endl;
      } catch (exception &) {
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "create vector for : " << m_filePrefix + m_file
                        << displaySpace << currentPara.asString() << endl;
        vector<LineDetails> list;
        list.push_back(detail);
        linesTable[std::make_pair(m_filePrefix + m_file,
                                  currentPara.asString())] = list;
      }
    }
  }

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << endl;
    METHOD_OUTPUT << "Result of doStatisticsByScanningLines:" << endl;
    METHOD_OUTPUT << "m_numberOfFirstParaHeader: " << m_numberOfFirstParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfMiddleParaHeader: "
                  << m_numberOfMiddleParaHeader << endl;
    METHOD_OUTPUT << "m_numberOfLastParaHeader: " << m_numberOfLastParaHeader
                  << endl;
    printLinesTable();
  }
}
