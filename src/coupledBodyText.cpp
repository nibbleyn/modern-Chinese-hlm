#include "coupledBodyText.hpp"
#include <functional> // std::greater

bool isFoundAsNonKeys(const string &line, const string &key) {
  unsigned int searchStart = 0;
  while (true) {
    auto keyBegin = line.find(key, searchStart);
    // no key found any more
    if (keyBegin == string::npos)
      break;
    auto testBeginPos = line.rfind(keyStartChars, keyBegin);
    auto endOfTag = 0;
    // if keyStartChars exists, deprecated in the future
    // when keyStartChars is gone
    if (testBeginPos != string::npos and testBeginPos > searchStart) {
      string testStr =
          line.substr(testBeginPos + keyStartChars.length(),
                      keyBegin - testBeginPos - keyStartChars.length());
      if (testStr.find(keyEndChars) != string::npos)
        return true;
      endOfTag = line.find(keyEndChars, keyBegin);
    } else {
      testBeginPos = line.rfind(titleStartChars, keyBegin);
      // if titleStartChars exists
      if (testBeginPos != string::npos and testBeginPos > searchStart) {
        string testStr =
            line.substr(testBeginPos + titleStartChars.length(),
                        keyBegin - testBeginPos - titleStartChars.length());
        if (testStr.find(titleEndChars) != string::npos)
          return true;
        endOfTag = line.find(titleEndChars, keyBegin);
      } else
        return true;
    }
    searchStart = endOfTag + 1;
  }
  return false;
}

bool isFoundOutsidePersonalComments(const string &line, const string &key) {
  unsigned int searchStart = 0;
  while (true) {
    auto keyBegin = line.find(key, searchStart);
    // no key found any more
    if (keyBegin == string::npos)
      break;
    auto testBeginPos = line.rfind(personalCommentStartChars, keyBegin);
    // if personalCommentStartChars exists
    if (testBeginPos != string::npos and testBeginPos > searchStart) {
      string testStr = line.substr(
          testBeginPos + personalCommentStartChars.length(),
          keyBegin - testBeginPos - personalCommentStartChars.length());
      FUNCTION_OUTPUT << testStr << endl;
      if (testStr.find(personalCommentEndChars) != string::npos)
        return true;
    } else
      return true;
    searchStart = line.find(personalCommentEndChars, keyBegin) + 1;
  }
  return false;
}

void CoupledBodyText::loadBodyTextsFromFixBackToOutput() {
  vector<string> filenameList;
  Poco::File(BODY_TEXT_FIX).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToCopy(BODY_TEXT_FIX + file);
    fileToCopy.copyTo(BODY_TEXT_OUTPUT + file);
  }
}

void CoupledBodyText::setInputOutputFiles() {
  string attachmentPart{""};
  if (m_attachNumber != 0)
    attachmentPart = attachmentFileMiddleChar + TurnToString(m_attachNumber);

  m_inputFile = BODY_TEXT_OUTPUT + m_filePrefix + m_file + attachmentPart +
                BODY_TEXT_SUFFIX;
  m_outputFile =
      BODY_TEXT_FIX + m_filePrefix + m_file + attachmentPart + BODY_TEXT_SUFFIX;

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "input file is: " << m_inputFile << endl;
    METHOD_OUTPUT << "output file is: " << m_outputFile << endl;
  }
}

/**
 * find a keyword in a numbered bodytext file
 * whose name is specified as fullPath
 * @param key the keyword used to search
 * @param fullPath the fullpath of the bodytext file
 * @param lineNumber the return value of the line's number like P8L2
 * @param needChange return value indicating a valid line is found
 * and thus need to change original link
 * @return changed key (to KeyNotFound + key + reason) if it is not found,
 * otherwise the original key
 */
bool CoupledBodyText::findKey(const string &key) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    m_searchError = "file doesn't exist:" + m_inputFile;
    METHOD_OUTPUT << m_searchError << endl;
    return false;
  }
  bool found = false;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    string line{""};
    getline(infile, line); // Saves the line

    if (line.find(key) == string::npos) // not appear in this line
    {
      continue;
    }

    // skip text in image group
    if (isImageGroupLine(line)) {
      continue;
    }

    // if "keys" found are only part of "key" of another link, skip this line
    if (not isFoundAsNonKeys(line, key)) {
      continue;
    }

    // if "keys" found are only part of personal comments, skip them
    if (not isFoundOutsidePersonalComments(line, key)) {
      continue;
    }

    LineNumber ln;
    ln.loadFirstFromContainedLine(line);
    if (not ln.valid()) {
      m_searchError =
          "file doesn't get numbered:" + m_inputFile + " at line:" + line;
      METHOD_OUTPUT << m_searchError << endl;
      return false;
    }

    // special hack for ignoring one lineNumber
    if (not m_ignoreSet.empty() and
        m_ignoreSet.find(ln.asString()) != m_ignoreSet.end()) {
      continue;
    }

    found = true;
    m_result.insert(ln.asString());
    if (m_onlyFirst) {
      break;
    }
  }
  return found;
}

