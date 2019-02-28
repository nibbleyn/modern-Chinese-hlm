#include "tools.hpp"
#include <regex>

void fixFooter(string &footer, const string &filename) {
  string previous =
      formatIntoZeroPatchedChapterNumber(TurnToInt(filename) - 1, 3);
  string next = formatIntoZeroPatchedChapterNumber(TurnToInt(filename) + 1, 3);
  const string toReplacePrevious = "a077";
  const string toReplaceNext = "a079";

  do {
    auto toReplacePreviousBegin = footer.find(toReplacePrevious);
    if (toReplacePreviousBegin == string::npos)
      break;
    footer.replace(toReplacePreviousBegin, toReplacePrevious.length(),
                   getHtmlFileNamePrefix(FILE_TYPE::JPM) + previous);
  } while (true);
  do {
    auto toReplaceNextBegin = footer.find(toReplaceNext);
    if (toReplaceNextBegin == string::npos)
      break;
    footer.replace(toReplaceNextBegin, toReplaceNext.length(),
                   getHtmlFileNamePrefix(FILE_TYPE::JPM) + next);
  } while (true);
  const string toReplaceContentTable = "aindex";
  if (footer.find(toReplaceContentTable) == string::npos)
    return;
  const string newContentTable =
      getHtmlFileNamePrefix(FILE_TYPE::JPM) + "index";
  footer.replace(footer.find(toReplaceContentTable),
                 toReplaceContentTable.length(), newContentTable);
  cout << footer << endl;
}

void fixHeader(string &header, const string &filename) {

  string previous =
      formatIntoZeroPatchedChapterNumber(TurnToInt(filename) - 1, 3);
  string next = formatIntoZeroPatchedChapterNumber(TurnToInt(filename) + 1, 3);
  //  cout << previous << endl;
  //  cout << next << endl;
  const string origHeader = header;
  //  cout << origHeader << endl;
  const string originalTitleBeginChars = R"(<b unhidden>)";
  const string originalTitleEndChars = R"(</b>)";
  string originalTitle;
  auto originalTitleBegin = header.find(originalTitleBeginChars);
  if (originalTitleBegin == string::npos)
    return;
  auto originalTitleEnd =
      header.find(originalTitleEndChars, originalTitleBegin);
  originalTitle = header.substr(
      originalTitleBegin + originalTitleBeginChars.length(),
      originalTitleEnd - originalTitleBegin - originalTitleBeginChars.length());
  //  cout << originalTitle << endl;
  const string strongTitleBeginChars = R"(<strong unhidden>)";
  const string strongTitleEndChars = R"(</strong>)";
  header.replace(originalTitleBegin,
                 originalTitleBeginChars.length() + originalTitle.length() +
                     originalTitleEndChars.length(),
                 strongTitleBeginChars + originalTitle + strongTitleEndChars);
  //  cout << origHeader << endl;
  //  cout << header << endl;
  string start = htmlTitleStart;
  string end = htmlTitleEnd;
  auto titleBegin = header.find(start);
  if (titleBegin != string::npos) {
    auto titleEnd = header.find(end, titleBegin);
    if (titleEnd != string::npos)
      header.replace(titleBegin + start.length(),
                     titleEnd - titleBegin - start.length(), originalTitle);
  }

  //  cout << header << endl;
  const string toReplacePrevious = "a077";
  const string toReplaceNext = "a079";
  do {
    auto toReplacePreviousBegin = header.find(toReplacePrevious);
    if (toReplacePreviousBegin == string::npos)
      break;
    header.replace(toReplacePreviousBegin, toReplacePrevious.length(),
                   getHtmlFileNamePrefix(FILE_TYPE::JPM) + previous);
  } while (true);
  do {
    auto toReplaceNextBegin = header.find(toReplaceNext);
    if (toReplaceNextBegin == string::npos)
      break;
    header.replace(toReplaceNextBegin, toReplaceNext.length(),
                   getHtmlFileNamePrefix(FILE_TYPE::JPM) + next);
  } while (true);

  const string toReplaceTranslatedTitle =
      R"(第七十八回　老学士与贾政闲征姽婳词	痴公子贾宝玉为晴雯杜撰芙蓉诔)";
  auto toReplaceTranslatedTitleBegin = header.find(toReplaceTranslatedTitle);
  const string sampTitleBeginChars = R"(<samp unhidden>)";
  const string sampTitleEndChars = R"(</samp>)";
  header.replace(toReplaceTranslatedTitleBegin,
                 toReplaceTranslatedTitle.length(),
                 sampTitleBeginChars + originalTitle + sampTitleEndChars);

  const string toReplaceContentTable = "aindex";
  const string newContentTable =
      getHtmlFileNamePrefix(FILE_TYPE::JPM) + "index";
  header.replace(header.find(toReplaceContentTable),
                 toReplaceContentTable.length(), newContentTable);
  cout << header << endl;
}

