#include "tools.hpp"

const string strongTitleBeginChars = R"(<strong unhidden>)";
const string strongTitleEndChars = R"(</strong>)";
const string sampTitleBeginChars = R"(<samp unhidden>)";
const string sampTitleEndChars = R"(</samp>)";

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
  FUNCTION_OUTPUT << footer << endl;
}

void fixHeader(string &header, const string &filename) {

  string previous =
      formatIntoZeroPatchedChapterNumber(TurnToInt(filename) - 1, 3);
  string next = formatIntoZeroPatchedChapterNumber(TurnToInt(filename) + 1, 3);
  const string origHeader = header;
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
  header.replace(originalTitleBegin,
                 originalTitleBeginChars.length() + originalTitle.length() +
                     originalTitleEndChars.length(),
                 strongTitleBeginChars + originalTitle + strongTitleEndChars);
  string start = htmlTitleStart;
  string end = htmlTitleEnd;
  auto titleBegin = header.find(start);
  if (titleBegin != string::npos) {
    auto titleEnd = header.find(end, titleBegin);
    if (titleEnd != string::npos)
      header.replace(titleBegin + start.length(),
                     titleEnd - titleBegin - start.length(), originalTitle);
  }

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
  header.replace(toReplaceTranslatedTitleBegin,
                 toReplaceTranslatedTitle.length(),
                 sampTitleBeginChars + originalTitle + sampTitleEndChars);

  const string toReplaceContentTable = "aindex";
  const string newContentTable =
      getHtmlFileNamePrefix(FILE_TYPE::JPM) + "index";
  header.replace(header.find(toReplaceContentTable),
                 toReplaceContentTable.length(), newContentTable);
  FUNCTION_OUTPUT << header << endl;
}

void fixHeaderAndFooter(const string &filename) {
  string header =
      R"(<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf8"><style type="text/css">a {text-decoration: none}</style><title>第七十八回　老学士闲征姽婳词	痴公子杜撰芙蓉诔</title></head><body text="white" bgcolor="#004040" topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" rightmargin="0" link="#fff000" vlink="#fff000"><center>		<table border="0" cellspacing="1" width="80%"   style="font-size: 12pt; font-family: 宋体; line-height:150%"><tr><td style="font-size: 10.5pt"><hr color="#F0BEC0"><hr color="#F0BEC0"><a unhidden href="aindex.htm">>回目录</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a077.htm#bottom">上回结尾</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a077.htm">上回开始</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="a079.htm">->下一回</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="#top">本回开始</a></td>	</tr><tr><td height="50" style="font-size: 15pt; font-family: 黑体; letter-spacing:1" align="center"><br><b unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</b><br>				<br>第七十八回　老学士与贾政闲征姽婳词	痴公子贾宝玉为晴雯杜撰芙蓉诔<br></td></tr><tr><td align="center"><hr color="#F0BEC0"></td></tr><tr><td>)";
  string headerCompareTo =
      R"(<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf8"><style type="text/css">a {text-decoration: none}</style><title>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</title></head><body text="white" bgcolor="#004040" topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" rightmargin="0" link="#fff000" vlink="#fff000"><center>		<table border="0" cellspacing="1" width="80%"   style="font-size: 12pt; font-family: 宋体; line-height:150%"><tr><td style="font-size: 10.5pt"><hr color="#F0BEC0"><hr color="#F0BEC0"><a unhidden href="dindex.htm">>回目录</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d083.htm#bottom">上回结尾</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d083.htm">上回开始</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="d085.htm">->下一回</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden href="#top">本回开始</a></td>	</tr><tr><td height="50" style="font-size: 15pt; font-family: 黑体; letter-spacing:1" align="center"><br><strong unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</strong><br>				<br><samp unhidden>第八十四回　　吴月娘大闹碧霞宫　曾静师化缘雪涧洞</samp><br></td></tr><tr><td align="center"><hr color="#F0BEC0"></td></tr><tr><td>)";
  fixHeader(header, filename);
  FUNCTION_OUTPUT << headerCompareTo << endl;
  FUNCTION_OUTPUT << (header == headerCompareTo) << endl;
  string footer =
      R"(</td></tr><tr><td><hr color="#F0BEC0"></td></tr><tr><td align="center"><a unhidden href="a077.htm">上一回</a>&nbsp;<a unhidden href="aindex.htm">回目录</a>&nbsp;<a unhidden href="a079.htm">下一回</a>&nbsp;<hr color="#F0BEC0">	<hr color="#F0BEC0"></td></tr></table></center></body></html>)";
  string footerCompareTo =
      R"(</td></tr><tr><td><hr color="#F0BEC0"></td></tr><tr><td align="center"><a unhidden href="d083.htm">上一回</a>&nbsp;<a unhidden href="dindex.htm">回目录</a>&nbsp;<a unhidden href="d085.htm">下一回</a>&nbsp;<hr color="#F0BEC0">	<hr color="#F0BEC0"></td></tr></table></center></body></html>)";
  fixFooter(footer, filename);
  FUNCTION_OUTPUT << footerCompareTo << endl;
  FUNCTION_OUTPUT << (footer == footerCompareTo) << endl;
}

