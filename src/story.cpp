#include "story.hpp"

/**
 * give a set of start, end para pairs in index file named after title
 * parameter, create a sub-story by get them out of original files
 * if autoNumbering is set to false, then autoNumbering won't happen
 * after manually change result html file, could call autoNumberingResultStory
 * on it then
 */
void reConstructStory(const string &title, const string &outputFilename,
                      const string &kind, bool autoNumbering) {
  string indexFilePath = HTML_OUTPUT_MAIN + title + BODY_TEXT_SUFFIX;
  CoupledBodyTextWithLink::loadRangeTableFromFile(indexFilePath);
  ListContainer container((outputFilename == emptyString) ? title + GENERATED
                                                          : outputFilename);
  container.clearExistingBodyText();

  // cannot change to hold quite many lines yet.
  CoupledParaHeader paraHeader;
  paraHeader.setStartNumber(LineNumber::getStartNumber());
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());

  CoupledBodyText bodyText;
  bodyText.setFilePrefixFromFileType(getFileTypeFromKind(kind));
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
    bodyText.setOutputBodyTextFilePath(container.getBodyTextFilePath());
    bodyText.appendLinesIntoBodyTextFile();
  }

  paraHeader.setCurrentParaNo(1);
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());
  container.setTitle(title);
  container.setDisplayTitle(title);
  container.assembleBackToHTM();
  if (autoNumbering)
    autoNumberingResultStory((outputFilename == emptyString) ? title + GENERATED
                                                             : outputFilename,
                             title);
  else
    FUNCTION_OUTPUT << "result is in file " << container.getoutputHtmlFilepath()
                    << endl;
}

void autoNumberingResultStory(const string &htmlFilename,
                              const string &outputHtmlFilename) {
  FUNCTION_OUTPUT << "autoNumbering: " << htmlFilename << endl;
  FormatedBodyTextContainer container((outputHtmlFilename == emptyString)
                                          ? htmlFilename + NUMBERED
                                          : outputHtmlFilename);
  container.setInputHtmlFilename(htmlFilename);
  container.numbering();
  FUNCTION_OUTPUT << "result is in file " << container.getoutputHtmlFilepath()
                  << endl;
}
