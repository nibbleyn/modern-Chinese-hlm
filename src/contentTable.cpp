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

void generateContentTableForMainHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::MAIN);

  TableContainer outputContainer(MAIN_INDEX);
  outputContainer.setInputFileName(TABLE_CONTAINER_FILENAME_SMALLER_FONT);
  outputContainer.createParaListFrom(6, 10, 70);
  outputContainer.addOneParaHeaderPosition(72);
  outputContainer.setMaxTarget(MAIN_MAX_CHAPTER_NUMBER);
  outputContainer.enableAddExistingFrontLinks();

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    outputContainer.addLinkToLinkStringSet(
        fixLinkFromMainTemplate("", file, LINK_DISPLAY_TYPE::UNHIDDEN, "", "",
                                brTab + container.getOriginalTitle() + brTab +
                                    container.getTranslatedTitle(),
                                ""));
  }
  outputContainer.outputToBodyTextFromLinkList();
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

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    outputContainer.addLinkToLinkStringSet(
        fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file, "", "",
                                    "", container.getOriginalTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
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

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    outputContainer.addLinkToLinkStringSet(fixLinkFromJPMTemplate(
        jpmDirForLinkFromMain, file, "", "", "", container.getOriginalTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.assembleBackToHTM(JPM_TITLE, JPM_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getOutputFilePath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for JPM Htmls finished. " << endl;
}

void generateContentTableForReferenceAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList)
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  TableContainer container(REFERENCE_ATTACHMENT_INDEX);
  container.assignLinkStringSet(
      CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
          ATTACHMENT_TYPE::REFERENCE));
  container.hideParaHeaders();
  container.outputToBodyTextFromLinkList();
  container.assembleBackToHTM(REFERENCE_ATTACHMENT_TITLE,
                              REFERENCE_ATTACHMENT_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}

void generateContentTableForPersonalAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList)
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  TableContainer container(PERSONAL_ATTACHMENT_INDEX);
  container.assignLinkStringSet(
      CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
          ATTACHMENT_TYPE::PERSONAL));
  container.hideParaHeaders();
  container.outputToBodyTextFromLinkList();
  container.assembleBackToHTM(PERSONAL_ATTACHMENT_TITLE,
                              PERSONAL_ATTACHMENT_DISPLAY_TITLE);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}
