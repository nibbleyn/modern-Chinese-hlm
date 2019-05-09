#include "linkSetContainer.hpp"

extern int debug;
/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void LinkSetContainer::clearExistingBodyText() {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "clear content in: " << outputBodyTextFile << endl;
  ofstream outfile(outputBodyTextFile);
}

/**
 * append a link string in the body text of final htm file
 * @param linkString the string to put into
 * @param containerNumber the selected container to put into
 */
void ListContainer::appendParagraphInBodyText(const string &text) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
  outfile << "<br>" << text << "</br>" << endl;
}

void ListContainer::appendParagrapHeader(const string &header) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
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
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "init content in: " << outputBodyTextFile << endl;
  ofstream outfile(outputBodyTextFile, ios_base::app);
  // copy content from BODY_TEXT_STARTER
  string starterFile = m_bodyTextInputFilePath + BODY_TEXT_STARTER;

  ifstream inStarterFile(starterFile);
  if (!inStarterFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << starterFile << endl;
    return;
  }

  while (!inStarterFile.eof()) {
    string line{""};
    getline(inStarterFile, line);
    // excluding start line
    outfile << line << endl;
  }
}

void TableContainer::finishBodyTextFile() {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
  // copy content from BODY_TEXT_DESSERT
  string dessertFile = m_bodyTextInputFilePath + BODY_TEXT_DESSERT;

  ifstream inDessertFile(dessertFile);
  if (!inDessertFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << dessertFile << endl;
    return;
  }

  while (!inDessertFile.eof()) {
    string line{""};
    getline(inDessertFile, line);
    // excluding start line
    outfile << line << endl;
  }
}

void TableContainer::insertFrontParagrapHeader(int totalPara,
                                               const string &units) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile);
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
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
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
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
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
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
  outfile << R"(<tr><td width="50%">)" << text << "</td>" << endl;
}

void TableContainer::appendRightParagraphInBodyText(const string &text) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, ios_base::app);
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

void LinkSetContainer::assembleBackToHTM(const string &title,
                                         const string &displayTitle) {

  string inputHtmlFile = getInputHtmlFilePath();
  string inputBodyTextFile = getOutputBodyTextFilePath();
  string outputFile = getoutputHtmlFilepath();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << inputBodyTextFile << endl;
    return;
  }
  ofstream outfile(outputFile);
  string line{""};
  bool started = false;
  // first line
  string start = topIdBeginChars;
  // last line
  string end = bottomIdBeginChars;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    if (not started) {
      auto linkBegin = line.find(start);
      if (linkBegin != string::npos) {
        started = true;
        break;
      }
      if (not title.empty()) {
        auto titleBegin = line.find(defaultTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultTitle.length(), title);
      }
      if (not displayTitle.empty()) {
        auto titleBegin = line.find(defaultDisplayTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultDisplayTitle.length(), displayTitle);
      }
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << line << endl;
      // excluding start line
      outfile << line << endl;
    }
  }
  if (inHtmlFile.eof() and not started) {
    METHOD_OUTPUT << "source htm" << inputBodyTextFile
                  << "has no start mark:" << start << endl;
    return;
  }
  bool ended = false;
  while (!inBodyTextFile.eof()) {
    getline(inBodyTextFile, line);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << line << endl;
    // including end line
    outfile << line << endl;
  }
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    if (not ended) {
      auto linkEnd = line.find(end);
      if (linkEnd != string::npos) {
        ended = true;
        continue;
      }
    } else {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << line << endl;
      // including end line
      outfile << line << endl;
    }
  }
}
