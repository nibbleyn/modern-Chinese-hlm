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

// reformat to smaller paragraphs
void CoupledBodyText::reformatParagraphToSmallerSize(
    const string &sampleBlock) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << utf8length(sampleBlock) << endl;
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << sampleBlock << endl;
  // continue reading
  string inLine;
  string CR{0x0D};
  string LF{0x0A};
  string CRLF{0x0D, 0x0A};
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    size_t end = -1;
    do {
      string line = utf8substr(inLine, end + 1, end, utf8length(sampleBlock));
      if (not line.empty()) {
        auto outputLine = line + CRLF;
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << outputLine << CR << CRLF;
        outfile << outputLine << CR << CRLF;
      }
      if (utf8length(line) < utf8length(sampleBlock) - 1)
        break;
    } while (true);
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "reformatting finished." << endl;
}

/**
 * count number of lines with lineNumberIdBeginChars as paragraph sign in it
 * count number of image groups not included in its own paragraph
 */
void CoupledBodyText::getNumberOfPara() {
  setInputOutputFiles();

  m_numberOfFirstParaHeader = 0;
  m_numberOfMiddleParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  m_numberOfImageGroupNotIncludedInPara = 0;
  m_brTable.clear();

  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  string inLine;
  bool expectPara = false;
  size_t seqOfImageGroup = 0;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (inLine.find(topIdBeginChars) != string::npos) {
      m_numberOfFirstParaHeader++; // end of whole FILE_TYPE::MAIN file
      expectPara = false;
      continue;
    }
    if (inLine.find(bottomIdBeginChars) != string::npos) {
      m_numberOfLastParaHeader++;
      break; // end of whole FILE_TYPE::MAIN file
    }
    // search for new paragraph and also fix the para number
    if (inLine.find(lineNumberIdBeginChars) !=
        string::npos) // one new paragraph is found
    {
      LineNumber ln;
      ln.loadFirstFromContainedLine(inLine);
      if (ln.isParagraphHeader()) {
        m_numberOfMiddleParaHeader++;
        expectPara = false;
        continue;
      }
    } else {
      if (isImageGroupLine(inLine)) {
        seqOfImageGroup++;
        expectPara = true;
        m_brTable[seqOfImageGroup] = make_pair(0, false);
        continue;
      }
    }
    if (expectPara) {
      if (isLeadingBr(inLine))
        m_brTable[seqOfImageGroup].first++;
      else if (hasEndingBr(inLine)) {
        expectPara = false;
        m_numberOfImageGroupNotIncludedInPara++;
        m_brTable[seqOfImageGroup].second = true;
        if (m_brTable[seqOfImageGroup].first > 0)
          m_brTable[seqOfImageGroup].first--;
      }
    }
  }
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << endl;
    METHOD_OUTPUT << "Result of getNumberOfPara:" << endl;
    printBrAfterImageGroupTable();
    METHOD_OUTPUT << "m_numberOfFirstParaHeader: " << m_numberOfFirstParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfMiddleParaHeader: "
                  << m_numberOfMiddleParaHeader << endl;
    METHOD_OUTPUT << "m_numberOfLastParaHeader: " << m_numberOfLastParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfImageGroupNotIncludedInPara: "
                  << m_numberOfImageGroupNotIncludedInPara << endl;
  }
}

void CoupledBodyText::removeNbspsAndSpaces(string &inLine) {
  // remove existing nbsps and spaces
  auto iterBegin = inLine.begin();
  while (iterBegin < inLine.end()) {
    if (*iterBegin == displaySpace.at(0))
      ++iterBegin;
    else if (*iterBegin == space.at(0) and
             (iterBegin + space.length() < inLine.end()) and
             (string(iterBegin, iterBegin + space.length()) == space)) {
      iterBegin += space.length();
    } else
      break;
  }
  inLine = string(iterBegin, inLine.end());
}

void CoupledBodyText::removeOldLineNumber(string &inLine) {
  auto linkEnd = inLine.find(LineNumberEnd);
  inLine = inLine.substr(linkEnd + LineNumberEnd.length());
}
/**
 * add following line name with a name of lineNumber
 * before each line
 * <a unhidden id="P4L2">4.2</a>
 * assume the input file in following format:
 * none-paragraph head texts
 * 0-n paragraph
 * none-paragraph head texts
 * and one paragraph in following format:
 * paragraph head
 * 0-n non-LINE
 * 0-N LINE
 * assume one LINE consists of two lines
 * starting from one line of <br>
 * and continue with another line ending with <br>
 * @param inputFile the file to number
 * @param outputFile the output file after numbering
 * @param separatorColor the color to separate paragraphs
 */
