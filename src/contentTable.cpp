#include "contentTable.hpp"

static const string MAIN_INDEX = R"(aindex)";
static const string MAIN_TITLE = R"(脂砚斋重评石头记 现代白话文版)";
static const string MAIN_DISPLAY_TITLE =
    R"(脂砚斋重评石头记 现代白话文版 目录)";

static const string ORG_INDEX = R"(cindex)";
static const string ORG_TITLE = R"(脂砚斋重评石头记 原文)";
static const string ORG_DISPLAY_TITLE = R"(脂砚斋重评石头记 原文目录)";

static const string JPM_INDEX = R"(dindex)";
static const string JPM_TITLE = R"(张竹坡批注金瓶梅)";
static const string JPM_DISPLAY_TITLE = R"(张竹坡批注金瓶梅 目录)";

static const string REFERENCE_ATTACHMENT_INDEX = R"(bindex1)";
static const string REFERENCE_ATTACHMENT_TITLE = R"(引经据典 附件目录)";
static const string REFERENCE_ATTACHMENT_DISPLAY_TITLE =
    R"(引经据典 附件目录)";

static const string PERSONAL_ATTACHMENT_INDEX = R"(bindex2)";
static const string PERSONAL_ATTACHMENT_TITLE = R"(个人感悟 附件目录)";
static const string PERSONAL_ATTACHMENT_DISPLAY_TITLE =
    R"(个人感悟 附件目录)";

vector<int> createParaList(int first, int incremental, int max) {
  vector<int> result;
  result.push_back(first);
  int i = first;
  while ((i += incremental) < max) {
    result.push_back(i);
  }
  return result;
}

void generateContentTableForMainHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::MAIN);

  TableContainer outputContainer(MAIN_INDEX);
  outputContainer.setInputFileName(TABLE_CONTAINER_FILENAME_SMALLER_FONT);
  outputContainer.createParaListFrom(6, 10, 70);
  outputContainer.addOneParaHeaderPosition(72);
  auto paraList = createParaList(6, 10, 70);
  paraList.push_back(72);
  sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  outputContainer.addExistingFrontParagraphs();
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link =
        fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::UNHIDDEN, "", "",
                                brTab + container.getOriginalTitle() + brTab +
                                    container.getTranslatedTitle(),
                                "");
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(MAIN_TITLE, MAIN_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Main Htmls finished. " << endl;
}

void generateContentTableForOriginalHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::ORIGINAL);
  TableContainer outputContainer(ORG_INDEX);
  outputContainer.createParaListFrom(18, 22, 70);
  auto paraList = createParaList(18, 22, 70);
  sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link =
        fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file, "", "",
                                    "", container.getOriginalTitle());
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(ORG_TITLE, ORG_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Original Htmls finished. "
                  << endl;
}

void generateContentTableForJPMHtmls() {
  int minTarget = JPM_MIN_CHAPTER_NUMBER, maxTarget = JPM_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::JPM);
  TableContainer outputContainer(JPM_INDEX);
  outputContainer.createParaListFrom(18, 22, 90);
  auto paraList = createParaList(18, 22, 90);
  sort(paraList.begin(), paraList.end());
  for (const auto &no : paraList)
    FUNCTION_OUTPUT << no << endl;
  auto start = paraList.begin();
  outputContainer.insertFrontParagrapHeader(*start);
  int i = 1;
  int seqOfPara = 1;
  int totalPara = 0;
  for (const auto &file :
       buildFileSet(minTarget, maxTarget, THREE_DIGIT_FILENAME)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    auto link = fixLinkFromJPMTemplate(jpmDirForLinkFromMain, file, "", "", "",
                                       container.getOriginalTitle());
    if (i % 2 == 0)
      outputContainer.appendRightParagraphInBodyText(link);
    else
      outputContainer.appendLeftParagraphInBodyText(link);
    if (start != paraList.end() and i == *start) {
      auto enterLastPara = (start + 1 == paraList.end());
      auto startParaNo = i + 1;
      int endParaNo = (enterLastPara) ? maxTarget : *(start + 1);
      totalPara = endParaNo - startParaNo + 1;
      int preTotalPara = i - *(start - 1);
      outputContainer.insertMiddleParagrapHeader(enterLastPara, seqOfPara,
                                                 startParaNo, endParaNo,
                                                 totalPara, preTotalPara);
      seqOfPara++;
      start++;
    }
    i++;
  }
  outputContainer.insertBackParagrapHeader(seqOfPara, totalPara);
  outputContainer.assembleBackToHTM(JPM_TITLE, JPM_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for JPM Htmls finished. " << endl;
}

void generateContentTableForReferenceAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList)
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  ListContainer container(REFERENCE_ATTACHMENT_INDEX);
  container.assignLinkStringSet(CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
           ATTACHMENT_TYPE::REFERENCE));
  for (const auto &link :
       CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
           ATTACHMENT_TYPE::REFERENCE)) {
    container.appendParagraphInBodyText(link);
  }
  container.assembleBackToHTM(REFERENCE_ATTACHMENT_TITLE,
                              REFERENCE_ATTACHMENT_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}

void generateContentTableForPersonalAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList)
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  ListContainer container(PERSONAL_ATTACHMENT_INDEX);
  auto table = CoupledBodyTextContainer::refAttachmentTable;
  for (const auto &link :
       CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
           ATTACHMENT_TYPE::PERSONAL)) {
    container.appendParagraphInBodyText(link);
  }
  container.assembleBackToHTM(PERSONAL_ATTACHMENT_TITLE,
                              PERSONAL_ATTACHMENT_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}
