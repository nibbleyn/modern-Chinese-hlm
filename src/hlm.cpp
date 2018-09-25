#include "hlm.hpp"
#include "Poco/File.h"

/**
 *
 * @param outputHTMFilename
 */
void assembleContainerHTM(string outputHTMFilename, int containerNumber,
                          string title, string displayTitle,
                          int lastParaNumber) {
  string inputHtmlFile =
      HTML_CONTAINER + TurnToString(containerNumber) + ".htm";
  ;
  cout << inputHtmlFile << endl;
  string inputBodyTextFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << inputBodyTextFile << endl;
  ifstream inHtmlFile(inputHtmlFile);
  if (!inHtmlFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputHtmlFile << endl;
    return;
  }
  ifstream inBodyTextFile(inputBodyTextFile);
  if (!inBodyTextFile) // doesn't exist
  {
    cout << "file doesn't exist:" << inputBodyTextFile << endl;
    return;
  }
  ofstream outfile(outputHTMFilename);
  string line{"not found"};
  bool started = false;
  string start = R"(name="top")";  // first line
  string end = R"(name="bottom")"; // last line
  string defaultTitle = "XXX";
  string defaultDisplayTitle = "YYY";
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    if (not started) {
      auto linkBegin = line.find(start);
      if (linkBegin != string::npos) {
        started = true;
        break;
      }
      if (not title.empty()) {
        auto titleBegin = line.find(defaultTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultTitle.length(), title);
      }
      if (not displayTitle.empty()) {
        auto titleBegin = line.find(defaultDisplayTitle);
        if (titleBegin != string::npos)
          line.replace(titleBegin, defaultDisplayTitle.length(), displayTitle);
      }
      if (debug)
        cout << line << endl;  // including end line
      outfile << line << endl; // excluding start line
    }
  }
  if (inHtmlFile.eof() and not started) {
    cout << "source htm" << inputBodyTextFile << "has no start mark:" << start
         << endl;
    return;
  }
  bool ended = false;
  while (!inBodyTextFile.eof()) // To get you all the lines.
  {
    getline(inBodyTextFile, line); // Saves the line in line.
    if (debug)
      cout << line << endl;  // including end line
    outfile << line << endl; // including end line
  }
  while (!inHtmlFile.eof()) // To get you all the lines.
  {
    getline(inHtmlFile, line); // Saves the line in line.
    if (not ended) {
      auto linkEnd = line.find(end);
      if (linkEnd != string::npos) {
        ended = true;
        continue;
      }
    } else {
      if (debug)
        cout << line << endl;  // including end line
      outfile << line << endl; // excluding end line
    }
  }
}

/**
 * to get ready to write new text in this file which would be composed into
 * container htm
 */
void clearLinksInContainerBodyText(int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile(outputFile);
}

/**
 * append a text string in the body text of final htm file
 * @param text the string to put into
 * @param containerNumber the selected container to put into
 */
void appendTextInContainerBodyText(string text, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>"
          << R"(<b>)" << text << "</b>"
          << "</br>" << endl;
}

void appendNumberLineInContainerBodyText(string line, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>" << endl;
  outfile << line << endl;
}

/**
 * append a link string in the body text of final htm file
 * @param linkString the string to put into
 * @param containerNumber the selected container to put into
 */
void appendLinkInContainerBodyText(string linkString, int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << "<br>" << linkString << "</br>" << endl;
}

void addFirstParagraphInContainerBodyText(int startNumber,
                                          int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  //  outfile << firstParaHeader << endl;
}

void addParagraphInContainerBodyText(int startNumber, int paraNumber,
                                     int containerNumber) {}

void addLastParagraphInContainerBodyText(int startNumber, int paraNumber,
                                         int containerNumber) {
  string outputFile =
      BODY_TEXT_CONTAINER + TurnToString(containerNumber) + ".txt";
  cout << outputFile << endl;
  ofstream outfile;
  outfile.open(outputFile, std::ios_base::app);
  outfile << R"(<hr color="#F0BEC0">)" << endl;
  //  outfile << lastParaHeader << endl;
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

void removePersonalCommentsOverNumberedFiles(string referFile,
                                             FILE_TYPE fileType, int attachNo) {
  string attachmentPart{""};
  if (fileType ==
      FILE_TYPE::ATTACHMENT) // type is only used here in this function
    attachmentPart = "_" + TurnToString(attachNo);
  string inputFile = BODY_TEXT_OUTPUT + getBodyTextFilePrefix(fileType) +
                     referFile + attachmentPart + ".txt";

  ifstream infile(inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << inputFile << endl;
    return;
  }
  string outputFile = BODY_TEXT_FIX + getBodyTextFilePrefix(fileType) +
                      referFile + attachmentPart + ".txt";
  ofstream outfile(outputFile);
  string inLine{"not found"};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug)
      cout << inLine << endl;
    auto orgLine = inLine; // inLine would change in loop below
    string start = R"(（<u unhidden)";
    string end = R"(</u>）)";
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
    string linkStart = R"(<a)";
    string linkEnd = R"(</a>)";
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

void findFirstInFiles(string key, FILE_TYPE targetFileType, int minTarget,
                      int maxTarget, string outputFilename) {
  //  LinkSet resultLinkList;
  //
  //  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
  //    string referPara{""};
  //    bool ignoreChange;
  //    string newKey = findKeyInFile(key, file, targetFileType, referPara,
  //                                  ignoreChange); // continue using same key
  //    if (newKey.find("KeyNotFound") == string::npos) {
  //      if (targetFileType == FILE_TYPE::ORIGINAL) {
  //        LinkDetails detail;
  //        detail.usedKey = key;
  //        detail.referPara = referPara;
  //        detail.annotation = fixLinkFromOriginalTemplate(file, key,
  //        referPara); resultLinkList[file].push_back(detail);
  //      }
  //      if (targetFileType == FILE_TYPE::MAIN) {
  //        LinkDetails detail;
  //        detail.usedKey = key;
  //        detail.referPara = referPara;
  //        LineNumber ln(START_PARA_NUMBER - 1, referPara);
  //        string expectedSection =
  //            R"(第)" + TurnToString(TurnToInt(file)) + R"(章)" +
  //            TurnToString(ln.getParaNumber()) + R"(.)" +
  //            TurnToString(ln.getlineNumber()) + R"(节:)";
  //        detail.annotation = fixLinkFromMainTemplate(
  //            file, DIRECT, key, expectedSection, key, referPara);
  //        resultLinkList[file].push_back(detail);
  //      }
  //    }
  //  }
  //  // use default no. 1 container to output
  //  clearLinksInContainerBodyText(1);
  //  int total = 0;
  //  for (const auto &chapter : resultLinkList) {
  //    appendLinkInContainerBodyText("found in " +
  //    filenamePrefix[targetFileType]
  //    +
  //                                      chapter.first + ".htm:",
  //                                  1);
  //    auto list = chapter.second;
  //    for (const auto &detail : list) {
  //      total++;
  //      appendLinkInContainerBodyText(detail.annotation, 1);
  //    }
  //  }
  //  appendLinkInContainerBodyText(TurnToString(total) + " links are found.",
  //  1); assembleContainerHTM(HTML_OUTPUT + outputFilename, 1, "search
  //  results",
  //                       "searchInFiles for key: " +
  //                           key); // always under HTML_OUTPUT
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
  cout << "and to remove attachment link with type of 感悟." << endl;
  for (const auto &file :
       buildFileSet(minTarget, maxTarget)) // files need to be fixed
  {
    removePersonalCommentsOverNumberedFiles(file, fileType); // reference files
  }
}
