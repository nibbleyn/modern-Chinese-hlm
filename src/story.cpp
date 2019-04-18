#include "story.hpp"

/**
 * give a set of links, create a sub-story by following these links
 * @param filename
 */
void reConstructStory(const string &indexFilename,
                      const string &outputFilename) {
  ListContainer container(outputFilename);
  ifstream infile(indexFilename);
  if (!infile) // doesn't exist
  {
    FUNCTION_OUTPUT << "file doesn't exist:" << indexFilename << endl;
    return;
  }
  container.clearExistingBodyText();

  // cannot change to hold quite many lines yet.
  ParaHeader paraHeader;
  paraHeader.m_startNumber = LineNumber::getStartNumber();
  paraHeader.m_color = getSeparateLineColor(FILE_TYPE::MAIN);
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());

  string title{""};
  if (!infile.eof())
    getline(infile, title);
  else
    return; // empty file
  FUNCTION_OUTPUT << title << endl;
  FILE_TYPE targetFileType = getFileTypeFromString("main");
  while (!infile.eof()) // To get you all the lines.
  {
    string startChapter, startPara, startLine;
    getline(infile, startChapter, '#');
    getline(infile, startPara, '.');
    getline(infile, startLine, ' ');
    string endChapter, endPara, endLine;
    getline(infile, endChapter, '#');
    getline(infile, endPara, '.');
    getline(infile, endLine, '\n');
    FUNCTION_OUTPUT << startChapter << startPara << startLine << endl;
    FUNCTION_OUTPUT << endChapter << endPara << endLine << endl;
    // fetch lines from specified chapter
    CoupledBodyText bodyText;
    bodyText.setFilePrefixFromFileType(targetFileType);
    // assume startChapter is same as endChapter
    bodyText.setFileAndAttachmentNumber(startChapter);
    bodyText.setStartOfRange(
        LineNumber(TurnToInt(startPara), TurnToInt(startLine)));
    bodyText.setEndOfRange(LineNumber(TurnToInt(endPara), TurnToInt(endLine)));
    bodyText.fetchLineTexts();
    bodyText.setOutputBodyTextFilePath(container.getOutputBodyTextFilePath());
    bodyText.appendLinesIntoBodyTextFile();
  }
  paraHeader.m_currentParaNo = 1;
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());

  container.assembleBackToHTM(title, title);
  FUNCTION_OUTPUT << "result is in file " << container.getOutputFilePath()
                  << endl;
}
