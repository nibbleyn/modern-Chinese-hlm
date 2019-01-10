#include "search.hpp"

string findKeyInFile(const string &key, const string &fullPath,
                     string &lineNumber, bool &needChange) {
  ifstream infile(fullPath);
  if (!infile) {
    cout << "file doesn't exist:" << fullPath << endl;
    return keyNotFound + key + ": bodytext file doesn't exist";
  }
  string line{""};
  string lineName{""};
  bool found = false;
  LineNumber ln;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    getline(infile, line);              // Saves the line
    if (line.find(key) == string::npos) // not appear in this line
    {
      continue;
    }
    // if "key" is only part of the key of another link, skip this line
    if (isOnlyPartOfOtherKeys(line, key)) {
      continue;
    }
    // find the key in current line
    ln.loadFromContainedLine(line);
    if (not ln.valid()) {
      cout << "file doesn't get numbered:" << fullPath << " at line:" << line
           << endl;
      return keyNotFound + key + ": bodytext file doesn't get numbered";
    }

    found = true;
    break;
  }
  needChange = true;
  if (not found) {
    return keyNotFound + key;
  }
  // continue with lineName found
  lineNumber = ln.asString();
  return key;
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
    string newKey = findKeyInFile(key, referFile, referPara,
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
  ListContainer container(outputFilename);
  container.clearBodyTextFile();
  int total = 0;
  for (const auto &chapter : resultLinkList) {
    container.appendParagraphInBodyText("found in " +
                                        getBodyTextFilePrefix(targetFileType) +
                                        chapter.first + HTML_SUFFIX + " :");
    auto list = chapter.second;
    for (const auto &detail : list) {
      total++;
      container.appendParagraphInBodyText(detail.link);
    }
  }
  container.appendParagraphInBodyText(TurnToString(total) +
                                      " links are found.");
  container.assembleBackToHTM("search  results",
                              "searchInFiles for key: " + key);
}

void findFirstInNoAttachmentFiles(const string &key, const string &fileType,
                                  const string &outputFilename) {
  int minTarget = 1, maxTarget = 80;
  findFirstInNoAttachmentFiles(key, getFileTypeFromString(fileType), minTarget,
                               maxTarget, outputFilename);
  cout << "result is in file "
       << HTML_OUTPUT_MAIN + outputFilename + HTML_SUFFIX << endl;
}
