#include "tools.hpp"

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
                   getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) +
                       previous);
  } while (true);
  do {
    auto toReplaceNextBegin = footer.find(toReplaceNext);
    if (toReplaceNextBegin == string::npos)
      break;
    footer.replace(toReplaceNextBegin, toReplaceNext.length(),
                   getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) + next);
  } while (true);
  const string toReplaceContentTable = "aindex";
  if (footer.find(toReplaceContentTable) == string::npos)
    return;
  const string newContentTable =
      getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) + "index";
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
                   getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) +
                       previous);
  } while (true);
  do {
    auto toReplaceNextBegin = header.find(toReplaceNext);
    if (toReplaceNextBegin == string::npos)
      break;
    header.replace(toReplaceNextBegin, toReplaceNext.length(),
                   getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) + next);
  } while (true);

  const string toReplaceTranslatedTitle =
      R"(第七十八回　老学士与贾政闲征姽婳词	痴公子贾宝玉为晴雯杜撰芙蓉诔)";
  auto toReplaceTranslatedTitleBegin = header.find(toReplaceTranslatedTitle);
  header.replace(toReplaceTranslatedTitleBegin,
                 toReplaceTranslatedTitle.length(),
                 sampTitleBeginChars + originalTitle + sampTitleEndChars);

  const string toReplaceContentTable = "aindex";
  const string newContentTable =
      getHtmlFileNamePrefixFromFileType(FILE_TYPE::JPM) + "index";
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
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  // continue reading till first paragraph header
  string inLine{emptyString};
  while (!infile.eof()) {
    getline(infile, inLine);
    unsigned int before = 0, after = 0;
    // inLine would change in loop below
    auto orgLine = inLine;
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
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  set<int, greater<int>> occurences;
  // continue reading till first paragraph header
  string inLine{emptyString};
  auto cutLength{0};
  while (!infile.eof()) {
    getline(infile, inLine);
    occurences.clear();
    cutLength = 0;
    // inLine would change in loop below
    auto orgLine = inLine;
    do {
      auto signBegin = inLine.find(signOfTagBeforeReplaceTag);
      // no signOfTagBeforeReplaceTag any more, continue with next line
      if (signBegin == string::npos)
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
      // find next link in the inLine
      inLine = inLine.substr(fromBegin + from.length());
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
  bodyText.setFileAndAttachmentNumber(91);
  bodyText.fixTagPairBegin(R"(</strong>)", R"(<b unhidden>)",
                           R"(<strong>)");
  bodyText.fixTagPairEnd(R"(<samp)", R"(</font>)", R"(</samp>)");
  bodyText.fixTagPairEnd(R"(见左图)", R"(</font>)", R"(</var>)");
}

void fixTagsOfMainBodyText(int minTarget, int maxTarget) {
  bool fixTag = false;
  // files need to be fixed
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    if (fixTag) {
      bodyText.fixTagPairBegin(R"(</strong>)", R"(<b unhidden>)",
                               R"(<strong>)");
      bodyText.fixTagPairEnd(R"(<samp)", R"(</font>)", R"(</samp>)");
      bodyText.fixTagPairEnd(R"(见左图)", R"(</font>)", R"(</var>)");
    }
  }
}

void CoupledBodyTextContainer::makeSingleLineHeaderAndFooter() {

  ifstream inHtmlFile(getInputHtmlFilePath());
  if (not fileExist(inHtmlFile, getInputHtmlFilePath()))
    return;
  ofstream outHtmlFile(getoutputHtmlFilepath());
  string line{emptyString};

  string singleLineHeader;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    auto linkBegin = line.find(topIdBeginChars);
    if (linkBegin != string::npos) {
      break;
    } else {
      line = regex_replace(line, regex("(?:\\t)"), displaySpace);
      line = regex_replace(line, regex("(?:\\r\\n|\\n|\\r)"), emptyString);
      singleLineHeader += line;
    }
  }
  FUNCTION_OUTPUT << singleLineHeader << endl;
  // excluding start line
  outHtmlFile << singleLineHeader << endl;
  // output start line
  outHtmlFile << line << endl;
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << getInputHtmlFilePath()
                    << " has no start mark:" << topIdBeginChars << endl;
    return;
  }
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    outHtmlFile << line << endl; // output line
    auto linkEnd = line.find(bottomIdBeginChars);

    if (linkEnd != string::npos) {
      break;
    }
  }
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << getInputHtmlFilePath()
                    << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }
  string singleLineFooter;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    line = regex_replace(line, regex("(?:\\t)"), displaySpace);
    line = regex_replace(line, regex("(?:\\r\\n|\\n|\\r)"), emptyString);
    singleLineFooter += line;
  }
  FUNCTION_OUTPUT << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl;
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "makeSingleLineHeaderAndFooter finished for "
                    << getoutputHtmlFilepath() << endl;
}

