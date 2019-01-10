#include "search.hpp"

void findFirstInNoAttachmentFiles(const string key, FILE_TYPE targetFileType,
                                  int minTarget, int maxTarget,
                                  const string &outputFilename) {

  using LinksList = map<string, vector<Link::LinkDetails>>;
  LinksList resultLinkList;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {

    BodyText bodyText(getBodyTextFilePrefix(targetFileType));
    bodyText.resetBeforeSearch();
    bodyText.searchForAll();
    bool found = bodyText.findKey(key, file);
    if (found) {
      BodyText::lineNumberSet lineSet = bodyText.getResultLineSet();
      if (targetFileType == FILE_TYPE::ORIGINAL) {
        for (auto const &line : lineSet) {
          Link::LinkDetails detail{
              key, file, line,
              fixLinkFromOriginalTemplate(R"(original\)", file, key, line)};
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
          resultLinkList[file].push_back(detail);
        }
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
  cout << "result is in file " << container.getOutputFilePath() << endl;
}

void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll) {
  int minTarget = 1, maxTarget = 80;
  findFirstInNoAttachmentFiles(key, getFileTypeFromString(fileType), minTarget,
                               maxTarget, outputFilename);
}
