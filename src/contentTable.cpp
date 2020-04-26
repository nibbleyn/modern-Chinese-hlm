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

static const string MDT_INDEX = R"(eindex)";
static const string MDT_TITLE = R"(牡丹亭)";
static const string MDT_DISPLAY_TITLE = R"(牡丹亭 目录)";

static const string XXJ_INDEX = R"(findex)";
static const string XXJ_TITLE = R"(西厢记)";
static const string XXJ_DISPLAY_TITLE = R"(西厢记 目录)";

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

void generateContentTableForMDTHtmls() {
  int minTarget = MDT_MIN_CHAPTER_NUMBER, maxTarget = MDT_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::MDT);
  TableContainer outputContainer(MDT_INDEX);
  outputContainer.setMaxTarget(MDT_MAX_CHAPTER_NUMBER);
  outputContainer.createParaListFrom(18, 22, 54);

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget, MDT)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    AttachmentNumber num(TurnToInt(file), 0);
    outputContainer.addLinkToLinkStringSet(
        num, fixLinkFromMDTTemplate(mdtDirForLinkFromMain, file, emptyString,
                                    emptyString, container.getOriginalTitle()));
  }
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(MDT_TITLE);
  outputContainer.setDisplayTitle(MDT_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for MDT Htmls finished. " << endl;
}

void generateContentTableForXXJHtmls() {
  int minTarget = XXJ_MIN_CHAPTER_NUMBER, maxTarget = XXJ_MAX_CHAPTER_NUMBER;
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::XXJ);
  TableContainer outputContainer(XXJ_INDEX);
  outputContainer.setMaxTarget(XXJ_MAX_CHAPTER_NUMBER);

  outputContainer.clearLinkStringSet();
  for (const auto &file : buildFileSet(minTarget, maxTarget, XXJ)) {
    container.setFileAndAttachmentNumber(file);
    container.fetchOriginalAndTranslatedTitles();
    AttachmentNumber num(TurnToInt(file), 0);
    outputContainer.addLinkToLinkStringSet(
        num, fixLinkFromXXJTemplate(xxjDirForLinkFromMain, file, emptyString,
                                    emptyString, container.getOriginalTitle()));
  }
  outputContainer.createParaListFrom(18, 22, 16);
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(XXJ_TITLE);
  outputContainer.setDisplayTitle(XXJ_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for XXJ Htmls finished. " << endl;
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
  outputContainer.outputToBodyTextFromLinkList();
  outputContainer.setTitle(POEM_TITLE);
  outputContainer.setDisplayTitle(POEM_DISPLAY_TITLE);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: "
                  << outputContainer.getoutputHtmlFilepath() << endl;
  FUNCTION_OUTPUT << "generateContentTable for poems finished. " << endl;
}

static const string REFERENCE_ATTACHMENT_INDEX = R"(bindex1)";
static const string REFERENCE_ATTACHMENT_TITLE = R"(引经据典 附件目录)";
static const string REFERENCE_ATTACHMENT_DISPLAY_TITLE =
    R"(引经据典 附件目录)";

static const string PERSONAL_ATTACHMENT_INDEX = R"(bindex2)";
static const string PERSONAL_ATTACHMENT_TITLE = R"(个人感悟 附件目录)";
static const string PERSONAL_ATTACHMENT_DISPLAY_TITLE =
    R"(个人感悟 附件目录)";

void generateContentTableForAttachments(ATTACHMENT_TYPE type,
                                        bool needToReloadAttachmentList) {
  if (not(type == ATTACHMENT_TYPE::PERSONAL or
          type == ATTACHMENT_TYPE::REFERENCE))
    return;
  auto index = (type == ATTACHMENT_TYPE::PERSONAL) ? PERSONAL_ATTACHMENT_INDEX
                                                   : REFERENCE_ATTACHMENT_INDEX;
  auto title = (type == ATTACHMENT_TYPE::PERSONAL) ? PERSONAL_ATTACHMENT_TITLE
                                                   : REFERENCE_ATTACHMENT_TITLE;
  auto displayTitle = (type == ATTACHMENT_TYPE::PERSONAL)
                          ? PERSONAL_ATTACHMENT_DISPLAY_TITLE
                          : REFERENCE_ATTACHMENT_DISPLAY_TITLE;

  if (needToReloadAttachmentList) {
    CoupledBodyTextContainer::refAttachmentTable.setSourceFile(
        OUTPUT_REF_ATTACHMENT_LIST_PATH);
    CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
  }
  TableContainer outputContainer(index);
  outputContainer.clearLinkStringSet();
  outputContainer.assignLinkStringSet(
      CoupledBodyTextContainer::refAttachmentTable.allAttachmentsAsLinksByType(
          type));
  outputContainer.setMaxTargetAsSetSize();
  outputContainer.createParaListFrom(18, 22);
  outputContainer.outputToBodyTextFromLinkList(attachmentUnit);
  outputContainer.setTitle(title);
  outputContainer.setDisplayTitle(displayTitle);
  outputContainer.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file "
                  << outputContainer.getoutputHtmlFilepath() << endl;
}

void generateContentTable(TABLE_TYPE type, ATTACHMENT_TYPE attType,
                          bool needToReloadAttachmentList) {
  switch (type) {
  case TABLE_TYPE::MAIN:
    generateContentTableForMainHtmls();
    break;
  case TABLE_TYPE::ORIGINAL:
    generateContentTableForOriginalHtmls();
    break;
  case TABLE_TYPE::JPM:
    generateContentTableForJPMHtmls();
    break;
  case TABLE_TYPE::MDT:
    generateContentTableForMDTHtmls();
    break;
  case TABLE_TYPE::XXJ:
    generateContentTableForXXJHtmls();
    break;
  case TABLE_TYPE::IMAGE:
    generateContentTableForImages();
    break;
  case TABLE_TYPE::POEMS:
    generateContentTableForPoems();
    break;
  case TABLE_TYPE::ATTACHMENT:
    generateContentTableForAttachments(attType, needToReloadAttachmentList);
    break;
  default:
    FUNCTION_OUTPUT << "no ContentTable generated." << endl;
  }
}
