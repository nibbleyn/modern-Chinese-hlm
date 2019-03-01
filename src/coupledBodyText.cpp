#include "coupledBodyText.hpp"

bool KeyStartNotFound(const string &testStr, const string &key) {
  if (debug >= LOG_INFO)
    cout << testStr << " and key: " << key << endl;
  auto keyStartBegin = testStr.find(keyStartChars);
  if (keyStartBegin == string::npos)
    return true;
  return false;
}

bool isOnlyPartOfOtherKeys(const string &orgLine, const string &key) {
  string line = orgLine;
  while (true) {
    auto keyBegin = line.find(key);
    if (keyBegin == string::npos) // no key any more
      break;
    auto keyEnd = line.find(keyEndChars, keyBegin);
    if (keyEnd == string::npos) // no keyEnd any more
      return false;
    string testStr = line.substr(keyBegin, keyEnd - keyBegin);
    if (not KeyStartNotFound(testStr, key))
      return false;
    else {
      string beforeKey = line.substr(0, keyBegin);
      if (debug >= LOG_INFO)
        cout << beforeKey << endl;
      // if only part of comment
      if (beforeKey.find(keyStartChars) == string::npos)
        return false;
    }
    line = line.substr(keyEnd + keyEndChars.length());
    if (debug >= LOG_INFO)
      cout << line << endl;
  }
  return true;
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

void CoupledBodyText::setFilePrefixFromFileType(FILE_TYPE type) {
  if (type == FILE_TYPE::MAIN)
    m_filePrefix = MAIN_BODYTEXT_PREFIX;
  if (type == FILE_TYPE::ATTACHMENT)
    m_filePrefix = ATTACHMENT_BODYTEXT_PREFIX;
  if (type == FILE_TYPE::ORIGINAL)
    m_filePrefix = ORIGINAL_BODYTEXT_PREFIX;
  if (type == FILE_TYPE::JPM)
    m_filePrefix = JPM_BODYTEXT_PREFIX;
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
    cout << "input file is: " << m_inputFile << endl;
    cout << "output file is: " << m_outputFile << endl;
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
    cout << m_searchError << endl;
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
    // if "key" is only part of the key of another link, skip this line
    if (isOnlyPartOfOtherKeys(line, key)) {
      if (debug >= LOG_INFO)
        cout << "found as key only: " << line << endl;
      continue;
    }

    LineNumber ln;
    ln.loadFirstFromContainedLine(line);
    if (not ln.valid()) {
      m_searchError =
          "file doesn't get numbered:" + m_inputFile + " at line:" + line;
      cout << m_searchError << endl;
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
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  cout << utf8length(sampleBlock) << endl;
  cout << sampleBlock << endl;
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
          cout << outputLine << CR << CRLF;
        outfile << outputLine << CR << CRLF;
      }
      if (utf8length(line) < utf8length(sampleBlock) - 1)
        break;
    } while (true);
  }
  if (debug >= LOG_INFO)
    cout << "reformatting finished." << endl;
}

// regrouping to make total size smaller
void CoupledBodyText::regroupingParagraphs(const string &sampleBlock,
                                           const string &sampleFirstLine,
                                           const string &sampleWholeLine) {
  if (debug >= LOG_INFO)
    cout << "regrouping finished." << endl;
}

/**
 * count number of lines with lineNumberIdBeginChars as paragraph sign in it
 * return a tuple of numbers of first paragraph header,
 * middle header and last header
 * @param fullPath of target file
 * @return a tuple of numbers
 */
CoupledBodyText::ParaStruct CoupledBodyText::getNumberOfPara() {
  setInputOutputFiles();

  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return std::make_tuple(0, 0, 0);
  }
  int first = 0, middle = 0, last = 0;
  string inLine;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (inLine.find(topIdBeginChars) != string::npos) {
      first++; // end of whole FILE_TYPE::MAIN file
      continue;
    }
    if (inLine.find(bottomIdBeginChars) != string::npos) {
      last++;
      return std::make_tuple(first, middle,
                             last); // end of whole FILE_TYPE::MAIN file
    }
    // search for new paragraph and also fix the para number
    if (inLine.find(lineNumberIdBeginChars) !=
        string::npos) // one new paragraph is found
    {
      LineNumber ln;
      ln.loadFirstFromContainedLine(inLine);
      if (ln.isParagraphHeader())
        middle++;
    }
  }
  return std::make_tuple(first, middle, last);
}