void fixHeaderAndFooter(const string &filename) {
  string header =
      R"(<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf8"><style type="text/css">a {text-decoration: none}</style><title>第七十八回　老学士闲征姽婳词	痴公子杜撰芙蓉诔</title></head><body text="white" bgcolor="#004040" topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" rightmargin="0" link="#fff000" vlink="#fff000"><center>		<table border="0" cellspacing="1" width="80%"   style="font-size: 12pt; font-family: 宋体; line-height:150%"><tr><td style="font-size: 10.5pt"><hr color="#F0BEC0"><hr color="#F0BEC0"><a unhidden href="aindex.htm">>回目录</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a077.htm#bottom">上回结尾</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a077.htm">上回开始</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a079.htm">->下一回</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="#top">本回开始</a></td>	</tr><tr><td height="50" style="font-size: 15pt; font-family: 黑体; letter-spacing:1" align="center"><br><b unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</b><br>				<br>第七十八回　老学士与贾政闲征姽婳词	痴公子贾宝玉为晴雯杜撰芙蓉诔<br></td></tr><tr><td align="center"><hr color="#F0BEC0"></td></tr><tr><td>)";
  string headerCompareTo =
      R"(<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf8"><style type="text/css">a {text-decoration: none}</style><title>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</title></head><body text="white" bgcolor="#004040" topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" rightmargin="0" link="#fff000" vlink="#fff000"><center>		<table border="0" cellspacing="1" width="80%"   style="font-size: 12pt; font-family: 宋体; line-height:150%"><tr><td style="font-size: 10.5pt"><hr color="#F0BEC0"><hr color="#F0BEC0"><a unhidden href="dindex.htm">>回目录</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d083.htm#bottom">上回结尾</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d083.htm">上回开始</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d085.htm">->下一回</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="#top">本回开始</a></td>	</tr><tr><td height="50" style="font-size: 15pt; font-family: 黑体; letter-spacing:1" align="center"><br><strong unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</strong><br>				<br><samp unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</samp><br></td></tr><tr><td align="center"><hr color="#F0BEC0"></td></tr><tr><td>)";
  fixHeader(header, filename);
  cout << headerCompareTo << endl;
  cout << (header == headerCompareTo) << endl;
  string footer =
      R"(</td></tr><tr><td><hr color="#F0BEC0"></td></tr><tr><td align="center"><a unhidden href="a077.htm">上一回</a>&nbsp;<a unhidden href="aindex.htm">回目录</a>&nbsp;<a unhidden href="a079.htm">下一回</a>&nbsp;<hr color="#F0BEC0">	<hr color="#F0BEC0"></td></tr></table></center></body></html>)";
  string footerCompareTo =
      R"(</td></tr><tr><td><hr color="#F0BEC0"></td></tr><tr><td align="center"><a unhidden href="d083.htm">上一回</a>&nbsp;<a unhidden href="dindex.htm">回目录</a>&nbsp;<a unhidden href="d085.htm">下一回</a>&nbsp;<hr color="#F0BEC0">	<hr color="#F0BEC0"></td></tr></table></center></body></html>)";
  fixFooter(footer, filename);
  cout << footerCompareTo << endl;
  cout << (footer == footerCompareTo) << endl;
}

void CoupledBodyText::fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                                      const string &from, const string &to) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);

  // continue reading till first paragraph header
  string inLine{""};

  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    unsigned int before = 0, after = 0;
    auto orgLine = inLine; // inLine would change in loop below
    auto signBegin = inLine.find(signOfTagAfterReplaceTag);
    if (signBegin != string::npos) {
      // at most two
      after = orgLine.find(from, signBegin);
      auto beforeStr = inLine.substr(0, signBegin);
      before = beforeStr.find(from);
      if (after != string::npos) {
        orgLine.replace(after, from.length(), to);
      }
      if (before != string::npos) {
        orgLine.replace(before, from.length(), to);
      }
    }
    outfile << orgLine << endl;
  }
}

