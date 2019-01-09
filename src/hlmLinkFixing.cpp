#include "hlmLinkFixing.hpp"
/**
 * check lineNumber from refAttachmentList about a link to attachment
 * and put that lineNumber in that attachment file header
 * @param filename
 */
void fixReturnLinkForAttachmentFile(const string &referFile,
                                    const string &inputHtmlFile,
                                    const string &outputFile) {
  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "HTM file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ofstream outfile(outputFile);
  string line{""};
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    auto textBegin = line.find(returnLinkFromAttachmentHeader);
    if (textBegin == string::npos) {
      outfile << line << endl;
      continue;
    } else {
      auto orgLine = line; // inLine would change in loop below
      auto start = linkStartChars;
      string link{""};
      while (true) {
        auto linkBegin = line.find(start);
        if (linkBegin == string::npos) // no link any more, continue with next
                                       // line
          break;
        auto linkEnd = line.find(linkEndChars, linkBegin);
        link = line.substr(linkBegin, linkEnd + 4 - linkBegin);
        LinkFromAttachment lfm(referFile,
                               link); // get only type and annotation
        if (lfm.getAnnotation() == returnLinkFromAttachmentHeader)
          break;
        else
          line = line.substr(linkEnd + 4); // find next link in the line
      }
      if (not link.empty()) {
        LinkFromAttachment lfm(referFile, link);
        auto num = getAttachmentNumber(
            getFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile);
        // special hack to make sure using a0... as return file name
        lfm.setTypeThruFileNamePrefix("main"); // must return to main html
        lfm.fixReferFile(num.first);
        lfm.fixReferPara(LinkFromMain::getFromLineOfAttachment(num));
        if (lfm.needUpdate()) // replace old value
        {
          if (debug >= LOG_INFO)
            cout << lfm.asString() << endl;
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      outfile << orgLine << endl;
    }
  }
}

/**
 * fix links of certain type in referFile which refer to one of file in files
 * @param referFile
 * @param files
 */
void fixMainLinksOverNumberedFiles(const string &referFile, fileSet files,
                                   int minPara = 0, int maxPara = 0,
                                   int minLine = 0, int maxLine = 0) {
  string inputFile = BODY_TEXT_OUTPUT + getBodyTextFilePrefix(FILE_TYPE::MAIN) +
                     referFile + BODY_TEXT_SUFFIX;
  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile = BODY_TEXT_FIX + getBodyTextFilePrefix(FILE_TYPE::MAIN) +
                      referFile + BODY_TEXT_SUFFIX;
  ofstream outfile(outputFile);
  string inLine{""};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto orgLine = inLine;   // inLine would change in loop below
    LineNumber ln;
    ln.loadFromContainedLine(orgLine);
    if (ln.isParagraphHeader() or not ln.valid() or
        not ln.isWithinLineRange(minPara, maxPara, minLine, maxLine)) {
      outfile << orgLine << endl;
      continue; // not fix headers or non-numbered lines
    }
    inLine = inLine.substr(
        ln.generateLinePrefix().length()); // skip line number link
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    auto start = linkStartChars;
    string targetFile{""};
    do {
      auto linkBegin = inLine.find(start);
      if (linkBegin == string::npos) // no link any more, continue with next
                                     // line
        break;
      auto linkEnd = inLine.find(linkEndChars, linkBegin);
      auto link = inLine.substr(linkBegin, linkEnd + 4 - linkBegin);
      LinkFromMain lfm(referFile,
                       link);      // get only type and annotation
      lfm.readReferFileName(link); // second step of construction, this is
                                   // needed to check isTargetToSelfHtm
      if (lfm.isTargetToOtherAttachmentHtm()) {
        lfm.fixFromString(link); // third step of construction
        lfm.setSourcePara(ln);
        lfm.doStatistics();
      }
      if (lfm.isTargetToSelfHtm()) {
        lfm.setSourcePara(ln);
        lfm.fixFromString(link); // third step of construction
        if (lfm.needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      if (lfm.isTargetToOtherMainHtm()) {
        targetFile = lfm.getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm.fixFromString(link); // third step of construction
          lfm.setSourcePara(ln);
          string next = originalLinkStartChars + linkStartChars;
          bool needAddOrginalLink = true;
          // still have above "next" and </a>
          if (inLine.length() > (link.length() + next.length() + 4)) {
            if (inLine.substr(linkEnd + 4, next.length()) == next) {
              // skip </a> and first parenthesis of next
              auto followingLink = inLine.substr(
                  linkEnd + next.length() + 2); // find next link in the inLine
              LinkFromMain following(referFile, followingLink);
              if (following.isTargetToOriginalHtm()) {
                needAddOrginalLink = false;
              }
            }
          }
          if (needAddOrginalLink)
            lfm.generateLinkToOrigin();
          lfm.doStatistics();
          if (lfm.needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), lfm.asString());
          }
        }
      }
      if (lfm.isTargetToOriginalHtm()) {
        targetFile = lfm.getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm.fixFromString(link); // third step of construction
          if (lfm.needUpdate())    // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            if (debug >= LOG_INFO)
              SEPERATE("isTargetToOriginalHtm", orgLine + "\n" + link);
            orgLine.replace(orglinkBegin, link.length(), lfm.asString());
          }
        }
      }
      inLine = inLine.substr(linkEnd + 4); // find next link in the inLine
    } while (1);
    outfile << orgLine << endl;
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 * @param minReference
 * @param maxReference
 */
