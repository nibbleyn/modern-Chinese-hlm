#include "search.hpp"

void findFirstInNoAttachmentFiles(const string key, const string &fileType,
                                  int minTarget, int maxTarget,
                                  const string &outputFilename) {
  FILE_TYPE targetFileType = getFileTypeFromString(fileType);
  using LinksList = map<string, vector<Link::LinkDetails>>;
  LinksList resultLinkList;
  resultLinkList.clear();
  int total = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {

    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(targetFileType);

    bodyText.resetBeforeSearch();
    bodyText.searchForAll();
    bodyText.setFileAndAttachmentNumber(file);
    bool found = bodyText.findKey(key);
    if (found) {
      CoupledBodyText::lineNumberSet lineSet = bodyText.getResultLineSet();
      if (targetFileType == FILE_TYPE::ORIGINAL) {
        for (auto const &line : lineSet) {
          Link::LinkDetails detail{
              key, file, line,
              fixLinkFromOriginalTemplate(R"(original\)", file, key, line)};
          total++;
          resultLinkList[file].push_back(detail);
        }
      }
      if (targetFileType == FILE_TYPE::MAIN) {
        for (auto const &line : lineSet) {
          LineNumber ln(line);
          string expectedSection =
              R"(第)" + TurnToString(TurnToInt(file)) + R"(章)" +
              TurnToString(ln.getParaNumber()) + R"(.)" +
              TurnToString(ln.getlineNumber()) + R"(节:)";
          Link::LinkDetails detail{
              key, file, line,
              fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::DIRECT, key,
                                      expectedSection, key, line)};
          total++;
          resultLinkList[file].push_back(detail);
        }
      }
    }
  }
  bool asList = true;
  if (asList) {
    ListContainer container(outputFilename);
    container.initBodyTextFile();
    for (const auto &chapter : resultLinkList) {
      container.appendParagraphInBodyText("found in " + fileType + " : " +
                                          chapter.first + HTML_SUFFIX + " :");
      auto list = chapter.second;
      for (const auto &detail : list) {
        container.appendParagraphInBodyText(detail.link);
      }
    }
    string verb = (total > 1) ? "s are" : " is";
    container.appendParagraphInBodyText(TurnToString(total) + " link" + verb +
                                        " found.");
    container.assembleBackToHTM("search  results",
                                "searchInFiles for key: " + key);
    cout << "result is in file " << container.getOutputFilePath() << endl;
  } else {
    TableContainer container(outputFilename);
    container.initBodyTextFile();
    container.finishBodyTextFile();
    container.assembleBackToHTM("search  results",
                                "searchInFiles for key: " + key);
    cout << "result is in file " << container.getOutputFilePath() << endl;
  }
}

void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll) {
  int minTarget = 1, maxTarget = 80;
  findFirstInNoAttachmentFiles(key, fileType, minTarget, maxTarget,
                               outputFilename);
}

void testFindFirstInFiles() {
  searchKeywordInNoAttachmentFiles(R"(聚赌嫖娼)", "main", "xxx3");
  searchKeywordInNoAttachmentFiles(R"(头一社)", "original", "xxx4");
}