void CoupledBodyText::fixTagPairEnd(const string &signOfTagBeforeReplaceTag,
                                    const string &from, const string &to,
                                    const string &skipTagPairBegin) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  std::set<int, std::greater<int>> occurences;
  // continue reading till first paragraph header
  string inLine{""};
  auto cutLength{0};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    occurences.clear();
    cutLength = 0;
    auto orgLine = inLine; // inLine would change in loop below
    do {
      auto signBegin = inLine.find(signOfTagBeforeReplaceTag);
      if (signBegin == string::npos) // no signOfTagBeforeReplaceTag any more,
                                     // continue with next line
        break;
      auto fromBegin = inLine.find(from, signBegin);
      if (not skipTagPairBegin.empty()) {
        do {
          fromBegin = inLine.find(from, signBegin);
          if (fromBegin == string::npos) {
            cout << from << "is not found after: " << signOfTagBeforeReplaceTag
                 << endl;
            return;
          }
          auto inBetween = inLine.substr(signBegin, fromBegin - signBegin);
          cout << inBetween << endl;
          if (inBetween.find(skipTagPairBegin) == string::npos)
            break;
          signBegin = fromBegin + 1;
        } while (1);
      }
      // found at fromBegin, replace
      auto orgFromBegin = orgLine.find(from, cutLength + signBegin);
      cout << cutLength << " " << signBegin << " " << orgFromBegin << endl;
      cout << orgLine.substr(cutLength + signBegin) << endl;
      occurences.insert(orgFromBegin);
      inLine = inLine.substr(fromBegin +
                             from.length()); // find next link in the inLine
      cutLength += fromBegin + from.length();
    } while (1);
    for (const auto &pos : occurences) {
      orgLine.replace(pos, from.length(), to);
    }
    outfile << orgLine << endl;
  }
}

void testfixTagPairEnd() {
  CoupledBodyTextWithLink bodyText;
  bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
  bodyText.setFileAndAttachmentNumber("91");
  bodyText.fixTagPairBegin(R"(</strong>)", R"(<b unhidden>)",
                           R"(<strong>)");
  bodyText.fixTagPairEnd(R"(<samp)", R"(</font>)", R"(</samp>)");
  bodyText.fixTagPairEnd(R"(<cite>)", keyEndChars, R"(</cite>)", keyStartChars);
  bodyText.fixTagPairEnd(R"(见左图)", R"(</font>)", R"(</var>)");
}

void fixTagsOfMainBodyText(int minTarget, int maxTarget) {
  bool fixTag = false;
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    if (fixTag) {
      bodyText.fixTagPairBegin(R"(</strong>)", R"(<b unhidden>)",
                               R"(<strong>)");
      bodyText.fixTagPairEnd(R"(<samp)", R"(</font>)", R"(</samp>)");
      bodyText.fixTagPairEnd(R"(见左图)", R"(</font>)", R"(</var>)");
      bodyText.fixTagPairEnd(R"(<cite>)", keyEndChars, R"(</cite>)",
                             keyStartChars);
    }
  }
}

void CoupledContainer::makeSingleLineHeaderAndFooter() {
  string inputHtmlFile = getInputHtmlFile();
  string outputHtmlFile = getoutputHtmlFile();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outHtmlFile(outputHtmlFile);
  string line{""};

  string singleLineHeader;
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto linkBegin = line.find(topIdBeginChars);
    if (linkBegin != string::npos) {
      break;
    } else {
      line = std::regex_replace(line, std::regex("(?:\\t)"), " ");
      line = std::regex_replace(line, std::regex("(?:\\r\\n|\\n|\\r)"), "");
      singleLineHeader += line;
    }
  }
  cout << singleLineHeader << endl;
  outHtmlFile << singleLineHeader << endl; // excluding start line
  outHtmlFile << line << endl;             // output start line
  if (inHtmlFile.eof()) {
    cout << "source " << inputHtmlFile
         << " has no start mark:" << topIdBeginChars << endl;
    return;
  }
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line);   // Saves the line in line.
    outHtmlFile << line << endl; // output line
    auto linkEnd = line.find(bottomIdBeginChars);

    if (linkEnd != string::npos) {
      break;
    }
  }
  if (inHtmlFile.eof()) {
    cout << "source " << inputHtmlFile
         << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }
  string singleLineFooter;
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    line = std::regex_replace(line, std::regex("(?:\\t)"), " ");
    line = std::regex_replace(line, std::regex("(?:\\r\\n|\\n|\\r)"), "");
    singleLineFooter += line;
  }
  cout << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl; // excluding start line
  if (debug >= LOG_INFO)
    cout << "makeSingleLineHeaderAndFooter finished for " << outputHtmlFile
         << endl;
}

void CoupledContainer::fixHeaderAndFooter() {
  string inputHtmlFile = getInputHtmlFile();
  string outputHtmlFile = getoutputHtmlFile();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outHtmlFile(outputHtmlFile);
  string line{""};

  string singleLineHeader;
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto linkBegin = line.find(topIdBeginChars);
    if (linkBegin != string::npos) {
      break;
    } else
      singleLineHeader += line;
  }
  fixHeader(singleLineHeader, m_file);
  cout << singleLineHeader << endl;
  outHtmlFile << singleLineHeader << endl; // excluding start line
  outHtmlFile << line << endl;             // output start line
  if (inHtmlFile.eof()) {
    cout << "source " << inputHtmlFile
         << " has no start mark:" << topIdBeginChars << endl;
    return;
  }
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line);   // Saves the line in line.
    outHtmlFile << line << endl; // output line
    auto linkEnd = line.find(bottomIdBeginChars);

    if (linkEnd != string::npos) {
      break;
    }
  }
  if (inHtmlFile.eof()) {
    cout << "source " << inputHtmlFile
         << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }
  string singleLineFooter;
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    singleLineFooter += line;
  }
  fixFooter(singleLineFooter, m_file);
  cout << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl; // excluding start line
  if (debug >= LOG_INFO)
    cout << "fixHeaderAndFooter finished for " << outputHtmlFile << endl;
}

