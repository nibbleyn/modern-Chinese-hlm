#include "coupledBodyText.hpp"
#include <functional> // std::greater

bool KeyStartNotFound(const string &testStr, const string &key) {
  if (debug >= LOG_INFO)
    cout << testStr << " and key: " << key << endl;
  auto keyStartBegin = testStr.find(keyStartChars);
  if (keyStartBegin == string::npos)
    return true;
  return false;
}

bool isFoundAsNonKeys(const string &line, const string &key) {
  unsigned int searchStart = 0;
  while (true) {
    auto keyBegin = line.find(key, searchStart);
    // no key found any more
    if (keyBegin == string::npos)
      break;
    auto testBeginPos = line.rfind(keyStartChars, keyBegin);
    // if keyStartChars exists
    if (testBeginPos != string::npos and testBeginPos > searchStart) {
      string testStr =
          line.substr(testBeginPos + keyStartChars.length(),
                      keyBegin - testBeginPos - keyStartChars.length());
      cout << testStr << " and key: " << key << endl;
      if (testStr.find(keyEndChars) != string::npos)
        return true;
    } else {
      testBeginPos = line.rfind(titleStartChars, keyBegin);
      // if titleStartChars exists
      if (testBeginPos != string::npos and testBeginPos > searchStart) {
        string testStr =
            line.substr(testBeginPos + titleStartChars.length(),
                        keyBegin - testBeginPos - titleStartChars.length());
        cout << testStr << " and key: " << key << endl;
        if (testStr.find(titleEndChars) != string::npos)
          return true;
      } else
        return true;
    }
    searchStart = keyBegin + 1;
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
    if (not isFoundAsNonKeys(line, key)) {
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

  string line1 =
      R"(弄得（<cite unhidden>宝玉</cite>）情色若痴，语言常乱，似染怔忡之疾，慌得袭人等又不敢回贾母，只百般逗他玩笑（<cite unhidden>指望他早日康复</cite>）。（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">怔忡，为病名，首见于《济生方·惊悸怔忡健忘门》中“惊者，心卒动而不宁也；悸者,心跳动而怕惊也；怔忡者，心中躁动不安，惕惕然后人将捕之也”,是心悸的一种，是指多因久病体虚、心脏受损导致气血、阴阳亏虚，或邪毒、痰饮、瘀血阻滞心脉，日久导致心失濡养，心脉不畅，从而引起的心中剔剔不安，不能自控的一种病证，常和惊悸合并称为心悸</u>）)";
  string key = R"(怔忡)";
  cout << line1 << endl;
  cout << isFoundAsNonKeys(line1, key) << endl;
  string line2 =
      R"(（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">宝玉真病了的时候，不过袭人<a unhidden href="#P94" title="怔忡"><i hidden>怔忡</i>瞒着不回贾母</a>，晨昏定省还是去的</u>）。宝玉方（<cite unhidden>放下笔，先</cite>）去请安（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">贾母关切，姊妹们则看在眼里</u>）)";
  cout << line2 << endl;
  cout << isFoundAsNonKeys(line2, key) << endl;
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
