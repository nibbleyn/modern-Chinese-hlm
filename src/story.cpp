#include "story.hpp"

void dispalyLinkRange(LinkRange fs) {
  for (const auto &link : fs) {
    FUNCTION_OUTPUT << link.first << "   " << link.second << endl;
  }
}

/**
 * give a set of links, create a sub-story by following these links
 * @param filename
 */
void reConstructStory(string indexFilename) {
  LinkRange cs;  // startChapter, endChapter
  LinkRange ls;  // startLine, endLine
  LinkRange pls; // startPara startLine, endPara endLine
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
  }
  dispalyLinkRange(cs);
  dispalyLinkRange(ls);
  dispalyLinkRange(pls);
}