void CoupledBodyText::fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                                      const string &from, const string &to) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    FUNCTION_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
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
    FUNCTION_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
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
            FUNCTION_OUTPUT
                << from << "is not found after: " << signOfTagBeforeReplaceTag
                << endl;
            return;
          }
          auto inBetween = inLine.substr(signBegin, fromBegin - signBegin);
          FUNCTION_OUTPUT << inBetween << endl;
          if (inBetween.find(skipTagPairBegin) == string::npos)
            break;
          signBegin = fromBegin + 1;
        } while (1);
      }
      // found at fromBegin, replace
      auto orgFromBegin = orgLine.find(from, cutLength + signBegin);
      FUNCTION_OUTPUT << cutLength << " " << signBegin << " " << orgFromBegin
                      << endl;
      FUNCTION_OUTPUT << orgLine.substr(cutLength + signBegin) << endl;
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

void fixTagPairEnd() {
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
    FUNCTION_OUTPUT << "file doesn't exist:" << inputHtmlFile << endl;
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
  FUNCTION_OUTPUT << singleLineHeader << endl;
  outHtmlFile << singleLineHeader << endl; // excluding start line
  outHtmlFile << line << endl;             // output start line
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << inputHtmlFile
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
    FUNCTION_OUTPUT << "source " << inputHtmlFile
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
  FUNCTION_OUTPUT << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl; // excluding start line
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "makeSingleLineHeaderAndFooter finished for "
                    << outputHtmlFile << endl;
}

void CoupledContainer::fetchOriginalAndTranslatedTitles() {
  string inputHtmlFile = getInputHtmlFile();
  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    FUNCTION_OUTPUT << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  string line{""};
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto linkBegin = line.find(topIdBeginChars);
    if (linkBegin != string::npos) {
      break;
    }
    auto strongTitleBegin = line.find(strongTitleBeginChars);
    if (strongTitleBegin != string::npos) {
      auto strongTitleEnd = line.find(strongTitleEndChars, strongTitleBegin);
      m_originalTitle = line.substr(
          strongTitleBegin + strongTitleBeginChars.length(),
          strongTitleEnd - strongTitleBegin - strongTitleBeginChars.length());
    }
    auto sampTitleBegin = line.find(sampTitleBeginChars);
    if (sampTitleBegin != string::npos) {
      auto sampTitleEnd = line.find(sampTitleEndChars, sampTitleBegin);
      m_translatedTitle = line.substr(
          sampTitleBegin + sampTitleBeginChars.length(),
          sampTitleEnd - sampTitleBegin - sampTitleBeginChars.length());
    }
  }
}

void CoupledContainer::fixHeaderAndFooter() {
  string inputHtmlFile = getInputHtmlFile();
  string outputHtmlFile = getoutputHtmlFile();

  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    FUNCTION_OUTPUT << "file doesn't exist:" << inputHtmlFile << endl;
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
  FUNCTION_OUTPUT << singleLineHeader << endl;
  outHtmlFile << singleLineHeader << endl; // excluding start line
  outHtmlFile << line << endl;             // output start line
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << inputHtmlFile
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
    FUNCTION_OUTPUT << "source " << inputHtmlFile
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
  FUNCTION_OUTPUT << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl; // excluding start line
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "fixHeaderAndFooter finished for " << outputHtmlFile
                    << endl;
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
  FUNCTION_OUTPUT << "fixHeaderAndFooter for JPM Htmls finished. " << endl;
}

void CoupledBodyText::fixPersonalView() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    FUNCTION_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  bool unpairFound{false};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << inLine << endl;
    if (isLeadingBr(inLine)) {
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
      FUNCTION_OUTPUT << inLine << endl;
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
  FUNCTION_OUTPUT << "fixPersonalView for JPM Htmls finished. " << endl;
}

void fixHeaderAndFooterForMainHtmls() {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::MAIN);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Main Htmls finished. " << endl;
}

void fixHeaderAndFooterForAttachmentHtmls() {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 1, maxAttachNo = 50;
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
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Attachment Htmls finished. "
                  << endl;
}

void fixHeaderAndFooterForOriginalHtmls() {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Original Htmls finished. " << endl;
}

std::vector<int> createParaList(int first, int incremental, int max) {
  std::vector<int> result;
  result.push_back(first);
  int i = first;
  while ((i += incremental) < max) {
    result.push_back(i);
  }
  return result;
}