void CoupledBodyText::addLineNumber(const string &separatorColor,
                                    bool forceUpdate, bool hideParaHeader) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  if (not m_autoNumbering) {
    getNumberOfPara(); // first scan

    if (m_numberOfFirstParaHeader == 0 or m_numberOfLastParaHeader == 0) {
      METHOD_OUTPUT << "no top or bottom paragraph found:" << m_inputFile
                    << endl;
      return;
      LineNumber::setStartNumber(START_PARA_NUMBER);
    }

  } else {
    METHOD_OUTPUT << "call CoupledBodyTextWithLink::addLineNumber instead."
                  << endl;
    return;
  }

  // increase this number to hold new paragraphs for image Group
  m_numberOfMiddleParaHeader += m_numberOfImageGroupNotIncludedInPara;
  ofstream outfile(m_outputFile);

  // continue reading till first paragraph header
  string inLine;
  bool stop = false; // need to output line even try to stop
  while (!infile.eof() and
         not stop) // To get all the lines till first paragraph header
  {
    getline(infile, inLine); // Saves the line in inLine.
    string line = inLine;
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.asString() << endl;
      }
      ParaHeader paraHeader;
      paraHeader.m_startNumber = LineNumber::getStartNumber();
      paraHeader.m_color = separatorColor;
      paraHeader.m_hidden = hideParaHeader;
      paraHeader.markAsFirstParaHeader();
      paraHeader.fixFromTemplate();
      line = paraHeader.getFixedResult();
      stop = true;
    }
    // incl. above header
    outfile << line << endl;
  }

  bool expectAnotherHalf = false;
  int para = 1;   // paragraph index
  int lineNo = 1; // LINE index within each group
  bool enterLastPara = (m_numberOfMiddleParaHeader == 0);
  bool readLinesAfterImageGroup = false;
  size_t seqOfImageGroup = 0;
  int linesAfter = 0;
  bool needToGenerateParaHeader = false;
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (enterLastPara == true) {
      if (ln.isParagraphHeader()) {
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << ln.asString() << endl;
        }
        ParaHeader paraHeader;
        paraHeader.m_startNumber = LineNumber::getStartNumber();
        paraHeader.m_currentParaNo = m_numberOfMiddleParaHeader + 1;
        paraHeader.m_color = separatorColor;
        paraHeader.m_hidden = hideParaHeader;
        paraHeader.markAsLastParaHeader();
        paraHeader.fixFromTemplate();
        outfile << paraHeader.getFixedResult() << endl;
        break; // end of whole file
      }
    }

    if (ln.isParagraphHeader()) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "paragraph header found as:" << endl;
        METHOD_OUTPUT << ln.asString() << endl;
      }

      enterLastPara = (para == m_numberOfMiddleParaHeader);
      ParaHeader paraHeader;
      paraHeader.m_startNumber = LineNumber::getStartNumber();
      paraHeader.m_currentParaNo = para++;
      paraHeader.m_color = separatorColor;
      paraHeader.m_hidden = hideParaHeader;
      paraHeader.m_lastPara = enterLastPara;
      paraHeader.markAsMiddleParaHeader();
      paraHeader.fixFromTemplate();
      outfile << paraHeader.getFixedResult() << endl;
      if (not enterLastPara) {
        lineNo = 1; // LINE index within each group
        expectAnotherHalf = false;
      }
      continue;
    }

    if (isEmptyLine(inLine)) {
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "processed empty line." << endl;
        METHOD_OUTPUT << inLine << endl;
      }
      outfile << inLine << endl;
      continue;
    }

    if (readLinesAfterImageGroup == true) {
      // output middle para Header
      if (linesAfter == 0) {
        readLinesAfterImageGroup = false;
        if (needToGenerateParaHeader) {
          enterLastPara = (para == m_numberOfMiddleParaHeader);
          ParaHeader paraHeader;
          paraHeader.m_startNumber = LineNumber::getStartNumber();
          paraHeader.m_currentParaNo = para++;
          paraHeader.m_color = separatorColor;
          paraHeader.m_hidden = hideParaHeader;
          paraHeader.m_lastPara = enterLastPara;
          paraHeader.markAsMiddleParaHeader();
          paraHeader.fixFromTemplate();
          outfile << paraHeader.getFixedResult() << endl;
          if (not enterLastPara) {
            lineNo = 1; // LINE index within each group
            expectAnotherHalf = false;
          }
        }
        // already read in one line, normally the next <br> after image Group
        if (isLeadingBr(inLine)) {
          outfile << inLine << endl;
          expectAnotherHalf = true;
        }
      } else {
        outfile << inLine << endl;
        linesAfter--;
      }
      continue;
    }

    if (isImageGroupLine(inLine)) {
      seqOfImageGroup++;
      linesAfter = m_brTable[seqOfImageGroup].first;
      needToGenerateParaHeader = m_brTable[seqOfImageGroup].second;
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "processed image Group line." << endl;
        METHOD_OUTPUT << inLine << endl;
        METHOD_OUTPUT << "linesAfter :" << endl;
        METHOD_OUTPUT << linesAfter << endl;
        METHOD_OUTPUT << "needToGenerateParaHeader :" << endl;
        METHOD_OUTPUT << needToGenerateParaHeader << endl;
      }
      outfile << inLine << endl;
      readLinesAfterImageGroup = true;
      continue;
    }

    if (expectAnotherHalf) {
      if (isLeadingBr(inLine)) {
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "processed :" << endl;
        METHOD_OUTPUT << inLine << endl;
        outfile << inLine << endl;
        // still expectAnotherHalf
        continue;
      }
      if (hasEndingBr(inLine)) {
        LineNumber newLn(para, lineNo);
        if (forceUpdate or not ln.equal(newLn)) {
          if (ln.valid()) // remove old line number
          {
            removeOldLineNumber(inLine);
          }
          removeNbspsAndSpaces(inLine);
          outfile << newLn.getWholeString() << doubleSpace << displaySpace
                  << inLine << endl; // Prints our line
        } else
          outfile << inLine << endl;
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << "processed :" << inLine << endl;
        lineNo++;
        expectAnotherHalf = false;
        continue;
      }
    } else {
      // must followed by a leading BR
      if (isLeadingBr(inLine)) {
        outfile << inLine << endl;
        expectAnotherHalf = true;
      } else {
        METHOD_OUTPUT << "expectAnotherHalf: " << expectAnotherHalf
                      << " wrong without leading " << brTab << " at line: "
                      << TurnToString(para) + "." + TurnToString(lineNo)
                      << " of file: " << m_inputFile << "content: " << inLine
                      << endl;
        exit(1);
        break;
      }
    }
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "numbering finished." << endl;
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