void fixMainLinks(int minTarget, int maxTarget, int minReference,
                  int maxReference) {
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    fixMainLinksOverNumberedFiles(
        file, buildFileSet(minReference, maxReference)); // reference files
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 */
void fixReturnLinkForAttachments(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    auto targetAttachments =
        getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      string inputHtmlFile =
          HTML_SRC_ATTACHMENT + getFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      string outputFile =
          HTML_OUTPUT_ATTACHMENT + getFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      fixReturnLinkForAttachmentFile(file + attachmentFileMiddleChar +
                                         TurnToString(attNo),
                                     inputHtmlFile, outputFile);
    }
  }
  if (debug >= LOG_INFO)
    cout << "fix Return Link finished. " << endl;
}

/**
 * before this function to work, numbering all main, original files
 * no requirement for numbering attachment files.
 * however, attachment files must be put under HTML_SRC_ATTACHMENT
 * for getting titles of them.
 * and fixReturnLinkForAttachments would fix these attachment files
 * and save them into HTML_OUTPUT_ATTACHMENT just like assembleAttachments
 * @param minTarget
 * @param maxTarget
 * @param minReference
 * @param maxReference
 */
void fixMainHtml(int minTarget, int maxTarget, int minReference,
                 int maxReference) {
  CoupledContainer container(FILE_TYPE::MAIN);
  backupAndOverwriteSrcForHTML(); // update html src
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.dissembleFromHTM(file);
  }
  fixMainLinks(minTarget, maxTarget, minReference, maxReference);
  loadBodyTexts(BODY_TEXT_FIX, BODY_TEXT_OUTPUT);
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
  fixReturnLinkForAttachments(minTarget, maxTarget);
}

void fixLinksFromMainHtmls() {
  int minTarget = 1, maxTarget = 80;
  int minReference = 1, maxReference = 80;
  fixMainHtml(minTarget, maxTarget, minReference, maxReference);
}

fileSet keyMissingChapters;
fileSet newAttachmentList;

/**
 *
 */
void clearReport() {
  keyMissingChapters.clear();
  newAttachmentList.clear();
}

/**
 *
 */
void displayMainFilesOfMissingKey() {
  if (keyMissingChapters.empty())
    return;
  cout << "files which has missing key links:" << endl;
  for (const auto &file : keyMissingChapters) {
    cout << getFileNamePrefix(FILE_TYPE::MAIN) + file + ".htm" << endl;
  }
}

/**
 *
 */
void displayNewlyAddedAttachments() {
  if (newAttachmentList.empty())
    return;
  cout << "Newly Added Attachments:" << endl;
  for (const auto &file : newAttachmentList) {
    cout << file + ".htm" << endl;
  }
}

void fixLinksFromMain() {
  clearReport();
  LinkFromMain::resetStatisticsAndLoadReferenceAttachmentList();
  fixLinksFromMainHtmls();
  LinkFromMain::outPutStatisticsToFiles();
  displayMainFilesOfMissingKey();
  displayNewlyAddedAttachments();
  cout << "fixLinksFromMain finished. " << endl;
}

/**
 *
 * @param referFile
 * @param files
 * @param attachNo
 */
