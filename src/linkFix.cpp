#include "linkFix.hpp"

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
    cout << getHtmlFileNamePrefix(FILE_TYPE::MAIN) + file + ".htm" << endl;
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
            getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + referFile);
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
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.fixLinksFromFile(buildFileSet(minReference, maxReference));
    bool fixTag = false;
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
          HTML_SRC_ATTACHMENT + getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) +
          file + attachmentFileMiddleChar + TurnToString(attNo) + HTML_SUFFIX;
      string outputFile = HTML_OUTPUT_ATTACHMENT +
                          getHtmlFileNamePrefix(FILE_TYPE::ATTACHMENT) + file +
                          attachmentFileMiddleChar + TurnToString(attNo) +
                          HTML_SUFFIX;
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
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.dissembleFromHTM();
  }
  fixMainLinks(minTarget, maxTarget, minReference, maxReference);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.assembleBackToHTM();
  }
  fixReturnLinkForAttachments(minTarget, maxTarget);
}

void fixLinksFromMainHtmls() {
  int minTarget = 1, maxTarget = 80;
  int minReference = 1, maxReference = 80;
  fixMainHtml(minTarget, maxTarget, minReference, maxReference);
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

void generateContentIndexTableForAttachments() {
  LinkFromMain::loadReferenceAttachmentList();
  ListContainer container("bindex1");
  container.initBodyTextFile();
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
  cout << "result is in file " << container.getOutputFilePath() << endl;
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
    for (const auto &attNo : targetAttachments) {
      CoupledBodyTextWithLink bodyText;
      bodyText.setFilePrefixFromFileType(FILE_TYPE::ATTACHMENT);
      bodyText.setFileAndAttachmentNumber(file, attNo);
      bodyText.fixLinksFromFile(buildFileSet(minReference, maxReference));
    }
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
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  CoupledContainer::backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  fixLinksToMainForAttachments(minTarget, maxTarget, minReference, maxReference,
                               minAttachNo, maxAttachNo);
  CoupledBodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

void fixLinksFromAttachmentHtmls() {
  int minTarget = 1, maxTarget = 80;
  int minReference = 1, maxReference = 80;
  int minAttachNo = 1, maxAttachNo = 1;
  // if to fix all attachments
  //  int minAttachNo = 0, maxAttachNo = 0;
  fixAttachments(minTarget, maxTarget, minReference, maxReference, minAttachNo,
                 maxAttachNo);
}

void fixLinksFromAttachment() {
  LinkFromAttachment::resetStatisticsAndLoadReferenceAttachmentList();
  fixLinksFromAttachmentHtmls();
  LinkFromAttachment::outPutStatisticsToFiles();
  cout << "fixLinksFromAttachment finished. " << endl;
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
    CoupledBodyTextWithLink bodyText;
    bodyText.setFilePrefixFromFileType(fileType);
    bodyText.setFileAndAttachmentNumber(file);
    bodyText.removePersonalCommentsOverNumberedFiles();
  }
}

void removeImageForAutoNumbering() {}

void addImageBackForManualNumbering() {}

void testRemovePersonalViewpoints() {
  int minTarget = 54, maxTarget = 54;
  removePersonalViewpoints(minTarget, maxTarget, FILE_TYPE::MAIN);
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

void testContainer() {
  ListContainer container("1_gen");
  auto link = fixLinkFromMainTemplate(
      "", "80", LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");

  // to test link to original file
  link = fixLinkFromOriginalTemplate(originalDirForLinkFromMain, "80",
                                     "菱角菱花", "第80章1.1节:", "94");

  LinkFromMain lfm("", link);
  lfm.readReferFileName(link); // second step of construction
  lfm.fixFromString(link);
  link = lfm.asString();

  container.initBodyTextFile();
  container.appendParagraphInBodyText(link);
  container.appendParagraphInBodyText("18 links are found.");
  container.assembleBackToHTM("test", "test container");
  cout << "result is in file " << container.getOutputFilePath() << endl;
}
