#include "container.hpp"

static const string defaultTitle = R"(XXX)";
static const string defaultDisplayTitle = R"(YYY)";

void Container::dissembleFromHTM() {
  ifstream infile(getInputHtmlFilePath());
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << getInputHtmlFilePath() << endl;
    return;
  }
  ofstream outfile(getBodyTextFilePath());
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
    METHOD_OUTPUT << "no top paragraph found for " << getInputHtmlFilePath()
                  << "as " << topIdBeginChars << endl;
  else if (not bodyFinished)
    METHOD_OUTPUT << "no end paragraph found for " << getInputHtmlFilePath()
                  << "as " << bottomIdBeginChars << endl;
  else if (debug >= LOG_INFO)
    METHOD_OUTPUT << "dissemble finished for " << getInputHtmlFilePath()
                  << endl;
}

void Container::assembleBackToHTM() {
  ifstream inHtmlFile(getInputHtmlFilePath());
  if (!inHtmlFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << getInputHtmlFilePath() << endl;
    return;
  }
  ifstream inBodyTextFile(getBodyTextFilePath());
  if (!inBodyTextFile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << getBodyTextFilePath() << endl;
    return;
  }

  ofstream outfile(getoutputHtmlFilepath());
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
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << line << endl;
    outfile << line << endl;
  }
  if (inHtmlFile.eof() and not started) {
    METHOD_OUTPUT << "source .htm: " << getInputHtmlFilePath()
                  << " has no start mark:" << topIdBeginChars << endl;
    return;
  }

  // merge lines from bodyText
  auto ended = false;
  while (!inBodyTextFile.eof()) {
    getline(inBodyTextFile, line);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << line << endl;
    // including topIdBeginChars and bottomIdBeginChars contained lines
    outfile << line << endl;
    if (m_bodyTextWithEndMark and
        line.find(bottomIdBeginChars) != string::npos) {
      ended = true;
      break;
    }
  }
  if (m_bodyTextWithEndMark and inBodyTextFile.eof() and ended == false) {
    METHOD_OUTPUT << "source body text file: " << getBodyTextFilePath()
                  << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }

  // merge footer of html
  auto bodySkipped = false;
  ended = false;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    if (bodySkipped) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << line << endl;
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
    METHOD_OUTPUT << "source .htm: " << getInputHtmlFilePath()
                  << " has no end mark:" << htmlFileFinalLine << endl;
    return;
  }

  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "assemble finished for " << getoutputHtmlFilepath()
                  << endl;
}

void Container::clearFixedBodyTexts() {
  vector<string> filenameList;
  Poco::File(m_fixedBodyTextDir).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToClear(m_fixedBodyTextDir + file);
    fileToClear.remove();
  }
}

/**
 * load files under BODY_TEXT_OUTPUT directory with files under BODY_TEXT_FIX
 * i.e. from afterFix to output
 * for continue link fixing after numbering..
 * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for
 * it
 */
void Container::loadFixedBodyTexts() {
  vector<string> filenameList;
  Poco::File(m_fixedBodyTextDir).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToCopy(m_fixedBodyTextDir + file);
    fileToCopy.copyTo(m_bodyTextDir + file);
  }
}
