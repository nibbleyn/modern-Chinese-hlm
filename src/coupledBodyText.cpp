#include "coupledBodyText.hpp"
#include <functional> // greater

void CoupledBodyText::validateFormatForNumbering() {
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  // continue reading till first paragraph header
  string inLine;
  bool stop = false;
  while (!infile.eof() and not stop) {
    getline(infile, inLine);
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isPartOfParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.getParaLineString() << endl;
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
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isPartOfParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.getParaLineString() << endl;
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
  string attachmentPart{emptyString};
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

void CoupledBodyText::numberingLine(ofstream &outfile) {
  LineNumberPlaceholderLink ln;
  ln.loadFirstFromContainedLine(m_inLine);
  LineNumber newLn(m_para, m_lineNo);
  // remove old line number if forced or invalid
  if (m_forceUpdateLineNumber or not ln.get().equal(newLn)) {
    if (ln.get().valid()) {
      removeOldLineNumber();
    }
    removeNbspsAndSpaces();
    LineNumberPlaceholderLink newLink(newLn);
    outfile << newLink.getWholeString() << doubleSpace << displaySpace
            << m_inLine << endl;
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
  if (not fileExist(infile, m_inputFile))
    return;
  bool processImgGroup = false;
  size_t lastImgGroupBegin = 0;
  size_t lastImgGroupEnd = 0;

  size_t seqOfLines = 0;
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << m_inLine << endl;
    }
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(m_inLine);
    if (ln.isPartOfParagraphHeader()) {
      m_paraHeader.loadFrom(m_inLine);
      LineInfo info{0, DISPLY_LINE_TYPE::PARA, ln.getParaLineString()};
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

void CoupledBodyText::paraGuidedNumbering() {
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
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
      numberingLine(outfile);
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
    auto testBeginPos = line.rfind(titleStartChars, keyBegin);
    auto endOfTag = testBeginPos;
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
      if (debug >= LOG_INFO)
        FUNCTION_OUTPUT << testStr << endl;
      if (testStr.find(personalCommentEndChars) != string::npos)
        return true;
    } else
      return true;
    searchStart = line.find(personalCommentEndChars, keyBegin) + 1;
  }
  return false;
}

/**
 * find a keyword in a numbered bodytext file
 */
bool CoupledBodyText::findKey(const string &key) {
  ifstream infile(m_inputFile);
  if (!infile) {
    m_searchError = ERROR_FILE_NOT_EXIST + m_inputFile;
    METHOD_OUTPUT << m_searchError << endl;
    return false;
  }
  bool found = false;
  while (!infile.eof()) {
    string line{emptyString};
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

    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(line);
    if (not ln.get().valid()) {
      m_searchError =
          "file doesn't get numbered:" + m_inputFile + " at line:" + line;
      METHOD_OUTPUT << m_searchError << endl;
      return false;
    }

    // special hack for ignoring one lineNumber
    if (not m_ignoreSet.empty() and
        m_ignoreSet.find(ln.getParaLineString()) != m_ignoreSet.end()) {
      continue;
    }

    found = true;
    m_result.insert(ln.getParaLineString());
    if (m_onlyFirst) {
      break;
    }
  }
  return found;
}

void CoupledBodyText::fetchLineTexts() {
  m_resultLines.clear();
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  if (infile) {
    LineNumber begin(m_range.first.first, m_range.first.second);
    LineNumber end(m_range.second.first, m_range.second.second);
    if (end.equal(END_OF_WHOLE_BODYTEXT) or not(begin > end)) {
      while (!infile.eof()) {
        getline(infile, m_inLine);
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << m_inLine << endl;
        }
        LineNumberPlaceholderLink ln;
        ln.loadFirstFromContainedLine(m_inLine);
        if (ln.isPartOfParagraphHeader() or not ln.get().valid()) {
          continue;
        }
        bool finished =
            end.equal(END_OF_WHOLE_BODYTEXT) ? false : (ln.get() > end);
        if (begin > ln.get())
          continue;
        else if (finished)
          break;
        m_resultLines[ln.get().getParaLineNumber()] = m_inLine;
      }
    }
  }
  printResultLines();
}

void CoupledBodyText::appendLinesIntoBodyTextFile() {
  ofstream outfile;
  outfile.open(m_outputFile, ios_base::app);
  for (const auto &line : m_resultLines) {
    outfile << brTab << endl;
    outfile << line.second << endl;
  }
}
