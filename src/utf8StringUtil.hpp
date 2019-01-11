#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

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

static const string topTab = R"(name="top")";       // of the body Text file
static const string bottomTab = R"(name="bottom")"; // of the body Text file
static const string keyNotFound = R"(KeyNotFound)";
static const string attachmentFileMiddleChar = R"(_)";

static const string commentBeginChars = R"(<i unhidden)";
static const string keyStartChars = R"(<i hidden>)";
static const string keyEndChars = R"(</i>)";

string formatIntoTwoDigitChapterNumber(int chapterNumber);

// operations to construct a group of file names
using fileSet = set<string>;
fileSet buildFileSet(int min, int max);

string replacePart(string &linkString, const string &key,
                   const string &toReplace);
int utf8length(std::string originalString);
std::string utf8substr(std::string originalString, size_t begin, size_t &end,
                       size_t SubStrLength);
