#include "genericContainer.hpp"

extern int debug;
/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void ListContainer::clearExistingBodyText() {
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
  outfile.open(outputBodyTextFile, std::ios_base::app);
  outfile << "<br>" << text << "</br>" << endl;
}

void ListContainer::appendParagrapHeader(const string &header) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, std::ios_base::app);
  outfile << header << endl;
}

const string TableContainer::BODY_TEXT_STARTER = R"(3front.txt)";
const string TableContainer::BODY_TEXT_DESSERT = R"(3back.txt)";

void TableContainer::addExistingFrontParagraphs() {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "init content in: " << outputBodyTextFile << endl;
  ofstream outfile(outputBodyTextFile, std::ios_base::app);
  // copy content from BODY_TEXT_STARTER
  string starterFile = m_bodyTextInputFilePath + BODY_TEXT_STARTER;

  ifstream inStarterFile(starterFile);
  if (!inStarterFile) // doesn't exist
  {
    METHOD_OUTPUT << "Starter file doesn't exist:" << starterFile << endl;
    return;
  }

  while (!inStarterFile.eof()) // To get you all the lines.
  {
    string line{""};
    getline(inStarterFile, line); // Saves the line in line.
    outfile << line << endl;      // excluding start line
  }
}

void TableContainer::finishBodyTextFile() {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, std::ios_base::app);
  // copy content from BODY_TEXT_DESSERT
  string dessertFile = m_bodyTextInputFilePath + BODY_TEXT_DESSERT;

  ifstream inDessertFile(dessertFile);
  if (!inDessertFile) // doesn't exist
  {
    METHOD_OUTPUT << "Dessert file doesn't exist:" << dessertFile << endl;
    return;
  }

  while (!inDessertFile.eof()) // To get you all the lines.
  {
    string line{""};
    getline(inDessertFile, line); // Saves the line in line.
    outfile << line << endl;      // excluding start line
  }
}

void TableContainer::insertFrontParagrapHeader(int totalPara,
                                               const string &units) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append content in: " << outputBodyTextFile << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile);
  LineNumber::setStartNumber(START_PARA_NUMBER);
  string line = fixFrontParaHeaderFromTemplate(LineNumber::getStartNumber(), "",
                                               totalPara, units, false);
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
  outfile.open(outputBodyTextFile, std::ios_base::app);
  string line;
  if (enterLastPara) {
    line = insertParaHeaderFromTemplate(LineNumber::getStartNumber(), seqOfPara,
                                        startParaNo, endParaNo, totalPara,
                                        preTotalPara, "", units, false, true);
  } else
    line = insertParaHeaderFromTemplate(LineNumber::getStartNumber(), seqOfPara,
                                        startParaNo, endParaNo, totalPara,
                                        preTotalPara, "", units, false, false);
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
  outfile.open(outputBodyTextFile, std::ios_base::app);
  string line = fixBackParaHeaderFromTemplate(
      LineNumber::getStartNumber(), seqOfPara, totalPara, "", units, false);
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
  outfile.open(outputBodyTextFile, std::ios_base::app);
  outfile << R"(<tr><td width="50%">)" << text << "</td>" << endl;
}

void TableContainer::appendRightParagraphInBodyText(const string &text) {
  string outputBodyTextFile = getOutputBodyTextFilePath();
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "append Paragraph In BodyText: " << outputBodyTextFile
                  << endl;
  ofstream outfile;
  outfile.open(outputBodyTextFile, std::ios_base::app);
  outfile << R"(<td width="50%">)" << text << R"(</td></tr>)" << endl;
}

void GenericContainer::assembleBackToHTM(const string &title,
                                         const string &displayTitle) {

  string inputHtmlFile = getInputHtmlFilePath();
  string inputBodyTextFile = getOutputBodyTextFilePath();
  string outputFile = getoutputHtmlFilepath();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    METHOD_OUTPUT << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) // doesn't exist
  {
    METHOD_OUTPUT << "file doesn't exist:" << inputBodyTextFile << endl;
    return;
  }
  ofstream outfile(outputFile);
  string line{""};
  bool started = false;
  string start = topIdBeginChars;  // first line
  string end = bottomIdBeginChars; // last line
  while (!inHtmlFile.eof())        // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
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
        METHOD_OUTPUT << line << endl; // including end line
      outfile << line << endl;         // excluding start line
    }
  }
  if (inHtmlFile.eof() and not started) {
    METHOD_OUTPUT << "source htm" << inputBodyTextFile
                  << "has no start mark:" << start << endl;
    return;
  }
  bool ended = false;
  while (!inBodyTextFile.eof()) // To get you all the lines.
  {
    getline(inBodyTextFile, line); // Saves the line in line.
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << line << endl; // including end line
    outfile << line << endl;         // including end line
  }
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    if (not ended) {
      auto linkEnd = line.find(end);
      if (linkEnd != string::npos) {
        ended = true;
        continue;
      }
    } else {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << line << endl; // including end line
      outfile << line << endl;         // excluding end line
    }
  }
}