void fixAttachmentLinksOverNumberedFiles(const string &referFile, fileSet files,
                                         int attachNo) {
  string inputFile = BODY_TEXT_OUTPUT +
                     getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) + referFile +
                     attachmentFileMiddleChar + TurnToString(attachNo) +
                     BODY_TEXT_SUFFIX;

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile =
      BODY_TEXT_FIX + getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) + referFile +
      attachmentFileMiddleChar + TurnToString(attachNo) + BODY_TEXT_SUFFIX;
  ofstream outfile(outputFile);
  string inLine{""};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto orgLine = inLine;   // inLine would change in loop below
    LineNumber ln;
    ln.loadFromContainedLine(orgLine);
    if (ln.isParagraphHeader() or not ln.valid()) {
      outfile << orgLine << endl;
      continue; // not fix headers or non-numbered lines
    }
    inLine = inLine.substr(
        ln.generateLinePrefix().length()); // skip line number link
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    auto start = linkStartChars;
    string targetFile{""};
    do {
      auto linkBegin = inLine.find(start);
      if (linkBegin == string::npos) // no link any more, continue with next
                                     // line
        break;
      auto linkEnd = inLine.find(linkEndChars, linkBegin);
      auto link = inLine.substr(linkBegin, linkEnd + 4 - linkBegin);
      // get only type and annotation
      LinkFromAttachment lfm(
          referFile + attachmentFileMiddleChar + TurnToString(attachNo), link);
      lfm.readReferFileName(link); // second step of construction, this is
                                   // needed to check isTargetToSelfHtm
      if (lfm.isTargetToOtherAttachmentHtm()) {
        lfm.fixFromString(link); // third step of construction
        lfm.setSourcePara(ln);
        lfm.doStatistics();
      }
      if (lfm.isTargetToSelfHtm()) {
        lfm.setSourcePara(ln);
        lfm.fixFromString(link); // third step of construction
        if (lfm.needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      if (lfm.isTargetToOtherMainHtm()) {
        targetFile = lfm.getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm.fixFromString(link); // third step of construction
          lfm.setSourcePara(ln);
          string next = originalLinkStartChars + linkStartChars;
          bool needAddOrginalLink = true;
          // still have above "next" and </a>
          if (inLine.length() > (link.length() + next.length() + 4)) {
            if (inLine.substr(linkEnd + 4, next.length()) == next) {
              // skip </a> and first parenthesis of next
              auto followingLink = inLine.substr(
                  linkEnd + next.length() + 2); // find next link in the inLine
              LinkFromAttachment following(referFile, followingLink);
              if (following.isTargetToOriginalHtm()) {
                needAddOrginalLink = false;
              }
            }
          }
          if (needAddOrginalLink)
            lfm.generateLinkToOrigin();
          lfm.doStatistics();
          if (lfm.needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), lfm.asString());
          }
        }
      }
      if (lfm.isTargetToOriginalHtm()) {
        targetFile = lfm.getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm.fixFromString(link); // third step of construction
          if (lfm.needUpdate())    // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), lfm.asString());
          }
        }
      }
      inLine = inLine.substr(linkEnd + 4); // find next link in the inLine
    } while (1);
    outfile << orgLine << endl;
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 * @param minReference
 * @param maxReference
 * @param minAttachNo
 * @param maxAttachNo
 */