void fixHeaderAndFooterForJPMHtml(int minTarget, int maxTarget) {
  CoupledContainer container(FILE_TYPE::JPM);
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.fixHeaderAndFooter();
  }
}

void fixHeaderAndFooterForJPMHtmls() {
  int minTarget = 2, maxTarget = 99;
  fixHeaderAndFooterForJPMHtml(minTarget, maxTarget);
  cout << "fixHeaderAndFooter for JPM Htmls finished. " << endl;
}

void CoupledBodyText::fixPersonalView() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  bool unpairFound{false};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    if ((inLine == brTab + "\r") or (inLine == brTab + "\n") or
        (inLine == brTab + "\r\n")) {
      outfile << inLine << endl;
      continue;
    }

    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    // assume only one unpaired personalComment
    auto personalCommentBegin = inLine.find(personalCommentStartChars);
    auto personalCommentEnd = inLine.find(personalCommentEndChars);
    if (unpairFound == true) {
      if (ln.valid()) // remove old line number
      {
        removeOldLineNumber(inLine);
      }
      removeNbspsAndSpaces(inLine);
      inLine = ln.getWholeString() + doubleSpace + displaySpace +
               personalCommentStartChars + personalCommentStartRestChars +
               endOfPersonalCommentBeginTag + inLine;
      if (personalCommentEnd != string::npos) {
        unpairFound = false;
      } else {
        const string endOfLineBr = R"(<br>)";
        if (inLine.find(endOfLineBr) != string::npos)
          inLine.replace(inLine.find(endOfLineBr), endOfLineBr.length(),
                         personalCommentEndChars + endOfLineBr);
      }
      cout << inLine << endl;
    } else if (personalCommentBegin != string::npos and
               personalCommentEnd == string::npos) {
      unpairFound = true;
      const string endOfLineBr = R"(<br>)";
      if (inLine.find(endOfLineBr) != string::npos)
        inLine.replace(inLine.find(endOfLineBr), endOfLineBr.length(),
                       personalCommentEndChars + endOfLineBr);
    }
    outfile << inLine << endl;
  }
}

void fixPersonalViewForJPMHtmls() {
  int minTarget = 1, maxTarget = 100;
  CoupledContainer container(FILE_TYPE::JPM);
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::JPM);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.fixPersonalView();
  }
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  cout << "fixPersonalView for JPM Htmls finished. " << endl;
}

void fixHeaderAndFooterForMainHtml(int minTarget, int maxTarget) {
  CoupledContainer container(FILE_TYPE::MAIN);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
}

void fixHeaderAndFooterForMainHtmls() {
  int minTarget = 1, maxTarget = 80;
  fixHeaderAndFooterForMainHtml(minTarget, maxTarget);
  cout << "fixHeaderAndFooter for Main Htmls finished. " << endl;
}

void fixHeaderAndFooterForAttachmentHtml(int minTarget, int maxTarget,
                                         int minAttachNo, int maxAttachNo) {
  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (not(minAttachNo == 0 and maxAttachNo == 0) and
      minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.makeSingleLineHeaderAndFooter();
    }
  }
}

void fixHeaderAndFooterForAttachmentHtmls() {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 1, maxAttachNo = 50;
  fixHeaderAndFooterForAttachmentHtml(minTarget, maxTarget, minAttachNo,
                                      maxAttachNo);
  cout << "fixHeaderAndFooter for Attachment Htmls finished. " << endl;
}

void fixHeaderAndFooterForOriginalHtml(int minTarget, int maxTarget) {
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
}

void fixHeaderAndFooterForOriginalHtmls() {
  int minTarget = 1, maxTarget = 80;
  fixHeaderAndFooterForOriginalHtml(minTarget, maxTarget);
  cout << "fixHeaderAndFooter for Original Htmls finished. " << endl;
}

void tools(int num) {
  SEPERATE("HLM tool", " started ");
  switch (num) {
  case 1:
    fixHeaderAndFooterForJPMHtmls();
    break;
  case 2:
    fixHeaderAndFooterForMainHtmls();
    break;
  case 3:
    fixHeaderAndFooterForOriginalHtmls();
    break;
  case 4:
    fixHeaderAndFooterForAttachmentHtmls();
    break;
  case 5:
    fixPersonalViewForJPMHtmls();
    break;
  default:
    cout << "invalid tool." << endl;
  }
}
