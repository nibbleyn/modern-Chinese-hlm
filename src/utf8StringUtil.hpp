#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
using namespace std;

const int LOG_EXCEPTION = 1;
const int LOG_INFO = 2;

#define TurnToInt(x) stoi(x, nullptr, 10)
#define TurnToString(x) to_string(x)

static const string leadingChar = R"(P)";
static const string middleChar = R"(L)";
static const string invalidLineNumber = R"(P0L0)";
static const string topParagraphIndicator =
    R"(top)"; // of the body Text file
static const string bottomParagraphIndicator =
    R"(bottom)"; // of the body Text file

#define GetTupleElement(x, y) std::get<y>(x)

#define SEPERATE(x, y)                                                         \
  cout << "*************************line " << __LINE__ << ": " << x << y       \
       << "*************************" << endl; //<< __func__<< __FILE__

static const string HTML_SRC_ATTACHMENT = "utf8HTML/src/attachment/";
static const string HTML_SUFFIX = R"(.htm)";

static const string BODY_TEXT_OUTPUT = "bodyTexts/output/";
static const string BODY_TEXT_FIX = "bodyTexts/afterFix/";
static const string BODY_TEXT_SUFFIX = R"(.txt)";

static const string htmlFileFinalLine =
    R"(</html>)"; // last line of the html file
static const string endOfHtmlHead = R"(/head)";
static const string htmlTitleStart = R"(<title>)";
static const string htmlTitleEnd = R"(</title>)";

static const string lineNumberIdBeginChars =
    R"(id="P)"; // of each paragraph and line number
static const string topIdBeginChars = R"(id="top")"; // of the body Text file
static const string bottomIdBeginChars =
    R"(id="bottom")"; // of the body Text file

static const string brTab = "<br>"; // start and end of each LINE

static const string keyNotFound = R"(KeyNotFound)";
static const string attachmentFileMiddleChar = R"(_)";

static const string bracketStartChars = R"(（)";
static const string bracketEndChars = R"(）)";

static const string unhiddenDisplayPropterty = R"( unhidden)";
static const string endOfBeginTag = R"(>)";

// links
static const string linkStartChars = R"(<a)";
static const string linkEndChars = R"(</a>)";
static const string titleStartChars = R"(title=")";
static const string titleEndChars = R"(" )";
static const string referFileMiddleChar = R"(href=")";
static const string referParaMiddleChar = R"(#)";
static const string referParaEndChar = R"(">)";
static const string keyStartChars = R"(<i hidden>)";
static const string keyEndChars = R"(</i>)";
static const string changeKey = R"(changeKey)";
static const string citationStartChars = R"(<sub hidden>)";
static const string citationChapterParaSeparator = R"(.)";
static const string citationPara = R"(节:)";
static const string citationEndChars = R"(</sub>)";
static const string charBeforeAnnotation = R"(>)";
static const string originalLinkStartChars = R"(（)";
static const string originalLinkEndChars = R"(）)";

// comments
static const string commentBeginChars = R"(<cite)";
static const string commentEndChars = R"(</cite>)";
static const string commentStart = bracketStartChars + commentBeginChars;
static const string commentEnd = commentEndChars + bracketEndChars;
static const string endOfCommentBeginTag = R"(>)";

string formatIntoZeroPatchedChapterNumber(int chapterNumber, int digits);

// operations to construct a group of file names
using fileSet = set<string>;
fileSet buildFileSet(int min, int max, int digits = 2);

void replacePart(string &linkString, const string &key,
                 const string &toReplace);
int utf8length(const string &originalString);
string utf8substr(const string &originalString, size_t begin, size_t &end,
                  size_t SubStrLength);