void fixLinksToMainForAttachments(int minTarget, int maxTarget,
                                  int minReference, int maxReference,
                                  int minAttachNo, int maxAttachNo) {

  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (minAttachNo != 0 and maxAttachNo != 0 and minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments)
      fixAttachmentLinksOverNumberedFiles(
          file, buildFileSet(minReference, maxReference), attNo);
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 * @param minReference
 * @param maxReference
 * @param minAttachNo
 * @param maxAttachNo
 */
void fixAttachments(int minTarget, int maxTarget, int minReference,
                    int maxReference, int minAttachNo, int maxAttachNo) {
  backupAndOverwriteSrcForHTML(); // update html src
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  fixLinksToMainForAttachments(minTarget, maxTarget, minReference, maxReference,
                               minAttachNo, maxAttachNo);
  loadBodyTexts(BODY_TEXT_FIX, BODY_TEXT_OUTPUT);
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

void fixLinksFromAttachmentHtmls() {
  int minTarget = 3, maxTarget = 3;
  int minReference = 1, maxReference = 80;
  int minAttachNo = 1, maxAttachNo = 1;
  fixAttachments(minTarget, maxTarget, minReference, maxReference, minAttachNo,
                 maxAttachNo);
}

void fixLinksFromAllAttachmentHtmls() {
  int minTarget = 3, maxTarget = 3;
  int minReference = 1, maxReference = 80;
  int minAttachNo = 0, maxAttachNo = 0;
  // fix all attachments
  fixAttachments(minTarget, maxTarget, minReference, maxReference, minAttachNo,
                 maxAttachNo);
}

void fixLinksFromAttachment() {
  LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();
  fixLinksFromAttachmentHtmls();
  LinkFromAttachment::outPutStatisticsToFiles();
  cout << "fixLinksFromAttachment finished. " << endl;
}

void findFirstInNoAttachmentFiles(const string key, FILE_TYPE targetFileType,
                                  int minTarget, int maxTarget,
                                  const string &outputFilename) {

  using LinksList = map<string, vector<Link::LinkDetails>>;
  LinksList resultLinkList;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string referPara{""};
    bool ignoreChange;
    // search key in referred file
    string referFile = BODY_TEXT_OUTPUT +
                       getBodyTextFilePrefix(targetFileType) + file +
                       BODY_TEXT_SUFFIX;
    lineNumberSet ignorelineNumberSet;
    string newKey =
        findKeyInFile(key, referFile, ignorelineNumberSet, referPara,
                      ignoreChange); // continue using same key
    if (newKey.find("KeyNotFound") == string::npos) {
      if (targetFileType == FILE_TYPE::ORIGINAL) {
        // output HTML file is always under HTML_OUTPUT_MAIN
        Link::LinkDetails detail{
            key, file, referPara,
            fixLinkFromOriginalTemplate(R"(original\)", file, key, referPara)};
        resultLinkList[file].push_back(detail);
      }
      if (targetFileType == FILE_TYPE::MAIN) {
        LineNumber ln(referPara);
        string expectedSection =
            R"(第)" + TurnToString(TurnToInt(file)) + R"(章)" +
            TurnToString(ln.getParaNumber()) + R"(.)" +
            TurnToString(ln.getlineNumber()) + R"(节:)";
        Link::LinkDetails detail{
            key, file, referPara,
            fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::DIRECT, key,
                                    expectedSection, key, referPara)};
        resultLinkList[file].push_back(detail);
      }
    }
  }
  // use default no. 1 container to output
  clearLinksInContainerBodyText(1);
  int total = 0;
  for (const auto &chapter : resultLinkList) {
    appendLinkInContainerBodyText("found in " +
                                      getBodyTextFilePrefix(targetFileType) +
                                      chapter.first + HTML_SUFFIX + " :",
                                  1);
    auto list = chapter.second;
    for (const auto &detail : list) {
      total++;
      appendLinkInContainerBodyText(detail.link, 1);
    }
  }
  appendLinkInContainerBodyText(TurnToString(total) + " links are found.", 1);
  ListContainer container(outputFilename);
  container.assembleBackToHTM("search  results",
                              "searchInFiles for key: " + key);
}

void findFirstInNoAttachmentFiles(const string &key, const string &fileType,
                                  const string &outputFilename) {
  ListContainer container;
  int minTarget = 1, maxTarget = 80;
  findFirstInNoAttachmentFiles(key, getFileTypeFromString(fileType), minTarget,
                               maxTarget, outputFilename);
  cout << "result is in file "
       << HTML_OUTPUT_MAIN + outputFilename + HTML_SUFFIX << endl;
}

static const string personalCommentStart = R"(（<u unhidden)";
static const string personalCommentEnd = R"(</u>）)";

