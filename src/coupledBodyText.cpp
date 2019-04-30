#include "coupledBodyText.hpp"
#include <functional> // std::greater

/**
 * load files under BODY_TEXT_OUTPUT directory with files under BODY_TEXT_FIX
 * i.e. from afterFix to output
 * for continue link fixing after numbering..
 * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for
 * it
 */
void CoupledBodyText::loadBodyTextsFromFixBackToOutput() {
  vector<string> filenameList;
  Poco::File(BODY_TEXT_FIX).list(filenameList);
  sort(filenameList.begin(), filenameList.end(), less<string>());
  for (const auto &file : filenameList) {
    Poco::File fileToCopy(BODY_TEXT_FIX + file);
    fileToCopy.copyTo(BODY_TEXT_OUTPUT + file);
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
    getline(infile, inLine);
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
  CoupledParaHeader paraHeaderLoaded;
  while (!infile.eof()) {
    getline(infile, inLine);
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

void CoupledBodyText::numberingLine(ofstream &outfile, bool forceUpdate) {
  LineNumber ln;
  ln.loadFirstFromContainedLine(m_inLine);
  LineNumber newLn(m_para, m_lineNo);
  // remove old line number if forced or invalid
  if (forceUpdate or not ln.equal(newLn)) {
    if (ln.valid()) {
      removeOldLineNumber();
    }
    removeNbspsAndSpaces();
    outfile << newLn.getWholeString() << doubleSpace << displaySpace << m_inLine
            << endl;
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
  if (not m_hideParaHeader)
    outfile << m_paraHeader.getFixedResult() << endl;
  m_para = 1;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "processed :" << endl;
    METHOD_OUTPUT << m_inLine << endl;
  }
}

void CoupledBodyText::addlastParaHeader(ofstream &outfile) {
  m_paraHeader.setCurrentParaNo(m_para++);
  m_paraHeader.markAsLastParaHeader();
  m_paraHeader.fixFromTemplate();
  if (not m_hideParaHeader)
    outfile << m_paraHeader.getFixedResult() << endl;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "processed :" << endl;
    METHOD_OUTPUT << m_inLine << endl;
  }
}

void CoupledBodyText::addMiddleParaHeader(ofstream &outfile,
                                          bool enterLastPara) {
  m_paraHeader.setCurrentParaNo(m_para++);
  m_paraHeader.markAsMiddleParaHeader();
  m_paraHeader.markAsLastMiddleParaHeader(enterLastPara);
  m_paraHeader.fixFromTemplate();
  if (not m_hideParaHeader)
    outfile << m_paraHeader.getFixedResult() << endl;
  m_lineNo = 1;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "para header added :" << endl;
    METHOD_OUTPUT << m_paraHeader.getFixedResult() << endl;
  }
}

void CoupledBodyText::addParaHeader(ofstream &outfile) {
  // first para Header
  if (m_para < m_numberOfFirstParaHeader) {
    addFirstParaHeader(outfile);
  } else if (m_para == m_numberOfMiddleParaHeader + m_numberOfFirstParaHeader) {
    addlastParaHeader(outfile);
  } else {
    bool enterLastPara =
        (m_para == m_numberOfMiddleParaHeader + m_numberOfFirstParaHeader - 1);
    addMiddleParaHeader(outfile, enterLastPara);
  }
}

void CoupledBodyText::scanByLines() {

  m_numberOfFirstParaHeader = 0;
  m_numberOfMiddleParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  m_lineAttrTable.clear();
  m_imgGroupFollowingParaTable.clear();

  ifstream infile(m_inputFile);

  bool processImgGroup = false;
  size_t lastImgGroupBegin = 0;
  size_t lastImgGroupEnd = 0;

  size_t seqOfLines = 0;
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << m_inLine << endl;
    }
    LineNumber ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isParagraphHeader()) {
      m_paraHeader.loadFrom(m_inLine);
      LineInfo info{0, DISPLY_LINE_TYPE::PARA, ln.asString()};
      m_lineAttrTable[seqOfLines] = info;
      if (m_paraHeader.isFirstParaHeader()) {
        m_numberOfFirstParaHeader++;
      } else if (m_paraHeader.isLastParaHeader()) {
        m_numberOfLastParaHeader++;
        m_lastSeqNumberOfLine = seqOfLines;
        break;
      } else if (m_paraHeader.isMiddleParaHeader())
        m_numberOfMiddleParaHeader++;
      if (processImgGroup)
        processImgGroup = false;
    } else if (isLeadingBr(m_inLine)) {
      LineInfo info{1, DISPLY_LINE_TYPE::EMPTY, "<BR>"};
      m_lineAttrTable[seqOfLines] = info;
      if (processImgGroup)
        lastImgGroupEnd = seqOfLines;
    } else if (isImageGroupLine(m_inLine)) {
      // last image Group just goes before this one
      if (processImgGroup) {
        m_imgGroupFollowingParaTable[lastImgGroupBegin] = lastImgGroupEnd;
      }
      LineInfo info{0, DISPLY_LINE_TYPE::IMAGE, "image"};
      m_lineAttrTable[seqOfLines] = info;
      processImgGroup = true;
      lastImgGroupBegin = seqOfLines;
      lastImgGroupEnd = seqOfLines;
    } else if (not isEmptyLine(m_inLine)) {
      if (processImgGroup) {
        // at least one BR in between, which should be true if validated
        if (lastImgGroupEnd != lastImgGroupBegin and
            isInLineAttrTable(lastImgGroupEnd) and
            m_lineAttrTable[lastImgGroupEnd].type == DISPLY_LINE_TYPE::EMPTY)
          lastImgGroupEnd = findEarlierLineInLineAttrTable(lastImgGroupEnd);
        m_imgGroupFollowingParaTable[lastImgGroupBegin] = lastImgGroupEnd;
        processImgGroup = false;
      }
      auto lastBr = m_inLine.find(brTab);
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << m_inLine.substr(0, lastBr) << endl;
      }
      size_t end = -1;
      LineInfo info{0, DISPLY_LINE_TYPE::TEXT, utf8substr(m_inLine, 0, end, 5)};
      m_lineAttrTable[seqOfLines] = info;
    }
    seqOfLines++;
  }

  m_numberOfMiddleParaHeader += m_imgGroupFollowingParaTable.size();

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << endl;
    METHOD_OUTPUT << "Result of scanByLines:" << endl;
    METHOD_OUTPUT << "m_numberOfFirstParaHeader: " << m_numberOfFirstParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfMiddleParaHeader: "
                  << m_numberOfMiddleParaHeader << endl;
    METHOD_OUTPUT << "m_numberOfLastParaHeader: " << m_numberOfLastParaHeader
                  << endl;
    METHOD_OUTPUT << "m_lastSeqNumberOfLine: " << m_lastSeqNumberOfLine << endl;
    printLineAttrTable();
    printImgGroupFollowingParaTable();
  }
}

