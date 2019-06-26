#include "container.hpp"

static const string defaultTitle = R"(XXX)";
static const string defaultDisplayTitle = R"(YYY)";

void Container::dissembleFromHTM() {
  string inputHtmlFile = getInputHtmlFilePath();
  string outputBodyTextFile = getBodyTextFilePath();

  ifstream infile(inputHtmlFile);
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << inputHtmlFile << endl;
    return;
  }
  ofstream outfile(outputBodyTextFile);
  string line{emptyString};
  bool headerSkipped = false, bodyFinished = false;
  while (!infile.eof()) {
    getline(infile, line);
    if (not headerSkipped) {
      if (line.find(topIdBeginChars) != string::npos) {
        headerSkipped = true;
        // also including the topIdBeginChars contained line
        outfile << line << endl;
      }
    } else {
      // headerSkipped
      if (not bodyFinished) {
        if (line.find(bottomIdBeginChars) != string::npos) {
          bodyFinished = true;
          // including the bottomIdBeginChars contained line
          outfile << line << endl;
          break;
        } else
          // including all lines in between
          outfile << line << endl;
      }
    }
  }
  if (not headerSkipped)
    METHOD_OUTPUT << "no top paragraph found for " << inputHtmlFile << "as "
                  << topIdBeginChars << endl;
  else if (not bodyFinished)
    METHOD_OUTPUT << "no end paragraph found for " << inputHtmlFile << "as "
                  << bottomIdBeginChars << endl;
  else if (debug >= LOG_INFO)
    METHOD_OUTPUT << "dissemble finished for " << inputHtmlFile << endl;
}

void Container::assembleBackToHTM() {

  string inputHtmlFile = getInputHtmlFilePath();
  string inputBodyTextFile = getBodyTextFilePath();
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
  string line{emptyString};
  bool started = false;
  // merge header of input html
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    // excluding output topIdBeginChars contained line
    if (line.find(topIdBeginChars) != string::npos) {
      started = true;
      break;
    }
    if (not m_title.empty()) {
      auto titleBegin = line.find(defaultTitle);
      if (titleBegin != string::npos)
        line.replace(titleBegin, defaultTitle.length(), m_title);
    }
    if (not m_displayTitle.empty()) {
      auto titleBegin = line.find(defaultDisplayTitle);
      if (titleBegin != string::npos)
        line.replace(titleBegin, defaultDisplayTitle.length(), m_displayTitle);
    }
    outfile << line << endl;
  }
  if (inHtmlFile.eof() and not started) {
    METHOD_OUTPUT << "source .htm: " << inputHtmlFile
                  << " has no start mark:" << topIdBeginChars << endl;
    return;
  }

  // merge lines from bodyText
  auto ended = false;
  while (!inBodyTextFile.eof()) {
    getline(inBodyTextFile, line);
    // including topIdBeginChars and bottomIdBeginChars contained lines
    outfile << line << endl;
    if (m_bodyTextWithEndMark and
        line.find(bottomIdBeginChars) != string::npos) {
      ended = true;
      break;
    }
  }
  if (m_bodyTextWithEndMark and inBodyTextFile.eof() and ended == false) {
    METHOD_OUTPUT << "source body text file: " << inputBodyTextFile
                  << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }

  // merge footer of html
  auto bodySkipped = false;
  ended = false;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    if (bodySkipped) {
      // excluding output bottomIdBeginChars contained line
      outfile << line << endl;
      // already included this final line
      if (m_htmlWithEndMark and line.find(htmlFileFinalLine) != string::npos) {
        ended = true;
        break;
      }
    } else {
      // excluding all lines in-between and also bottomIdBeginChars contained
      // line
      if (line.find(bottomIdBeginChars) != string::npos) {
        bodySkipped = true;
      }
    }
  }
  if (m_htmlWithEndMark and inHtmlFile.eof() and ended == false) {
    METHOD_OUTPUT << "source .htm: " << inputHtmlFile
                  << " has no end mark:" << htmlFileFinalLine << endl;
    return;
  }

  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "assemble finished for " << outputFile << endl;
}