void CoupledBodyText::removeNbspsAndSpaces(string &inLine) {
  // remove existing nbsps and spaces
  auto iterBegin = inLine.begin();
  while (iterBegin < inLine.end()) {
    if (*iterBegin == displaySpace.at(0))
      ++iterBegin;
    else if (*iterBegin == space.at(0) and
             (iterBegin + space.length() < inLine.end()) and
             (std::string(iterBegin, iterBegin + space.length()) == space)) {
      iterBegin += space.length();
    } else
      break;
  }
  inLine = std::string(iterBegin, inLine.end());
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
                                    bool forceUpdate, bool hidden) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  int numberOfFirstParaHeader{1};
  int numberOfMiddleParaHeader{0};
  int numberOfLastParaHeader{1};
  if (not m_autoNumbering) {
    ParaStruct res;
    res = getNumberOfPara(); // first scan
    numberOfFirstParaHeader = GetTupleElement(res, 0);
    numberOfMiddleParaHeader = GetTupleElement(res, 1);
    numberOfLastParaHeader = GetTupleElement(res, 2);

    if (debug >= LOG_INFO)
      cout << "numberOfFirstParaHeader: " << numberOfFirstParaHeader
           << " numberOfMiddleParaHeader: " << numberOfMiddleParaHeader
           << " numberOfLastParaHeader: " << numberOfLastParaHeader << endl;
    if (numberOfFirstParaHeader == 0 or numberOfLastParaHeader == 0) {
      cout << "no top or bottom paragraph found:" << m_inputFile << endl;
      return;
      LineNumber::setStartNumber(START_PARA_NUMBER);
    }

  } else {
    // remove img tag first and add them afterwards
    cout << "not supported now." << endl;
    return;
  }

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
      if (debug >= LOG_INFO)
        cout << "paragraph header found as:" << ln.asString() << endl;
      line = fixFirstParaHeaderFromTemplate(LineNumber::getStartNumber(),
                                            separatorColor, hidden);
      stop = true;
    }
    // incl. above header
    outfile << line << endl;
  }

  bool expectAnotherHalf = false;
  int para = 1;   // paragraph index
  int lineNo = 1; // LINE index within each group
  bool enterLastPara = (numberOfMiddleParaHeader == 0);
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (enterLastPara == true) {
      if (ln.isParagraphHeader()) {
        if (debug >= LOG_INFO)
          cout << ln.asString() << endl;
        outfile << fixLastParaHeaderFromTemplate(LineNumber::getStartNumber(),
                                                 numberOfMiddleParaHeader + 1,
                                                 separatorColor, hidden)
                << endl;
        break; // end of whole file
      }
    }

    if (ln.isParagraphHeader()) {
      if (debug >= LOG_INFO)
        cout << "paragraph header found as:" << ln.asString() << endl;

      if (para == numberOfMiddleParaHeader) {
        enterLastPara = true;
        outfile << fixMiddleParaHeaderFromTemplate(LineNumber::getStartNumber(),
                                                   para++, separatorColor,
                                                   hidden, true)
                << endl;
      } else
        outfile << fixMiddleParaHeaderFromTemplate(LineNumber::getStartNumber(),
                                                   para++, separatorColor,
                                                   hidden, false)
                << endl;
      lineNo = 1; // LINE index within each group
      expectAnotherHalf = false;
      continue;
    }

    if (isEmptyLine(inLine)) {
      if (debug >= LOG_INFO)
        cout << "processed empty line." << inLine << endl;
      outfile << inLine << endl;
      continue;
    }

    if (expectAnotherHalf) {
      if (isLeadingBr(inLine)) {
        if (debug >= LOG_INFO)
          cout << "processed :" << inLine << endl;
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
          cout << "processed :" << inLine << endl;
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
        cout << "expectAnotherHalf: " << expectAnotherHalf
             << " wrong without leading " << brTab
             << " at line: " << TurnToString(para) + "." + TurnToString(lineNo)
             << " of file: " << m_inputFile << "content: " << inLine << endl;
        exit(1);
        break;
      }
    }
  }
  if (debug >= LOG_INFO)
    cout << "numbering finished." << endl;
}