void CoupledBodyText::removeNbspsAndSpaces() {
  // remove existing nbsps and spaces
  auto iterBegin = m_inLine.begin();
  while (iterBegin < m_inLine.end()) {
    if (*iterBegin == displaySpace.at(0))
      ++iterBegin;
    else if (*iterBegin == space.at(0) and
             (iterBegin + space.length() < m_inLine.end()) and
             (string(iterBegin, iterBegin + space.length()) == space)) {
      iterBegin += space.length();
    } else
      break;
  }
  m_inLine = string(iterBegin, m_inLine.end());
}

void CoupledBodyText::removeOldLineNumber() {
  auto linkEnd = m_inLine.find(LineNumberEnd);
  m_inLine = m_inLine.substr(linkEnd + LineNumberEnd.length());
}

void CoupledBodyText::numberingLine(ofstream &outfile, bool forceUpdate,
                                    bool hideParaHeader) {
  LineNumber ln;
  ln.loadFirstFromContainedLine(m_inLine);
  LineNumber newLn(m_para, m_lineNo);
  if (forceUpdate or not ln.equal(newLn)) {
    if (ln.valid()) // remove old line number
    {
      removeOldLineNumber();
    }
    removeNbspsAndSpaces();
    outfile << newLn.getWholeString() << doubleSpace << displaySpace << m_inLine
            << endl; // Prints our line
  } else
    outfile << m_inLine << endl;
  m_lineNo++;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "processed :" << endl;
    METHOD_OUTPUT << m_inLine << endl;
  }
}

void CoupledBodyText::addFirstParaHeader(ofstream &outfile) {
  m_paraHeader.loadFrom(m_inLine);
  m_paraHeader.fixFromTemplate();
  outfile << m_paraHeader.getFixedResult() << endl;
  m_para = 1;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "processed :" << endl;
    METHOD_OUTPUT << m_inLine << endl;
  }
}

void CoupledBodyText::addlastParaHeader(ofstream &outfile) {
  m_paraHeader.markAsLastParaHeader();
  m_paraHeader.fixFromTemplate();
  outfile << m_paraHeader.getFixedResult() << endl;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "processed :" << endl;
    METHOD_OUTPUT << m_inLine << endl;
  }
}

void CoupledBodyText::addMiddleParaHeader(ofstream &outfile,
                                          bool enterLastPara) {
  m_paraHeader.m_currentParaNo = m_para++;
  m_paraHeader.m_lastPara = enterLastPara;
  m_paraHeader.markAsMiddleParaHeader();
  m_paraHeader.fixFromTemplate();
  outfile << m_paraHeader.getFixedResult() << endl;
  m_lineNo = 1; // LINE index within each group
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "para header added :" << endl;
    METHOD_OUTPUT << m_paraHeader.getFixedResult() << endl;
  }
}

void CoupledBodyText::addParaHeader(ofstream &outfile) {
  // first para Header
  if (m_para == 0) {
    addFirstParaHeader(outfile);
  } else if (m_para == m_numberOfMiddleParaHeader + 1) {
    addlastParaHeader(outfile);
  } else {
    bool enterLastPara = (m_para == m_numberOfMiddleParaHeader);
    addMiddleParaHeader(outfile, enterLastPara);
  }
}

void CoupledBodyText::validateFormatForNumbering() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  // continue reading till first paragraph header
  string inLine;
  bool stop = false;
  while (!infile.eof() and not stop) {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.asString() << endl;
      }
      stop = true;
    }
  }
  if (stop == false) {
    METHOD_OUTPUT << "no top or bottom paragraph found:" << m_inputFile << endl;
    return;
  }

  bool expectAnotherHalf = false;
  bool processedLastParaHeader = false;
  ParaHeader paraHeaderLoaded;
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.asString() << endl;
      }
      paraHeaderLoaded.readType(inLine);
      processedLastParaHeader = paraHeaderLoaded.isLastParaHeader();
      expectAnotherHalf = false;
      continue;
    }

    if (isEmptyLine(inLine)) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "processed empty line." << endl;
        METHOD_OUTPUT << inLine << endl;
      }
      continue;
    }

    if (isImageGroupLine(inLine)) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "processed image Group line." << endl;
        METHOD_OUTPUT << inLine << endl;
      }
      continue;
    }

    if (expectAnotherHalf) {
      if (isLeadingBr(inLine)) {
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << "processed Leading Br:" << endl;
          METHOD_OUTPUT << inLine << endl;
        }
        // still expectAnotherHalf
        continue;
      }
      if (hasEndingBr(inLine)) {
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << "processed normal Line:" << endl;
          METHOD_OUTPUT << inLine << endl;
        }
        expectAnotherHalf = false;
        continue;
      }
    } else {
      // must followed by a leading BR
      if (isLeadingBr(inLine)) {
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << "processed Leading Br." << endl;
          METHOD_OUTPUT << inLine << endl;
        }
        expectAnotherHalf = true;
      } else {
        METHOD_OUTPUT << "expectAnotherHalf: " << expectAnotherHalf
                      << " wrong without leading " << brTab << endl;
        METHOD_OUTPUT << inLine << endl;
        exit(1);
        break;
      }
    }
  }
  if (processedLastParaHeader == false) {
    METHOD_OUTPUT << "no bottom paragraph found:" << m_inputFile << endl;
    return;
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "validating finished." << endl;
}
