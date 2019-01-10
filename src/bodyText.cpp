#include "bodyText.hpp"

bool KeyStartAndCommentStartNotFound(const string &testStr, const string &key) {
  if (debug >= LOG_INFO)
    cout << testStr << " and key: " << key << endl;
  auto keyStartBegin = testStr.find(keyStartChars);
  auto commentStartBegin = testStr.find(commentBeginChars);
  if (keyStartBegin == string::npos and commentStartBegin == string::npos)
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
    if (not KeyStartAndCommentStartNotFound(testStr, key))
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
bool BodyText::findKey(const string &key, const string &file, int attachNo) {
  string attachmentPart{""};
  if (attachNo != 0)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);
  string fullPath =
      BODY_TEXT_OUTPUT + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;

  ifstream infile(fullPath);
  if (!infile) {
    searchError = "file doesn't exist:" + fullPath;
    cout << searchError << endl;
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
    ln.loadFromContainedLine(line);
    if (not ln.valid()) {
      searchError =
          "file doesn't get numbered:" + fullPath + " at line:" + line;
      cout << searchError << endl;
      return false;
    }

    // special hack for ignoring one lineNumber
    if (not ignoreSet.empty() and
        ignoreSet.find(ln.asString()) != ignoreSet.end()) {
      continue;
    }

    found = true;
    result.insert(ln.asString());
    if (onlyFirst) {
      break;
    }
  }
  return found;
}

// reformat to smaller paragraphs
void BodyText::reformatParagraphToSmallerSize(const string &sampleBlock, const string &file){
    string inputFile = BODY_TEXT_OUTPUT + filePrefix + file + BODY_TEXT_SUFFIX;
    string outputFile = BODY_TEXT_FIX + filePrefix + file + BODY_TEXT_SUFFIX;

    ifstream infile(inputFile);
    if (!infile) {
      cout << "file doesn't exist:" << inputFile << endl;
      return;
    }
    ofstream outfile(outputFile);
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
void BodyText::regroupingParagraphs(const string &sampleBlock, const string &sampleFirstLine,
        const string &sampleWholeLine, const string &file){
    if (debug >= LOG_INFO)
      cout << "regrouping finished." << endl;

}


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

// add line number before each paragraph
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
void BodyText::addLineNumber(const string &separatorColor, const string &file, int attachNo, bool hidden){
	  string attachmentPart{""};
	  if (attachNo != 0)
	    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

    string inputFile = BODY_TEXT_OUTPUT + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;
    string outputFile = BODY_TEXT_FIX  + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;

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
