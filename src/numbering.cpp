#include "numbering.hpp"

static const string firstParaHeader =
    R"(<b unhidden> 第1段 </b><a unhidden name="PXX" href="#PYY">v向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden name="top" href="#bottom">页面底部->||</a><hr color="#COLOR">)";
static const string MiddleParaHeader =
    R"(<hr color="#COLOR"><b unhidden> 第ZZ段 </b><a unhidden name="PXX" href="#PYY">向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden name="PWW" href="#PUU">向上</a><hr color="#COLOR">)";
static const string lastParaHeader =
    R"(<hr color="#COLOR"><a unhidden name="bottom" href="#top">||<-页面顶部</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden name="PXX" href="#PYY">^向上</a><hr color="#COLOR">)";

/**
 * generate real first Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @return first Paragragh header after fixed
 */
string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden) {
  string link = firstParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber));
  link = replacePart(link, "YY", TurnToString(startNumber + 1));

  return link;
}

/**
 * generate real middle Paragragh header
 * by filling right number of this paragraph
 * right name for uplink and downlink
 * and right href to uplink and downlink
 * and right color for different files
 * and if this is last middle paragraph
 * make downlink pointing to bottom
 * @param startNumber the number of first paragraph header
 * @param currentParaNo number of paragraphs before this header
 * @param color the separator line color
 * @param lastPara whether this is the last middle paragraph
 * @return Paragragh header after fixed
 */
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden,
                                       bool lastPara) {
  string link = MiddleParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber + currentParaNo));
  if (lastPara == true) {
    link = replacePart(link, "PYY", R"(bottom)");
  } else
    link =
        replacePart(link, "YY", TurnToString(startNumber + currentParaNo + 1));
  link = replacePart(link, "ZZ", TurnToString(currentParaNo + 1));
  link = replacePart(link, "WW", TurnToString(startNumber - currentParaNo));
  link = replacePart(link, "UU", TurnToString(startNumber - currentParaNo + 1));
  return link;
}

/**
 * generate real last Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @param lastParaNo number of paragraphs before this header
 * @param color the separator line color
 * @return last Paragragh header after fixed
 */
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden) {
  string link = lastParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber - lastParaNo));
  link = replacePart(link, "YY", TurnToString(startNumber - lastParaNo + 1));
  return link;
}

/**
 * count number of lines with R"(name=")" of paragraph sign in it
 * return a tuple of numbers of first paragraph header,
 * middle header and last header
 * @param fullPath of target file
 * @return a tuple of numbers
 */
ParaStruct getNumberOfPara(const string &inputFile) {
  int first = 0, middle = 0, last = 0;
  string paraTab =
      R"(name=")"; // of each paragraph
  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return std::make_tuple(first, middle, last);
  }
  string inLine;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (inLine.find(topTab) != string::npos) {
      first++; // end of whole FILE_TYPE::MAIN file
      continue;
    }
    if (inLine.find(bottomTab) != string::npos) {
      last++;
      return std::make_tuple(first, middle,
                             last); // end of whole FILE_TYPE::MAIN file
    }
    // search for new paragraph and also fix the para number
    if (inLine.find(paraTab) != string::npos) // one new paragraph is found
    {
      LineNumber ln;
      ln.loadFromContainedLine(inLine);
      if (ln.isParagraphHeader())
        middle++;
    }
  }
  return std::make_tuple(first, middle, last);
}