void removePersonalCommentsOverNumberedFiles(string referFile,
                                             FILE_TYPE fileType,
                                             int attachNo = 1) {
  string attachmentPart{""};
  if (fileType ==
      FILE_TYPE::ATTACHMENT) // type is only used here in this function
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);
  string inputFile = BODY_TEXT_OUTPUT + getBodyTextFilePrefix(fileType) +
                     referFile + attachmentPart + BODY_TEXT_SUFFIX;

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile = BODY_TEXT_FIX + getBodyTextFilePrefix(fileType) +
                      referFile + attachmentPart + BODY_TEXT_SUFFIX;
  ofstream outfile(outputFile);
  string inLine{"not found"};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    auto orgLine = inLine; // inLine would change in loop below
    string start = personalCommentStart;
    string end = personalCommentEnd;
    string to_replace = "";
    // first loop to remove all personal Comments
    auto removePersonalCommentLine = orgLine;
    auto personalCommentBegin = removePersonalCommentLine.find(start);
    while (personalCommentBegin != string::npos) {
      auto personalCommentEnd = removePersonalCommentLine.find(end);
      string personalComment = removePersonalCommentLine.substr(
          personalCommentBegin,
          personalCommentEnd + end.length() - personalCommentBegin);
      //      cout << personalComment << endl;
      to_replace = personalComment;
      auto replaceBegin = orgLine.find(to_replace);
      orgLine.replace(replaceBegin, to_replace.length(), "");
      removePersonalCommentLine =
          removePersonalCommentLine.substr(personalCommentEnd + end.length());
      // find next personalComment in the removePersonalCommentLine
      personalCommentBegin = removePersonalCommentLine.find(start);
    }
    // the second loop to remove all expected attachment link from result
    // orgLine
    auto removeSpecialLinkLine = orgLine;
    string linkStart = linkStartChars;
    string linkEnd = linkEndChars;
    auto specialLinkBegin = removeSpecialLinkLine.find(linkStart);
    while (specialLinkBegin != string::npos) {
      auto specialLinkEnd = removeSpecialLinkLine.find(linkEnd);
      string specialLink = removeSpecialLinkLine.substr(
          specialLinkBegin,
          specialLinkEnd + linkEnd.length() - specialLinkBegin);
      LinkFromAttachment lfm(referFile, specialLink);
      auto num = make_pair(lfm.getchapterNumer(), lfm.getattachmentNumber());
      if (lfm.isTargetToOtherAttachmentHtm() and
          LinkFromMain::getAttachmentType(num) == ATTACHMENT_TYPE::PERSONAL) {
        cout << specialLink << endl;
        to_replace = specialLink;
        auto replaceBegin = orgLine.find(to_replace);
        orgLine.replace(replaceBegin, to_replace.length(), "");
      }
      removeSpecialLinkLine =
          removeSpecialLinkLine.substr(specialLinkEnd + linkEnd.length());
      // find next specialLink in the removeSpecialLinkLine
      specialLinkBegin = removeSpecialLinkLine.find(linkStart);
    }

    outfile << orgLine << endl;
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 */
void removePersonalViewpoints(int minTarget, int maxTarget,
                              FILE_TYPE fileType) {
  cout << "to be implemented." << endl;
  cout << "to remove <u> pairs." << endl;
  cout << "and to remove personal attachment link." << endl;
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    removePersonalCommentsOverNumberedFiles(file, fileType); // reference files
  }
}

/**
 *
 * @param fs
 */
void dispalyLinkRange(LinkRange fs) {
  for (const auto &link : fs) {
    cout << link.first << "   " << link.second << endl;
  }
}

/**
 * give a set of links, create a sub-story by following these links
 * @param filename
 */
void reConstructStory(string indexFilename) {
  LinkRange cs;  // startChapter, endChapter
  LinkRange ls;  // startLine, endLine
  LinkRange pls; // startPara startLine, endPara endLine
  ifstream infile(indexFilename);
  if (!infile) // doesn't exist
  {
    cout << "file doesn't exist:" << indexFilename << endl;
    return;
  }
  string title{""};
  if (!infile.eof())
    getline(infile, title);
  else
    return; // empty file
  cout << title << endl;

  while (!infile.eof()) // To get you all the lines.
  {
    string startChapter, startPara, startLine;
    getline(infile, startChapter, '#');
    getline(infile, startPara, '.');
    getline(infile, startLine, ' ');
    string endChapter, endPara, endLine;
    getline(infile, endChapter, '#');
    getline(infile, endPara, '.');
    getline(infile, endLine, '\n');
    //		  cout <<
    // startChapter<<startPara<<startLine<<endChapter<<endPara<<endLine<<
    // endl;
    string startLink =
        TurnToString(TurnToInt(startPara) + START_PARA_NUMBER - 1) + startPara +
        "0" + startLine;
    string endLink = TurnToString(TurnToInt(endPara) + START_PARA_NUMBER - 1) +
                     endPara + "0" + endLine;
    //		  cout << startLink<<endLink<< endl;
    cs.push_back(make_pair(startChapter, endChapter));
    ls.push_back(make_pair(startLink, endLink));
    pls.push_back(
        make_pair(startPara + " " + startLine, endPara + " " + endLine));
  }
  dispalyLinkRange(cs);
  dispalyLinkRange(ls);
  dispalyLinkRange(pls);
}
