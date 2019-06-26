#include "story.hpp"

/**
 * give a set of start, end para pairs, create a sub-story by get them out of
 * original files
 * @param indexFilePath mapping file
 * @param outputFilename result html file
 */
void reConstructStory(const string &title, const string &outputFilename,
                      const string &kind) {
  string indexFilePath = HTML_OUTPUT_MAIN + title + BODY_TEXT_SUFFIX;
  CoupledBodyTextWithLink::loadRangeTableFromFile(indexFilePath);
  ListContainer container((outputFilename == emptyString) ? title + "_generated"
                                                          : outputFilename);
  container.clearExistingBodyText();

  // cannot change to hold quite many lines yet.
  CoupledParaHeader paraHeader;
  paraHeader.setStartNumber(LineNumber::getStartNumber());
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());

  CoupledBodyText bodyText;
  bodyText.setFilePrefixFromFileType(getFileTypeFromString(kind));
  for (const auto &element : CoupledBodyTextWithLink::rangeTable) {
    auto num = element.first.first;
    auto paraLine = element.first.second;
    auto startPara = element.second.first;
    auto endPara = element.second.second;
    FUNCTION_OUTPUT << num.first << "_" << num.second << ":P" << paraLine.first
                    << "L" << paraLine.second
                    << ":                                         P"
                    << startPara.first << "L" << startPara.second << "  ->  P"
                    << endPara.first << "L" << endPara.second << endl;
    bodyText.setFileAndAttachmentNumber(num.first, num.second);
    bodyText.setStartOfRange(startPara);
    bodyText.setEndOfRange(endPara);
    bodyText.fetchLineTexts();
    bodyText.setOutputBodyTextFilePath(container.getOutputBodyTextFilePath());
    bodyText.appendLinesIntoBodyTextFile();
  }

  paraHeader.setCurrentParaNo(1);
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());

  container.assembleBackToHTM(title, title);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}
