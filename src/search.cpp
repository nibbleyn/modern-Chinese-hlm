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
      for (auto const &line : lineSet) {
        LineNumber ln(line);
        string expectedSection =
            R"(第)" + TurnToString(TurnToInt(file)) + R"(章)" +
            TurnToString(ln.getParaNumber()) + R"(.)" +
            TurnToString(ln.getlineNumber()) + R"(节:)";
        if (targetFileType == FILE_TYPE::ORIGINAL) {
          Link::LinkDetails detail{
              key, file, line,
              fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file, key,
                                          expectedSection, line)};
          resultLinkList[file].push_back(detail);
        } else if (targetFileType == FILE_TYPE::MAIN) {
          Link::LinkDetails detail{
              key, file, line,
              fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::DIRECT, key,
                                      expectedSection, expectedSection, line)};
          resultLinkList[file].push_back(detail);
        }
        total++;
      }
    }
  }
  bool asList = false;
  if (asList) {
    ListContainer container(outputFilename);
    container.clearExistingBodyText();
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
    FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                    << endl;
  } else {
    TableContainer container(outputFilename);
    container.insertFrontParagrapHeader(total, searchUnit);
    int i = 1;
    for (const auto &chapter : resultLinkList) {
      auto list = chapter.second;
      for (const auto &detail : list) {
        if (i % 2 == 0)
          container.appendRightParagraphInBodyText(detail.link);
        else
          container.appendLeftParagraphInBodyText(detail.link);
        i++;
      }
    }
    // patch last right part
    if (total % 2 != 0)
      container.appendRightParagraphInBodyText("");
    container.insertBackParagrapHeader(0, total, searchUnit);
    container.assembleBackToHTM("search  results",
                                "searchInFiles for key: " + key);
    FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                    << endl;
  }
}

void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true) {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  findFirstInNoAttachmentFiles(key, fileType, minTarget, maxTarget,
                               outputFilename);
}

using KeyList = vector<string>;

void searchKeywordInNoAttachmentFiles(KeyList &keyList, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true) {}

void searchByTagInNoAttachmentFiles(const string &tag, const string &fileType,
                                    const string &outputFilename,
                                    bool showLineNumberOnly = true) {}

void searchForImages() {}

void findFirstInFiles() {
  searchKeywordInNoAttachmentFiles(R"(司棋)", "main", "xxx1");

  //  searchKeywordInNoAttachmentFiles(R"(聚赌嫖娼)", "main", "xxx3");
  //  searchKeywordInNoAttachmentFiles(R"(头一社)", "original", "xxx4");
}