void CoupledBodyText::paraGuidedNumbering(bool forceUpdate) {

  ifstream infile(m_inputFile);
  ofstream outfile(m_outputFile);

  m_para = 0;
  m_lineNo = 1;
  size_t seqOfLines = 0;
  bool needAddParaAfterImgGroup = false;
  size_t lineBeforeParaHeader = 0;
  while (!infile.eof()) {
    // Saves the line in m_inLine.
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << seqOfLines << ": " << m_inLine << endl;
    }
    auto inLineTable = isInLineAttrTable(seqOfLines);
    // output directly if not in m_lineAttrTable or BRs
    if (inLineTable == false or
        m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::EMPTY) {
      outfile << m_inLine << endl;
    } else if (m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::IMAGE) {
      // no numbering of image group
      outfile << m_inLine << endl;
      if (isInImgGroupFollowingParaTable(seqOfLines)) {
        needAddParaAfterImgGroup = true;
        lineBeforeParaHeader = m_imgGroupFollowingParaTable[seqOfLines];
      }
    } else if (m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::TEXT) {
      numberingLine(outfile, forceUpdate);
    } else if (m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::PARA) {
      addParaHeader(outfile);
    }
    if (needAddParaAfterImgGroup and seqOfLines == lineBeforeParaHeader) {
      addParaHeader(outfile);
    }
    seqOfLines++;
  }
}

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

bool CoupledBodyText::findKey(const string &key) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    m_searchError = "file doesn't exist:" + m_inputFile;
    METHOD_OUTPUT << m_searchError << endl;
    return false;
  }
  bool found = false;
  while (!infile.eof()) {
    string line{""};
    getline(infile, line);

    // if not appear in this line
    if (line.find(key) == string::npos) {
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
    if (m_ignorePersonalComments and
        not isFoundOutsidePersonalComments(line, key)) {
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

void CoupledBodyText::fetchLineTexts() {
  setInputOutputFiles();
  m_resultLines.clear();
  ifstream infile(m_inputFile);
  if (infile) {
    LineNumber begin = m_range.first;
    LineNumber end = m_range.second;
    if (end.equal(END_OF_WHOLE_BODYTEXT) or not(begin > end)) {
      while (!infile.eof()) {
        getline(infile, m_inLine);
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << m_inLine << endl;
        }
        LineNumber ln;
        ln.loadFirstFromContainedLine(m_inLine);
        if (ln.isParagraphHeader() or not ln.valid()) {
          continue;
        }
        bool finished = end.equal(END_OF_WHOLE_BODYTEXT) ? false : (ln > end);
        if (begin > ln)
          continue;
        else if (finished)
          break;
        m_resultLines[ln.asString()] = m_inLine;
      }
    }
  }
  printResultLines();
}

void CoupledBodyText::setOutputBodyTextFilePath(const string &absolutePath) {
  m_outputFile = absolutePath;

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "output file is: " << m_outputFile << endl;
  }
}

void CoupledBodyText::appendLinesIntoBodyTextFile() {
  ofstream outfile;
  outfile.open(m_outputFile, std::ios_base::app);
  for (const auto &line : m_resultLines) {
    outfile << brTab << endl;
    outfile << line.second << endl;
  }
}