void testSearchTextIsOnlyPartOfOtherKeys() {
  string line =
      R"(秋水眼又对秋水鸳鸯剑，埋下<a unhidden href="#P94">)" + keyStartChars +
      R"(春色)" + keyEndChars + R"(“倒底是不标致的好”</a>的悲剧结局)";
  cout << line << endl;
  line =
      R"(满脸春色，比白日更增了颜色)" + commentStart + R"(美丽)" + commentEnd +
      R"(）。贾琏搂她笑道：“人人都说我们那<a href="a044.htm#P94">)" +
      keyStartChars + R"(夜叉星)" + keyEndChars + R"(夜叉婆</a>)" +
      commentStart + R"(凤姐)" + commentEnd + R"(齐整)" + commentStart +
      R"(标致)" + commentEnd + R"(，如今我看来，)" + commentStart +
      R"(俯就你)" + commentEnd +
      R"(给你拾鞋也不要。”尤二姐道：“我虽标致，却无<a href="a066.htm#P94">)" +
      keyStartChars + R"(品行)" + keyEndChars +
      R"(品行</a>。看来倒底是不标致的好。)";
  cout << line << endl;
  string key = R"(春色)";
  cout << isOnlyPartOfOtherKeys(line, key) << endl;
  ;
}

void testLineHeader(string lnStr) {
  cout << "original: " << endl << lnStr << endl;
  LineNumber ln(lnStr);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  cout << "whole string: " << ln.getWholeString() << endl;
  cout << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testLineHeaderFromContainedLine(string containedLine) {
  cout << "original: " << endl << containedLine << endl;
  LineNumber ln;
  ln.loadFirstFromContainedLine(containedLine);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  cout << "whole string: " << ln.getWholeString() << endl;
  cout << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testParagraphHeader(string lnStr) {
  cout << "original: " << endl << lnStr << endl;
  LineNumber ln(lnStr);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  cout << "whole string: " << ln.getWholeString() << endl;
  cout << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testParagraphHeaderFromContainedLine(string containedLine) {
  cout << "original: " << endl << containedLine << endl;
  LineNumber ln;
  ln.loadFirstFromContainedLine(containedLine);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  cout << "whole string: " << ln.getWholeString() << endl;
  cout << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testLineNumber() {
  LineNumber::setStartNumber(START_PARA_NUMBER);
  testLineHeader("P1L4");
  SEPERATE("ln1", " finished ");
  testLineHeader("P13L4");
  SEPERATE("ln2", " finished ");
  testLineHeaderFromContainedLine(
      R"(<a unhidden id="P12L8">12.8</a>　　原来 这一个名唤 贾蔷，也系 宁国府 正派玄孙，父母早亡，从小 跟贾珍过活，如今 长了十六岁，比 贾蓉 生得还 风流俊俏。贾蓉、贾蔷兄弟二人最相亲厚，常相共处。<br>)");
  SEPERATE("ln3", " finished ");

  testParagraphHeaderFromContainedLine(fixFirstParaHeaderFromTemplate(
      LineNumber::getStartNumber(), getSeparateLineColor(FILE_TYPE::MAIN)));
  SEPERATE("ln4", " finished ");
  testParagraphHeaderFromContainedLine(fixMiddleParaHeaderFromTemplate(
      LineNumber::getStartNumber(), 1, getSeparateLineColor(FILE_TYPE::MAIN),
      false));
  SEPERATE("ln5", " finished ");
  testParagraphHeaderFromContainedLine(fixLastParaHeaderFromTemplate(
      LineNumber::getStartNumber(), 12, getSeparateLineColor(FILE_TYPE::MAIN)));
  SEPERATE("ln6", " finished ");
  testLineHeaderFromContainedLine(R"(<br>)");
  SEPERATE("ln7", " finished ");
  testLineHeaderFromContainedLine(R"(anything)");
  SEPERATE("ln8", " finished ");
}

void testConstructSubStory() {
  CoupledBodyText bodyText;
  bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
  bodyText.setFileAndAttachmentNumber("06");
  CoupledBodyText::ParaStruct res = bodyText.getNumberOfPara();
  cout << GetTupleElement(res, 0) << " " << GetTupleElement(res, 1) << " "
       << GetTupleElement(res, 2) << endl;
}