void generateContentTableForMainHtmls() {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::MAIN);
  TableContainer outputContainer("aindex");
  outputContainer.setInputFileName(TABLE_CONTAINER_FILENAME_SMALLER_FONT);
  auto paraList = createParaList(6, 10, 70);
  paraList.push_back(72);
  std::sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  outputContainer.addExistingFrontParagraphs();
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link =
        fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::UNHIDDEN, "", "",
                                brTab + container.getOriginalTitle() + brTab +
                                    container.getTranslatedTitle(),
                                "");
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(R"(脂砚斋重评石头记 现代白话文版)",
                                    R"(脂砚斋重评石头记 现代白话文版 目录)");
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Main Htmls finished. " << endl;
}

void generateContentTableForOriginalHtmls() {
  int minTarget = 1, maxTarget = 80;
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  TableContainer outputContainer("cindex");
  auto paraList = createParaList(18, 22, 70);
  std::sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link =
        fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file, "", "",
                                    "", container.getOriginalTitle());
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(R"(脂砚斋重评石头记)",
                                    R"(脂砚斋重评石头记 目录)");
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Original Htmls finished. "
                  << endl;
}

void generateContentTableForJPMHtmls() {
  int minTarget = 1, maxTarget = 100;
  CoupledContainer container(FILE_TYPE::JPM);
  TableContainer outputContainer("dindex");
  auto paraList = createParaList(18, 22, 90);
  std::sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget, 3)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link = fixLinkFromJPMTemplate(jpmDirForLinkFromMain, file, "", "", "",
                                       container.getOriginalTitle());
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(R"(张竹坡批注金瓶梅)",
                                    R"(张竹坡批注金瓶梅 目录)");
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for JPM Htmls finished. " << endl;
}

void generateContentTableForReferenceAttachments() {}

void generateContentTableForPersonalAttachments() {
  LinkFromMain::loadReferenceAttachmentList();
  ListContainer container("bindex2");
  auto table = Link::refAttachmentTable;
  for (const auto &attachment : table) {
    auto attachmentName = attachment.first;
    auto entry = attachment.second.second;
    ATTACHMENT_TYPE attachmentType = GetTupleElement(entry, 2);

    if (attachmentType == ATTACHMENT_TYPE::PERSONAL) {
      string name = citationChapterNo + TurnToString(attachmentName.first) +
                    citationChapter + R"(附件)" +
                    TurnToString(attachmentName.second) + R"(: )";
      container.appendParagraphInBodyText(fixLinkFromAttachmentTemplate(
          attachmentDirForLinkFromMain,
          formatIntoZeroPatchedChapterNumber(attachmentName.first, 2),
          TurnToString(attachmentName.second),
          name + GetTupleElement(entry, 1)));
    }
  }
  container.assembleBackToHTM("personal attachments", "personal attachments");
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}

void reformatTxtFiles(int minTarget, int maxTarget, const string &example) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string filename = file;
    if (filename.length() == 2) {
      filename = "0" + filename;
    }
    CoupledBodyText bodyText("pjpm");
    bodyText.setFileAndAttachmentNumber(filename);
    bodyText.reformatParagraphToSmallerSize(example);
  }
}

void reformatTxtFilesForReader() {
  const string example =
      R"(话说安童领着书信，辞了黄通判，径往山东大道而来。打听巡按御史在东昌府住扎，姓曾，双名孝序，【夹批：曾者，争也。序即天叙有典之叙，盖作者为世所厄不能自全其孝，故抑郁愤懑)";

  int minTarget = 1, maxTarget = 100;
  reformatTxtFiles(minTarget, maxTarget, example);
  FUNCTION_OUTPUT << "reformat files from " << minTarget << " to " << maxTarget
                  << " finished. " << endl;
}

void renderingBodyText(const string &fileType, bool hideParaHeader = false) {
  const string sampleBlock = R"()";
  const string sampleFirstLine = R"()";
  const string sampleWholeLine = R"()";
  int minTarget = 49, maxTarget = 49;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(getFileTypeFromString(fileType));
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.render();
  }
}

void removePersonalViewpoints() {
  int minTarget = 54, maxTarget = 54;
  FUNCTION_OUTPUT << "to be implemented." << endl;
  FUNCTION_OUTPUT << "to remove <u> pairs." << endl;
  FUNCTION_OUTPUT << "and to remove personal attachment link." << endl;
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.removePersonalCommentsOverNumberedFiles();
  }
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
  case 6:
    generateContentTableForMainHtmls();
    break;
  case 7:
    generateContentTableForOriginalHtmls();
    break;
  case 8:
    generateContentTableForJPMHtmls();
    break;
  case 9:
    generateContentTableForReferenceAttachments();
    break;
  case 10:
    generateContentTableForPersonalAttachments();
    break;
  case 11:
    ConvertNonPrefixedGb2312FilesToUtf8();
    break;
  case 12:
    removePersonalViewpoints();
    break;
  case 13:
    fixTagPairEnd();
    break;
  case 14:
    findFirstInFiles();
    break;
  case 15:
    constructSubStory();
    break;
  case 16:
    renderingBodyText("main");
    break;
  default:
    FUNCTION_OUTPUT << "invalid tool." << endl;
  }
}
