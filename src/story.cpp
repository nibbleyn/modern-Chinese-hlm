#include "story.hpp"

void dispalyLinkRange(LinkRange fs) {
  for (const auto &link : fs) {
    cout << link.first << "   " << link.second << endl;
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
    cout << "file doesn't exist:" << indexFilename << endl;
    return;
  }
  string title{""};
  if (!infile.eof())
    getline(infile, title);
  else
    return; // empty file
  cout << title << endl;

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
    //		  cout <<
    // startChapter<<startPara<<startLine<<endChapter<<endPara<<endLine<<
    // endl;
    //    string startLink =
    //        TurnToString(TurnToInt(startPara) + START_PARA_NUMBER - 1) +
    //        startPara + "0" + startLine;
    //    string endLink = TurnToString(TurnToInt(endPara) + START_PARA_NUMBER -
    //    1) +
    //                     endPara + "0" + endLine;
    //    //		  cout << startLink<<endLink<< endl;
    //    cs.push_back(make_pair(startChapter, endChapter));
    //    ls.push_back(make_pair(startLink, endLink));
    //    pls.push_back(
    //        make_pair(startPara + " " + startLine, endPara + " " + endLine));
  }
  dispalyLinkRange(cs);
  dispalyLinkRange(ls);
  dispalyLinkRange(pls);
}
