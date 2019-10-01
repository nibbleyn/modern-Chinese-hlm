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

static const string PIC_INDEX = R"(picIndex)";
static const string PIC_TITLE = R"(图片)";
static const string PIC_DISPLAY_TITLE = R"(图片 目录)";

static const string POEM_INDEX = R"(poemIndex)";
static const string POEM_TITLE = R"(诗词翻译)";
static const string POEM_DISPLAY_TITLE = R"(诗词翻译 目录)";

void generateContentTableForMainHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::MAIN);

  TableContainer outputContainer(MAIN_INDEX);
  outputContainer.setConstantBodyTextFileName(
      TABLE_CONTAINER_FILENAME_SMALLER_FONT);
  outputContainer.setMaxTarget(MAIN_MAX_CHAPTER_NUMBER);
  outputContainer.createParaListFrom(6, 10, 70);
  outputContainer.addOneParaHeaderPosition(72);
  outputContainer.enableAddExistingFrontLinks();

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    AttachmentNumber num(TurnToInt(file), 0);
    outputContainer.addLinkToLinkStringSet(
        num,
        fixLinkFromMainTemplate(emptyString, file, DISPLAY_TYPE::UNHIDDEN,
                                emptyString, emptyString,
                                brTab + container.getOriginalTitle() + brTab +
                                    container.getTranslatedTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(MAIN_TITLE);
  outputContainer.setDisplayTitle(MAIN_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Main Htmls finished. " << endl;
}

void generateContentTableForOriginalHtmls() {
  int minTarget = MAIN_MIN_CHAPTER_NUMBER, maxTarget = MAIN_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::ORIGINAL);
  TableContainer outputContainer(ORG_INDEX);
  outputContainer.setMaxTarget(MAIN_MAX_CHAPTER_NUMBER);
  outputContainer.createParaListFrom(18, 22, 70);

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget, ORIGINAL)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    AttachmentNumber num(TurnToInt(file), 0);
    outputContainer.addLinkToLinkStringSet(
        num, fixLinkFromOriginalTemplate(originalDirForLinkFromMain, file,
                                         emptyString, emptyString,
                                         container.getOriginalTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(ORG_TITLE);
  outputContainer.setDisplayTitle(ORG_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for Original Htmls finished. "
                  << endl;
}

void generateContentTableForJPMHtmls() {
  int minTarget = JPM_MIN_CHAPTER_NUMBER, maxTarget = JPM_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::JPM);
  TableContainer outputContainer(JPM_INDEX);
  outputContainer.setMaxTarget(JPM_MAX_CHAPTER_NUMBER);
  outputContainer.createParaListFrom(18, 22, 90);

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget, JPM)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    AttachmentNumber num(TurnToInt(file), 0);
    outputContainer.addLinkToLinkStringSet(
        num, fixLinkFromJPMTemplate(jpmDirForLinkFromMain, file, emptyString,
                                    emptyString, container.getOriginalTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(JPM_TITLE);
  outputContainer.setDisplayTitle(JPM_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for JPM Htmls finished. " << endl;
}

void generateContentTableForReferenceAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList) {
    CoupledBodyTextContainer::refAttachmentTable.setSourceFile(
        OUTPUT_REF_ATTACHMENT_LIST_PATH);
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  }
  TableContainer outputContainer(REFERENCE_ATTACHMENT_INDEX);
  outputContainer.clearLinkStringSet();
  outputContainer.assignLinkStringSet(
      CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
          ATTACHMENT_TYPE::REFERENCE));
  outputContainer.setMaxTargetAsSetSize();
  outputContainer.createParaListFrom(18, 22);
  outputContainer.outputToBodyTextFromLinkList(attachmentUnit);
  outputContainer.setTitle(REFERENCE_ATTACHMENT_TITLE);
  outputContainer.setDisplayTitle(REFERENCE_ATTACHMENT_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file "
                  << outputContainer.getoutputHtmlFilepath() << endl;
}

void generateContentTableForImages() {
  TableContainer outputContainer(PIC_INDEX);
  outputContainer.clearLinkStringSet();
  LinkFromMain::setupStatisticsParameters();
  outputContainer.assignLinkStringSet(
      CoupledLink::loadImagesLinksFromStatisticsFile());
  outputContainer.setMaxTargetAsSetSize();
  outputContainer.createParaListFrom(18, 22);
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(PIC_TITLE);
  outputContainer.setDisplayTitle(PIC_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for images finished. " << endl;
}

void generateContentTableForPoems() {
  TableContainer outputContainer(POEM_INDEX);
  outputContainer.clearLinkStringSet();
  outputContainer.assignLinkStringSet(
      CoupledBodyTextWithLink::loadPoemsLinksFromStatisticsFile());
  outputContainer.setMaxTargetAsSetSize();
  outputContainer.createParaListFrom(18, 22);
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(POEM_TITLE);
  outputContainer.setDisplayTitle(POEM_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for poems finished. " << endl;
}

void generateContentTableForPersonalAttachments(
    bool needToReloadAttachmentList) {
  if (needToReloadAttachmentList) {
    CoupledBodyTextContainer::refAttachmentTable.setSourceFile(
        OUTPUT_REF_ATTACHMENT_LIST_PATH);
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  }
  TableContainer outputContainer(PERSONAL_ATTACHMENT_INDEX);
  outputContainer.clearLinkStringSet();
  outputContainer.assignLinkStringSet(
      CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
          ATTACHMENT_TYPE::PERSONAL));
  outputContainer.setMaxTargetAsSetSize();
  outputContainer.createParaListFrom(18, 22);
  outputContainer.outputToBodyTextFromLinkList(attachmentUnit);
  outputContainer.setTitle(PERSONAL_ATTACHMENT_TITLE);
  outputContainer.setDisplayTitle(PERSONAL_ATTACHMENT_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file "
                  << outputContainer.getoutputHtmlFilepath() << endl;
}
