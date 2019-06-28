#include "linkSetContainer.hpp"

extern int debug;

/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void LinkSetContainer::clearExistingBodyText() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "clear content in: " << getBodyTextFilePath() << endl;
  ofstream outfile(getBodyTextFilePath());
}

// if max is 0, must call setMaxTarget or setMaxTargetAsSetSize before
void LinkSetContainer::createParaListFrom(int first, int incremental, int max) {
  if (max == 0)
    max = m_maxTarget;
  m_paraHeaderPositionSet.clear();
  m_paraHeaderPositionSet.push_back(first);
  int i = first;
  while ((i += incremental) < max) {
    m_paraHeaderPositionSet.push_back(i);
  }
}

/**
 * append a link string in the body text of final htm file
 * @param linkString the string to put into
 * @param containerNumber the selected container to put into
 */
void ListContainer::appendParagraphInBodyText(const string &text) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << getBodyTextFilePath()
                  << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  outfile << "<br>" << text << "</br>" << endl;
}

void ListContainer::appendParagrapHeader(const string &header) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << getBodyTextFilePath()
                  << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  outfile << header << endl;
}

void ListContainer::outputToBodyTextFromLinkList(const string &units) {
  clearExistingBodyText();
  for (const auto &link : m_linkStringSet) {
    appendParagraphInBodyText(link.second);
  }
}

const string TableContainer::BODY_TEXT_STARTER = R"(3front.txt)";
const string TableContainer::BODY_TEXT_DESSERT = R"(3back.txt)";

void TableContainer::addExistingFrontLinks() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "init content in: " << getBodyTextFilePath() << endl;
  ofstream outfile(getBodyTextFilePath(), ios_base::app);
  // copy content from BODY_TEXT_STARTER
  string starterFile = m_inputBodyTextDir + BODY_TEXT_STARTER;

  ifstream inStarterFile(starterFile);
  if (!inStarterFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << starterFile << endl;
    return;
  }

  while (!inStarterFile.eof()) {
    string line{emptyString};
    getline(inStarterFile, line);
    // excluding start line
    outfile << line << endl;
  }
}

void TableContainer::finishBodyTextFile() {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << getBodyTextFilePath() << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  // copy content from BODY_TEXT_DESSERT
  string dessertFile = m_inputBodyTextDir + BODY_TEXT_DESSERT;

  ifstream inDessertFile(dessertFile);
  if (!inDessertFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << dessertFile << endl;
    return;
  }

  while (!inDessertFile.eof()) {
    string line{emptyString};
    getline(inDessertFile, line);
    // excluding start line
    outfile << line << endl;
  }
}

void TableContainer::insertFrontParagrapHeader(int totalPara,
                                               const string &units) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << getBodyTextFilePath() << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath());
  GenericParaHeader paraHeader;
  paraHeader.setTotalParaNumber(totalPara);
  paraHeader.setUnits(units);
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  string line = paraHeader.getFixedResult();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << line << endl;
  outfile << line << endl;
}

void TableContainer::insertMiddleParagrapHeader(bool enterLastPara,
                                                int seqOfPara, int startParaNo,
                                                int endParaNo, int totalPara,
                                                int preTotalPara,
                                                const string &units) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << getBodyTextFilePath() << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  GenericParaHeader paraHeader;
  paraHeader.setTotalParaNumber(totalPara);
  paraHeader.setpreTotalParaNumber(preTotalPara);
  paraHeader.setSeqOfPara(seqOfPara);
  paraHeader.setStartParaNo(startParaNo);
  paraHeader.setEndParaNo(endParaNo);
  paraHeader.setUnits(units);
  paraHeader.markAsMiddleParaHeader();
  paraHeader.markAsLastMiddleParaHeader(enterLastPara);
  paraHeader.fixFromTemplate();
  string line = paraHeader.getFixedResult();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << line << endl;
  outfile << line << endl;
}

void TableContainer::insertBackParagrapHeader(int seqOfPara, int totalPara,
                                              const string &units) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << getBodyTextFilePath() << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  GenericParaHeader paraHeader;
  paraHeader.setTotalParaNumber(totalPara);
  paraHeader.setSeqOfPara(seqOfPara);
  paraHeader.setUnits(units);
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  string line = paraHeader.getFixedResult();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << line << endl;
  outfile << line << endl;
}

void TableContainer::appendLeftParagraphInBodyText(const string &text) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << getBodyTextFilePath()
                  << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  outfile << R"(<tr><td width="50%">)" << text << "</td>" << endl;
}

void TableContainer::appendRightParagraphInBodyText(const string &text) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << getBodyTextFilePath()
                  << endl;
  ofstream outfile;
  outfile.open(getBodyTextFilePath(), ios_base::app);
  outfile << R"(<td width="50%">)" << text << R"(</td></tr>)" << endl;
}

void TableContainer::outputToBodyTextFromLinkList(const string &units) {
  clearExistingBodyText();
  sort(m_paraHeaderPositionSet.begin(), m_paraHeaderPositionSet.end());
  auto start = m_paraHeaderPositionSet.begin();
  auto lessThanOnePara = (m_maxTarget < *start);
  if (not m_hideParaHeaders and not lessThanOnePara)
    insertFrontParagrapHeader(*start, units);
  if (m_enableAddExistingFrontLinks)
    addExistingFrontLinks();
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &link : m_linkStringSet) {
    if (i % 2 == 0)
      appendRightParagraphInBodyText(link.second);
    else
      appendLeftParagraphInBodyText(link.second);
    if (start != m_paraHeaderPositionSet.end() and i == *start) {
      auto enterLastPara = (start + 1 == m_paraHeaderPositionSet.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? m_maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      if (not m_hideParaHeaders)
        insertMiddleParagrapHeader(enterLastPara, seqOfPara, startParaNo,
                                   endParaNo, totalPara, preTotalPara, units);
      seqOfPara++;
      start++;
    }
    i++;
  }
  if (not m_hideParaHeaders and not lessThanOnePara)
    insertBackParagrapHeader(seqOfPara, totalPara, units);
}