void CoupledBodyTextContainer::fixHeaderAndFooter() {
  ifstream inHtmlFile(getInputHtmlFilePath());
  if (not fileExist(inHtmlFile, getInputHtmlFilePath()))
    return;
  ofstream outHtmlFile(getoutputHtmlFilepath());
  string line{emptyString};
  string singleLineHeader;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    auto linkBegin = line.find(topIdBeginChars);
    if (linkBegin != string::npos) {
      break;
    } else
      singleLineHeader += line;
  }
  fixHeader(singleLineHeader, m_file);
  FUNCTION_OUTPUT << singleLineHeader << endl;
  outHtmlFile << singleLineHeader << endl;
  // output start line
  outHtmlFile << line << endl;
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << getInputHtmlFilePath()
                    << " has no start mark:" << topIdBeginChars << endl;
    return;
  }
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    outHtmlFile << line << endl; // output line
    auto linkEnd = line.find(bottomIdBeginChars);

    if (linkEnd != string::npos) {
      break;
    }
  }
  if (inHtmlFile.eof()) {
    FUNCTION_OUTPUT << "source " << getInputHtmlFilePath()
                    << " has no end mark:" << bottomIdBeginChars << endl;
    return;
  }
  string singleLineFooter;
  while (!inHtmlFile.eof()) {
    getline(inHtmlFile, line);
    singleLineFooter += line;
  }
  fixFooter(singleLineFooter, m_file);
  FUNCTION_OUTPUT << singleLineFooter << endl;
  outHtmlFile << singleLineFooter << endl;
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << "fixHeaderAndFooter finished for "
                    << getoutputHtmlFilepath() << endl;
}

void fixHeaderAndFooterForJPMHtml(int minTarget, int maxTarget) {
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::JPM);
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    container.setFileAndAttachmentNumber(file);
    container.fixHeaderAndFooter();
  }
}

void fixHeaderAndFooterForJPMHtmls() {
  int minTarget = 2, maxTarget = 99;
  fixHeaderAndFooterForJPMHtml(minTarget, maxTarget);
  FUNCTION_OUTPUT << "fixHeaderAndFooter for JPM Htmls finished. " << endl;
}

// reformat to smaller paragraphs
void CoupledBodyText::reformatParagraphToSmallerSize(
    const string &sampleBlock) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << utf8length(sampleBlock) << endl;
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << sampleBlock << endl;
  // continue reading
  string inLine;
  while (!infile.eof()) {
    getline(infile, inLine);
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

void CoupledBodyText::fixPersonalView() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  bool unpairFound{false};
  while (!infile.eof()) {
    getline(infile, m_inLine);
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << m_inLine << endl;
    if (isLeadingBr(m_inLine)) {
      outfile << m_inLine << endl;
      continue;
    }

    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(m_inLine);
    // assume only one unpaired personalComment
    auto personalCommentBegin = m_inLine.find(personalCommentStartChars);
    auto personalCommentEnd = m_inLine.find(personalCommentEndChars);
    if (unpairFound == true) {
      // remove old line number if invalid
      if (ln.get().valid()) {
        removeOldLineNumber();
      }
      removeNbspsAndSpaces();
      m_inLine = ln.getWholeString() + doubleSpace + displaySpace +
                 personalCommentStartChars + personalCommentStartRestChars +
                 endOfBeginTag + m_inLine;
      if (personalCommentEnd != string::npos) {
        unpairFound = false;
      } else {
        const string endOfLineBr = R"(<br>)";
        if (m_inLine.find(endOfLineBr) != string::npos)
          m_inLine.replace(m_inLine.find(endOfLineBr), endOfLineBr.length(),
                           personalCommentEndChars + endOfLineBr);
      }
      FUNCTION_OUTPUT << m_inLine << endl;
    } else if (personalCommentBegin != string::npos and
               personalCommentEnd == string::npos) {
      unpairFound = true;
      const string endOfLineBr = R"(<br>)";
      if (m_inLine.find(endOfLineBr) != string::npos)
        m_inLine.replace(m_inLine.find(endOfLineBr), endOfLineBr.length(),
                         personalCommentEndChars + endOfLineBr);
    }
    outfile << m_inLine << endl;
  }
}

void fixPersonalViewForJPMHtmls() {
  int minTarget = JPM_MIN_CHAPTER_NUMBER, maxTarget = JPM_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::JPM);
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  container.clearFixedBodyTexts();
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::JPM);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.fixPersonalView();
  }
  container.loadFixedBodyTexts();
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  FUNCTION_OUTPUT << "fixPersonalView for JPM Htmls finished. " << endl;
}

void fixHeaderAndFooterForMainHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::MAIN);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Main Htmls finished. " << endl;
}

void fixHeaderAndFooterForAttachmentHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  int minAttachNo = MIN_ATTACHMENT_NUMBER, maxAttachNo = MAX_ATTACHMENT_NUMBER;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextContainer container;
    container.setFileType(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    for (const auto &attNo :
         container.getAttachmentFileList(minAttachNo, maxAttachNo)) {
      container.setFileAndAttachmentNumber(file, attNo);
      container.makeSingleLineHeaderAndFooter();
    }
  }
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Attachment Htmls finished. "
                  << endl;
}

void fixHeaderAndFooterForOriginalHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::ORIGINAL);
  for (const auto &file : buildFileSet(minTarget, maxTarget, ORIGINAL)) {
    container.setFileAndAttachmentNumber(file);
    container.makeSingleLineHeaderAndFooter();
  }
  FUNCTION_OUTPUT << "fixHeaderAndFooter for Original Htmls finished. " << endl;
}

void reformatTxtFiles(int minTarget, int maxTarget, const string &example) {}

void reformatTxtFilesForReader() {
  const string example =
      R"(话说安童领着书信，辞了黄通判，径往山东大道而来。打听巡按御史在东昌府住扎，姓曾，双名孝序，【夹批：曾者，争也。序即天叙有典之叙，盖作者为世所厄不能自全其孝，故抑郁愤懑)";

  int minTarget = JPM_MIN_CHAPTER_NUMBER, maxTarget = JPM_MAX_CHAPTER_NUMBER;
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    CoupledBodyText bodyText("pjpm");
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.reformatParagraphToSmallerSize(example);
  }
  FUNCTION_OUTPUT << "reformat files from " << minTarget << " to " << maxTarget
                  << " finished. " << endl;
}

// must be called after called
// CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
void CoupledBodyTextWithLink::removePersonalCommentsOverNumberedFiles() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  while (!infile.eof()) {
    getline(infile, inLine);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << inLine << endl;
    // inLine would change in loop below
    auto orgLine = inLine;
    string start = personalCommentStartChars;
    string end = personalCommentEndChars;
    string to_replace = emptyString;
    // first loop to remove all personal Comments
    auto removePersonalCommentLine = orgLine;
    auto personalCommentBegin = removePersonalCommentLine.find(start);
    while (personalCommentBegin != string::npos) {
      auto personalCommentEnd = removePersonalCommentLine.find(end);
      string personalComment = removePersonalCommentLine.substr(
          personalCommentBegin,
          personalCommentEnd + end.length() - personalCommentBegin);
      to_replace = personalComment;
      auto replaceBegin = orgLine.find(to_replace);
      orgLine.replace(replaceBegin, to_replace.length(), emptyString);
      removePersonalCommentLine =
          removePersonalCommentLine.substr(personalCommentEnd + end.length());
      // find next personalComment in the removePersonalCommentLine
      personalCommentBegin = removePersonalCommentLine.find(start);
    }
    // the second loop to remove all expected attachment link from result
    // orgLine
    auto removeSpecialLinkLine = orgLine;
    auto specialLinkBegin = removeSpecialLinkLine.find(linkStartChars);
    while (specialLinkBegin != string::npos) {
      auto specialLinkEnd = removeSpecialLinkLine.find(linkEndChars);
      string specialLink =
          getIncludedStringBetweenTags(removeSpecialLinkLine, linkStartChars,
                                       linkEndChars, specialLinkBegin);
      LinkFromAttachment m_linkPtr(m_file, specialLink);
      auto num = make_pair(m_linkPtr.getchapterNumer(),
                           m_linkPtr.getattachmentNumber());
      if (m_linkPtr.isTargetToOtherAttachmentHtm() and
          CoupledBodyTextContainer::refAttachmentTable.getAttachmentType(num) ==
              ATTACHMENT_TYPE::PERSONAL) {
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << specialLink << endl;
        to_replace = specialLink;
        auto replaceBegin = orgLine.find(to_replace);
        orgLine.replace(replaceBegin, to_replace.length(), emptyString);
      }
      removeSpecialLinkLine =
          removeSpecialLinkLine.substr(specialLinkEnd + linkEndChars.length());
      // find next specialLink in the removeSpecialLinkLine
      specialLinkBegin = removeSpecialLinkLine.find(linkStartChars);
    }

    outfile << orgLine << endl;
  }
}

void removePersonalViewpoints() {
  int minTarget = 54, maxTarget = 54;
  FUNCTION_OUTPUT << "to be implemented." << endl;
  FUNCTION_OUTPUT << "to remove <u> pairs." << endl;
  FUNCTION_OUTPUT << "and to remove personal attachment link." << endl;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
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
    ConvertNonPrefixedGb2312FilesToUtf8();
    break;
  case 7:
    removePersonalViewpoints();
    break;
  case 8:
    fixTagPairEnd();
    break;
  default:
    FUNCTION_OUTPUT << "invalid tool." << endl;
  }
}
