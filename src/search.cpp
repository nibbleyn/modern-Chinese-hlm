#include "search.hpp"

void findFirstInNoAttachmentFiles(const string key, const string &fileType,
                                  int minTarget, int maxTarget,
                                  const string &outputFilename) {
  FILE_TYPE targetFileType = getFileTypeFromString(fileType);
  bool asList = false;
  unique_ptr<LinkSetContainer> containerPtr{nullptr};
  if (asList) {
    containerPtr = make_unique<ListContainer>(outputFilename);
  } else {
    containerPtr = make_unique<TableContainer>(outputFilename);
  }
  containerPtr->clearLinkStringSet();

  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(targetFileType);
    bodyText.resetBeforeSearch();
    bodyText.searchForAll();
    if (targetFileType == FILE_TYPE::MAIN)
      bodyText.ignorePersonalComments();
    bodyText.setFileAndAttachmentNumber(file);
    bool found = bodyText.findKey(key);
    if (found) {
      CoupledBodyText::lineNumberSet lineSet = bodyText.getResultLineSet();
      auto seq = 1;
      for (auto const &line : lineSet) {
        LineNumber ln(line);
        string expectedSection =
            R"(第)" + TurnToString(TurnToInt(file)) + R"(章)" +
            TurnToString(ln.getParaNumber()) + R"(.)" +
            TurnToString(ln.getlineNumber()) + R"(节:)";
        // reuse attachment number as sequence of item
        // should improve when search over attachments
        AttachmentNumber num(TurnToInt(file), seq);
        switch (targetFileType) {
        case FILE_TYPE::MAIN:
          containerPtr->addLinkToLinkStringSet(
              num,
              fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::DIRECT, key,
                                      expectedSection, expectedSection, line));
          break;
        case FILE_TYPE::ORIGINAL:
          containerPtr->addLinkToLinkStringSet(
              num, fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file,
                                               key, expectedSection, line));
          break;
        case FILE_TYPE::JPM:
          containerPtr->addLinkToLinkStringSet(
              num, fixLinkFromJPMTemplate(originalDirForLinkFromMain, file, key,
                                          expectedSection, line));
          break;
        case FILE_TYPE::ATTACHMENT:
        default:
          FUNCTION_OUTPUT << "not supported yet." << endl;
        }
        seq++;
      }
    }
  }
  auto total = containerPtr->getLinkStringSetSize();
  containerPtr->setMaxTargetAsSetSize();
  containerPtr->createParaListFrom(18, 22);
  containerPtr->outputToBodyTextFromLinkList(searchUnit);
  string verb = (total > 1) ? "s are" : " is";
  containerPtr->assembleBackToHTM("search  results",
                                  "search for key: " + key + " in " + fileType +
                                      " files. " + TurnToString(total) +
                                      " link" + verb + " found.");
  FUNCTION_OUTPUT << "result is in file " << containerPtr->getOutputFilePath()
                  << endl;
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

void search(int num, const string &key, const string &outputFilename) {
  SEPERATE("HLM search", " started ");
  switch (num) {
  case 1:
    searchKeywordInNoAttachmentFiles(key, MAIN, outputFilename);
    break;
  case 2:
    searchKeywordInNoAttachmentFiles(key, ORIGINAL, outputFilename);
    break;
  case 3:
    searchKeywordInNoAttachmentFiles(key, JPM, outputFilename);
    break;
  default:
    FUNCTION_OUTPUT << "invalid search." << endl;
  }
}
