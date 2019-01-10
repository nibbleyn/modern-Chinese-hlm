#include "numbering.hpp"

/**
 * numbering lines a set of Original body text files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void addLineNumbers(int minTarget, int maxTarget, FILE_TYPE targetFileType,
                    bool hidden = false) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string separatorColor = getSeparateLineColor(targetFileType);
    BodyText bodyText(getBodyTextFilePrefix(targetFileType));
    bodyText.addLineNumber(separatorColor, file, hidden);
  }
}

/**
 * Numbering a set of original Html files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void NumberingOriginalHtml(int minTarget, int maxTarget, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::ORIGINAL);
  container.backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.dissembleFromHTM(file);
  }
  addLineNumbers(minTarget, maxTarget, FILE_TYPE::ORIGINAL, hidden);
  BodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
}

/**
 * Numbering a set of Main Html files
 * @param minTarget starting from this file
 * @param maxTarget until this file
 */
void NumberingMainHtml(int minTarget, int maxTarget, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::MAIN);
  container.backupAndOverwriteAllInputHtmlFiles();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.dissembleFromHTM(file);
  }
  addLineNumbers(minTarget, maxTarget, FILE_TYPE::MAIN, hidden);
  BodyText::loadBodyTextsFromFixBackToOutput();
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    container.assembleBackToHTM(file);
  }
  cout << "Numbering Main Html finished. " << endl;
}

/**
 * numbering lines of a set of Attachment body text files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * Numbering all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void addLineNumbersForAttachmentHtml(int minTarget, int maxTarget,
                                     int minAttachNo, int maxAttachNo,
                                     bool hidden = false) {

  vector<int> targetAttachments;
  bool overAllAttachments = true;
  if (not(minAttachNo == 0 and maxAttachNo == 0) and
      minAttachNo <= maxAttachNo) {
    for (int i = maxAttachNo; i >= minAttachNo; i--)
      targetAttachments.push_back(i);
    overAllAttachments = false;
  }
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    if (overAllAttachments == true)
      targetAttachments =
          getAttachmentFileListForChapter(file, HTML_SRC_ATTACHMENT);
    for (const auto &attNo : targetAttachments) {
      FILE_TYPE targetFileType = FILE_TYPE::ATTACHMENT;
      string separatorColor = getSeparateLineColor(targetFileType);
      BodyText bodyText(getBodyTextFilePrefix(targetFileType));
      bodyText.addLineNumber(separatorColor, file, attNo, hidden);
    }
  }
}

/**
 * Numbering a set of Attachment Html files of a set of chapters
 * if minAttachNo>maxAttachNo or both are zero
 * Numbering all attachments for those chapters
 * @param minTarget starting from this chapter
 * @param maxTarget until this chapter
 * @param minAttachNo for each chapter, start from this attachment
 * @param maxAttachNo for each chapter, until this attachment
 */
void NumberingAttachmentHtml(int minTarget, int maxTarget, int minAttachNo,
                             int maxAttachNo, bool hidden = false) {
  CoupledContainer container(FILE_TYPE::ATTACHMENT);
  container.backupAndOverwriteAllInputHtmlFiles();
  dissembleAttachments(minTarget, maxTarget, minAttachNo,
                       maxAttachNo); // dissemble html to bodytext
  addLineNumbersForAttachmentHtml(
      minTarget, maxTarget, minAttachNo, maxAttachNo,
      hidden); // reformat bodytext by adding line number
  BodyText::loadBodyTextsFromFixBackToOutput();
  assembleAttachments(minTarget, maxTarget, minAttachNo, maxAttachNo);
}

/**
 * copy main files into HTML_OUTPUT
 * before run this
 */
void numberMainHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  NumberingMainHtml(minTarget, maxTarget, hidden);
}
/**
 * copy original files into HTML_SRC_ORIGINAL
 * before run this
 */
void numberOriginalHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  NumberingOriginalHtml(minTarget, maxTarget, hidden);
  cout << "Numbering Original Html finished. " << endl;
}

/**
 * copy attachment files into HTML_SRC_ATTACHMENT
 * before run this
 */
void numberAttachmentHtmls(bool hidden) {
  int minTarget = 1, maxTarget = 80;
  int minAttachNo = 0, maxAttachNo = 50;
  NumberingAttachmentHtml(minTarget, maxTarget, minAttachNo, maxAttachNo,
                          hidden);
  cout << "Numbering Attachment Html finished. " << endl;
}

/**
 *
 */
void reformatTxtFiles(int minTarget, int maxTarget, const string &example) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string filename = file;
    if (filename.length() == 2) {
      filename = "0" + filename;
    }
    BodyText bodyText("pjpm");
    bodyText.reformatParagraphToSmallerSize(example, filename);
  }
}

/**
 *
 */
void reformatTxtFilesForReader() {
  const string example =
      R"(话说安童领着书信，辞了黄通判，径往山东大道而来。打听巡按御史在东昌府住扎，姓曾，双名孝序，【夹批：曾者，争也。序即天叙有典之叙，盖作者为世所厄不能自全其孝，故抑郁愤懑)";

  int minTarget = 1, maxTarget = 100;
  reformatTxtFiles(minTarget, maxTarget, example);
  cout << "reformat files from " << minTarget << " to " << maxTarget
       << " finished. " << endl;
}

void splitFiles(int minTarget, int maxTarget, const string &fileType,
                const string &sampleBlock, const string &sampleFirstLine,
                const string &sampleWholeLine) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    BodyText bodyText(getBodyTextFilePrefix(getFileTypeFromString(fileType)));
    bodyText.regroupingParagraphs(sampleBlock, sampleFirstLine, sampleWholeLine,
                                  file);
  }
}

void autoSplitBodyText(const string &fileType) {
  const string sampleBlock = R"()";
  const string sampleFirstLine = R"()";
  const string sampleWholeLine = R"()";
  int minTarget = 1, maxTarget = 1;
  splitFiles(minTarget, maxTarget, fileType, sampleBlock, sampleFirstLine,
             sampleWholeLine);
}
