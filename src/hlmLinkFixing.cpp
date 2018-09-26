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
          if (debug)
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
void fixMainLinksOverNumberedFiles(const string &referFile, fileSet files) {
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
  string inLine{"not found"};
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
    if (debug)
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
                       link); // get only type and annotation
      if (lfm.isTargetToOtherAttachmentHtm()) {
        lfm.readReferFileName(link); // second step of construction
        lfm.fixFromString(link);     // third step of construction
        lfm.setSourcePara(ln);
        lfm.doStatistics();
      }
      if (lfm.isTargetToSelfHtm()) {
        lfm.readReferFileName(link); // second step of construction
        lfm.setSourcePara(ln);
        lfm.fixFromString(link); // third step of construction
        if (lfm.needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      if (lfm.isTargetToOtherMainHtm()) {
        lfm.readReferFileName(link); // second step of construction
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
        lfm.readReferFileName(link); // second step of construction
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
      string inputHtmlFile = HTML_SRC_ATTACHMENT +
                             getFileNamePrefix(FILE_TYPE::ATTACHMENT) + file +
                             "_" + TurnToString(attNo) + HTML_SUFFIX;
      string outputFile = HTML_OUTPUT_ATTACHMENT +
                          getFileNamePrefix(FILE_TYPE::ATTACHMENT) + file +
                          "_" + TurnToString(attNo) + HTML_SUFFIX;
      fixReturnLinkForAttachmentFile(file + "_" + TurnToString(attNo),
                                     inputHtmlFile, outputFile);
    }
  }
  if (debug)
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
  backupAndOverwriteSrcForHTML();           // update html src
  dissembleMainHtmls(minTarget, maxTarget); // dissemble html to bodytext
  fixMainLinks(minTarget, maxTarget, minReference, maxReference);
  loadBodyTexts(BODY_TEXT_FIX, BODY_TEXT_OUTPUT);
  assembleMainHtmls(minTarget, maxTarget);
  fixReturnLinkForAttachments(minTarget, maxTarget);
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
                     "_" + TurnToString(attachNo) + BODY_TEXT_SUFFIX;

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile = BODY_TEXT_FIX +
                      getBodyTextFilePrefix(FILE_TYPE::ATTACHMENT) + referFile +
                      "_" + TurnToString(attachNo) + BODY_TEXT_SUFFIX;
  ofstream outfile(outputFile);
  string inLine{"not found"};
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
    if (debug)
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
      LinkFromAttachment lfm(referFile + "_" + TurnToString(attachNo), link);
      if (lfm.isTargetToOtherAttachmentHtm()) {
        lfm.readReferFileName(link); // second step of construction
        lfm.fixFromString(link);     // third step of construction
        lfm.setSourcePara(ln);
        lfm.doStatistics();
      }
      if (lfm.isTargetToSelfHtm()) {
        lfm.readReferFileName(link); // second step of construction
        lfm.setSourcePara(ln);
        lfm.fixFromString(link); // third step of construction
        if (lfm.needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm.asString());
        }
      }
      if (lfm.isTargetToOtherMainHtm()) {
        lfm.readReferFileName(link); // second step of construction
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
        lfm.readReferFileName(link); // second step of construction
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