/**
 * add following line name with a name of lineNumber
 * before each line
 * <a unhidden name="P4L2">4.2</a>
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
void addLineNumber(const string &inputFile, const string &outputFile,
                   const string &separatorColor, bool hidden = false) {
  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  LineNumber::setStartNumber(START_PARA_NUMBER);

  ParaStruct res = getNumberOfPara(inputFile); // first scan
  auto numberOfFirstParaHeader = GetTupleElement(res, 0);
  auto numberOfMiddleParaHeader = GetTupleElement(res, 1);
  auto numberOfLastParaHeader = GetTupleElement(res, 2);

  if (debug >= LOG_INFO)
    cout << "numberOfFirstParaHeader: " << numberOfFirstParaHeader
         << "numberOfMiddleParaHeader: " << numberOfMiddleParaHeader
         << "numberOfLastParaHeader: " << numberOfLastParaHeader << endl;

  if (numberOfFirstParaHeader == 0 or numberOfLastParaHeader == 0) {
    cout << "no top or bottom paragraph found:" << inputFile << endl;
    return;
  }

  ofstream outfile(outputFile);

  // continue reading till first paragraph header
  string inLine;
  bool stop = false; // need to output line even try to stop
  while (!infile.eof() and
         not stop) // To get all the lines till first paragraph header
  {
    getline(infile, inLine); // Saves the line in inLine.
    string line = inLine;
    LineNumber ln;
    ln.loadFromContainedLine(inLine);
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

  enum class STATE { EXPECT_MIDDLE_PARA_HEADER, LINEFOUND, LINECOMPLETED };
  STATE state = STATE::EXPECT_MIDDLE_PARA_HEADER;
  int para = 1;          // paragraph index
  int lineNo = 1;        // LINE index within each group
  string brTab = "<br>"; // start and end of each LINE
  bool enterLastPara = (numberOfMiddleParaHeader == 0);
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFromContainedLine(inLine);
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

    // can keep STATE::EXPECT_MIDDLE_PARA_HEADER or enter STATE::LINEFOUND
    if (state == STATE::EXPECT_MIDDLE_PARA_HEADER) {
      if (ln.isParagraphHeader()) {
        if (debug >= LOG_INFO)
          cout << "paragraph header found as:" << ln.asString() << endl;

        if (para == numberOfMiddleParaHeader) {
          enterLastPara = true;
          outfile << fixMiddleParaHeaderFromTemplate(
                         LineNumber::getStartNumber(), para++, separatorColor,
                         hidden, true)
                  << endl;
        } else
          outfile << fixMiddleParaHeaderFromTemplate(
                         LineNumber::getStartNumber(), para++, separatorColor,
                         hidden, false)
                  << endl;
        lineNo = 1; // LINE index within each group
        continue;
      }
      // search for LINE start
      if (inLine.find(brTab) != string::npos) {
        state = STATE::LINEFOUND; // the LINE start
      }
      // also incl. non-LINE start line
      outfile << inLine << endl;
      continue;
    }

    // can only enter STATE::LINECOMPLETED,
    // otherwise a line error reported and abort
    if (state == STATE::LINEFOUND) {
      if (ln.isParagraphHeader() or
          inLine.find(brTab) ==
              string::npos) // search for LINE end, even an empty line
      {
        cout << "wrong without " << brTab
             << " at line: " << TurnToString(para) + "." + TurnToString(lineNo)
             << " of file: " << inputFile << "content: " << inLine << endl;
        exit(1);
        break;
      }
      if (debug >= LOG_INFO)
        cout << "processing paragraph:" << para << " line: " << lineNo << endl;
      LineNumber newLn(para, lineNo);
      if (not ln.equal(newLn)) {
        if (ln.valid()) // remove old one
        {
          auto linkEnd = inLine.find("</a>");
          inLine = inLine.substr(linkEnd + 4);
        }
        outfile << newLn.generateLinePrefix() + TurnToString(para) + "." +
                       TurnToString(lineNo) + "</a>"
                << "    " << inLine << endl; // Prints our line
      } else
        outfile << inLine << endl;
      if (debug >= LOG_INFO)
        cout << "processed :" << inLine << endl;
      lineNo++;
      state = STATE::LINECOMPLETED;
      continue;
    }

    // can enter STATE::LINEFOUND or STATE::EXPECT_MIDDLE_PARA_HEADER
    if (state == STATE::LINECOMPLETED) {
      if (ln.isParagraphHeader()) {
        if (debug >= LOG_INFO)
          cout << "paragraph header found as:" << ln.asString() << endl;

        if (para == numberOfMiddleParaHeader) {
          enterLastPara = true;
          outfile << fixMiddleParaHeaderFromTemplate(
                         LineNumber::getStartNumber(), para++, separatorColor,
                         hidden, true)
                  << endl;
        } else {
          outfile << fixMiddleParaHeaderFromTemplate(
                         LineNumber::getStartNumber(), para++, separatorColor,
                         hidden, false)
                  << endl;
          state = STATE::EXPECT_MIDDLE_PARA_HEADER;
        }
        lineNo = 1; // LINE index within each group
        continue;
      }
      if (inLine.find(brTab) != string::npos) // another LINE start
      {
        state = STATE::LINEFOUND;
      }
      // also incl. non-LINE start
      outfile << inLine << endl;
      continue;
    }
  }
  if (debug >= LOG_INFO)
    cout << "numbering finished." << endl;
}

/**
 * numbering lines a set of Original body text files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void addLineNumbersForOriginalHtml(int minTarget, int maxTarget,
                                   bool hidden = false) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputFile = BODY_TEXT_OUTPUT +
                       getBodyTextFilePrefix(FILE_TYPE::ORIGINAL) + file +
                       ".txt";
    string outputFile = BODY_TEXT_FIX +
                        getBodyTextFilePrefix(FILE_TYPE::ORIGINAL) + file +
                        ".txt";
    string separatorColor = getSeparateLineColor(FILE_TYPE::ORIGINAL);
    addLineNumber(inputFile, outputFile, separatorColor, hidden);
  }
}

/**
 * numbering lines of a set of Main body text files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void addLineNumbersForMainHtml(int minTarget, int maxTarget,
                               bool hidden = false) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputFile = BODY_TEXT_OUTPUT +
                       getBodyTextFilePrefix(FILE_TYPE::MAIN) + file + ".txt";
    string outputFile =
        BODY_TEXT_FIX + getBodyTextFilePrefix(FILE_TYPE::MAIN) + file + ".txt";
    string separatorColor = getSeparateLineColor(FILE_TYPE::MAIN);
    addLineNumber(inputFile, outputFile, separatorColor, hidden);
  }
}

/**
 * numbering lines of a set of Attachment body text files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * Numbering all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void addLineNumbersForAttachmentHtml(int minTarget, int maxTarget,
                                     int minAttachNo, int maxAttachNo,
                                     bool hidden = false) {

  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (not(minAttachNo == 0 and maxAttachNo == 0) and
      minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      string attach_file = file + R"(_)" + TurnToString(attNo);
      if (debug >= LOG_INFO)
        cout << "line numbering for: b0" << attach_file << ".htm" << endl;
      string inputFile = BODY_TEXT_OUTPUT +
                         getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) +
                         attach_file + ".txt";

      string outputFile = BODY_TEXT_FIX +
                          getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) +
                          attach_file + ".txt";
      string separatorColor = getSeparateLineColor(FILE_TYPE::ATTACHMENT);
      addLineNumber(inputFile, outputFile, separatorColor, hidden);
    }
  }
}

/**
 * Numbering a set of original Html files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void NumberingOriginalHtml(int minTarget, int maxTarget, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  container.backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.dissembleFromHTM(file);
  }
  addLineNumbersForOriginalHtml(
      minTarget, maxTarget, hidden); // reformat bodytext by adding line number
  BodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
}

/**
 * Numbering a set of Main Html files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void NumberingMainHtml(int minTarget, int maxTarget, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::MAIN);
  container.backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.dissembleFromHTM(file);
  }
  addLineNumbersForMainHtml(minTarget, maxTarget,
                            hidden); // reformat bodytext by adding line number

  BodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
  cout << "Numbering Main Html finished. " << endl;
}

/**
 * Numbering a set of Attachment Html files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * Numbering all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void NumberingAttachmentHtml(int minTarget, int maxTarget, int minAttachNo,
                             int maxAttachNo, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  container.backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  addLineNumbersForAttachmentHtml(
      minTarget, maxTarget, minAttachNo, maxAttachNo,
      hidden); // reformat bodytext by adding line number
  BodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  NumberingMainHtml(minTarget, maxTarget, hidden);
}
/**
 * copy original files into HTML_SRC_ORIGINAL
 * before run this
 */
void numberOriginalHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  NumberingOriginalHtml(minTarget, maxTarget, hidden);
  cout << "Numbering Original Html finished. " << endl;
}

/**
 * copy attachment files into HTML_SRC_ATTACHMENT
 * before run this
 */
void numberAttachmentHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 0, maxAttachNo = 50;
  NumberingAttachmentHtml(minTarget, maxTarget, minAttachNo, maxAttachNo,
                          hidden);
  cout << "Numbering Attachment Html finished. " << endl;
}

#include "utf8StringUtil.hpp"
void reformatFile(const string &inputFile, const string &outputFile,
                  const string &example) {
  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  ofstream outfile(outputFile);
  cout << utf8length(example) << endl;
  cout << example << endl;
  // continue reading
  string inLine;
  string CR{0x0D};
  string LF{0x0A};
  string CRLF{0x0D, 0x0A};
  while (!infile.eof()) {
    getline(infile, inLine); // Saves the line in inLine.
    size_t end = -1;
    do {
      string line = utf8substr(inLine, end + 1, end, utf8length(example));
      if (not line.empty()) {
        auto outputLine = line + CRLF;
        if (debug >= LOG_INFO)
          cout << outputLine << CR << CRLF;
        outfile << outputLine << CR << CRLF;
      }
      if (utf8length(line) < utf8length(example) - 1)
        break;
    } while (true);
  }
  if (debug >= LOG_INFO)
    cout << "reformatting finished." << endl;
}

/**
 *
 */
void reformatTxtFiles(int minTarget, int maxTarget, const string &example) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string filename = file;
    if (filename.length() == 2) {
      filename = "0" + filename;
    }
    string inputFile = BODY_TEXT_OUTPUT + "pjpm" + filename + ".txt";
    string outputFile = BODY_TEXT_FIX + "pjpm" + filename + ".txt";
    reformatFile(inputFile, outputFile, example);
  }
}

/**
 *
 */
void reformatTxtFilesForReader() {
  const string example =
      R"(话说安童领着书信，辞了黄通判，径往山东大道而来。打听巡按御史在东昌府住扎，姓曾，双名孝序，【夹批：曾者，争也。序即天叙有典之叙，盖作者为世所厄不能自全其孝，故抑郁愤懑)";

  int minTarget = 1, maxTarget = 100;
  reformatTxtFiles(minTarget, maxTarget, example);
  cout << "reformat files from " << minTarget << " to " << maxTarget
       << " finished. " << endl;
}

void splitFile(const string &sampleBlock, const string &sampleFirstLine,
               const string &sampleWholeLine) {
  string file = "01";
  string inputFile =
      BODY_TEXT_OUTPUT + getBodyTextFilePrefix(FILE_TYPE::MAIN) + file + ".txt";
}

void autoSplitBodyText() {
  const string sampleBlock = R"()";
  const string sampleFirstLine = R"()";
  const string sampleWholeLine = R"()";
  splitFile(sampleBlock, sampleFirstLine, sampleWholeLine);
}
