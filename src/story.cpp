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
  string title{""};
  if (!infile.eof())
    getline(infile, title);
  else
    return; // empty file
  FUNCTION_OUTPUT << title << endl;

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
  }
}
