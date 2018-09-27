#pragma once
#include "hlmNumbering.hpp"
#include "hlmLinkFixing.hpp"

static const string HTML_CONTAINER = "container/";
static const string BODY_TEXT_CONTAINER = "container/";
static const string defaultTitle = "XXX";
static const string defaultDisplayTitle = "YYY";
static const string personalCommentStart = R"(（<u unhidden)";
static const string personalCommentEnd = R"(</u>）)";

// using container to display set of links separated by paragraphs
void clearLinksInContainerBodyText(int containerNumber);
void appendTextInContainerBodyText(string text, int containerNumber);
void appendNumberLineInContainerBodyText(string line, int containerNumber);
void appendLinkInContainerBodyText(string linkString, int containerNumber);
void addFirstParagraphInContainerBodyText(int startNumber, int containerNumber);
void addParagraphInContainerBodyText(int startNumber, int paraNumber,
                                     int containerNumber);
void addLastParagraphInContainerBodyText(int startNumber, int paraNumber,
                                         int containerNumber);
void assembleContainerHTM(string outputHTMFilename, int containerNumber,
                          string title = "", string displayTitle = "",
                          int lastParaNumber = 0);

// remove all personal view and attachments
void removePersonalCommentsOverNumberedFiles(string referFile,
                                             FILE_TYPE fileType,
                                             int attachNo = 1);
void removePersonalViewpoints(int minTarget, int maxTarget, FILE_TYPE fileType);

using LinkRange = vector<pair<string, string>>;
// make a sub story
void reConstructStory(string indexFilename);

// utility function to provide based on above capability
void findFirstInFiles(string key, FILE_TYPE targetFileType, int minTarget,
                      int maxTarget, string outputFilename);

// main entry of calling above functions
void testFunctions();
