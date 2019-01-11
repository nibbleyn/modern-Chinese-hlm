#include "bodyTextWithLink.hpp"

/**
 * fix links of certain type in referFile which refer to one of file in files
 * @param referFile
 * @param files
 */
void BodyTextWithLink::fixLinksFromFile(const string &file, fileSet files,
                                        int attachNo, int minPara, int maxPara,
                                        int minLine, int maxLine) {
  string attachmentPart{""};
  if (attachNo != 0)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);

  string inputFile =
      BODY_TEXT_OUTPUT + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;
  string outputFile =
      BODY_TEXT_FIX + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
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

      if (attachNo != 0) {
        lfm = std::make_unique<LinkFromMain>(file, link);
      } else {
        lfm = std::make_unique<LinkFromAttachment>(file + attachmentPart, link);
      }
      lfm->readReferFileName(link); // second step of construction, this is
                                    // needed to check isTargetToSelfHtm
      if (lfm->isTargetToOtherAttachmentHtm()) {
        lfm->fixFromString(link); // third step of construction
        lfm->setSourcePara(ln);
        lfm->doStatistics();
      }
      if (lfm->isTargetToSelfHtm()) {
        lfm->setSourcePara(ln);
        lfm->fixFromString(link); // third step of construction
        if (lfm->needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), lfm->asString());
        }
      }
      if (lfm->isTargetToOtherMainHtm()) {
        targetFile = lfm->getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm->fixFromString(link); // third step of construction
          lfm->setSourcePara(ln);
          string next = originalLinkStartChars + linkStartChars;
          bool needAddOrginalLink = true;
          // still have above "next" and </a>
          if (inLine.length() > (link.length() + next.length() + 4)) {
            if (inLine.substr(linkEnd + 4, next.length()) == next) {
              // skip </a> and first parenthesis of next
              auto followingLink = inLine.substr(
                  linkEnd + next.length() + 2); // find next link in the inLine
              if (attachNo != 0) {
                following = std::make_unique<LinkFromMain>(file, followingLink);
              } else {
                following = std::make_unique<LinkFromAttachment>(
                    file + attachmentPart, followingLink);
              }
              if (following->isTargetToOriginalHtm()) {
                needAddOrginalLink = false;
              }
            }
          }
          if (needAddOrginalLink)
            lfm->generateLinkToOrigin();
          lfm->doStatistics();
          if (lfm->needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), lfm->asString());
          }
        }
      }
      if (lfm->isTargetToOriginalHtm()) {
        targetFile = lfm->getChapterName();
        auto e = find(files.begin(), files.end(), targetFile);
        if (e != files.end()) // need to check and fix
        {
          lfm->fixFromString(link); // third step of construction
          if (lfm->needUpdate())    // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            if (debug >= LOG_INFO)
              SEPERATE("isTargetToOriginalHtm", orgLine + "\n" + link);
            orgLine.replace(orglinkBegin, link.length(), lfm->asString());
          }
        }
      }
      inLine = inLine.substr(linkEnd + 4); // find next link in the inLine
    } while (1);
    outfile << orgLine << endl;
  }
}
