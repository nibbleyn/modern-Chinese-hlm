#include "personal.hpp"

static const string personalCommentStart = R"(（<u unhidden)";
static const string personalCommentEnd = R"(</u>）)";

void removePersonalCommentsOverNumberedFiles(string referFile,
                                             FILE_TYPE fileType,
                                             int attachNo = 1) {
  string attachmentPart{""};
  if (fileType ==
      FILE_TYPE::ATTACHMENT) // type is only used here in this function
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);
  string inputFile = BODY_TEXT_OUTPUT +
                     getBodyTextFilePrefixFromFileType(fileType) + referFile +
                     attachmentPart + BODY_TEXT_SUFFIX;

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile = BODY_TEXT_FIX +
                      getBodyTextFilePrefixFromFileType(fileType) + referFile +
                      attachmentPart + BODY_TEXT_SUFFIX;
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
