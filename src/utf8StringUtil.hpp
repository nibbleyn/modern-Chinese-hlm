#pragma once
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <regex>
#include <set>

using namespace std;

const int LOG_EXCEPTION = 1;
const int LOG_INFO = 2;

#define TurnToInt(x) stoi(x, nullptr, 10)
#define TurnToString(x) to_string(x)

static const std::string ERROR_FILE_NOT_EXIST = R"(file doesn't exist: )";

static const string leadingChar = R"(P)";
static const string middleChar = R"(L)";
static const string invalidLineNumber = R"(P0L0)";
// top of the body Text file
static const string topParagraphIndicator = R"(top)";
// bottom of the body Text file
static const string bottomParagraphIndicator = R"(bottom)";

#define SEPERATE(x, y)                                                         \
  cout << "*************************line " << __LINE__ << ": " << x << y       \
       << "*************************" << endl; //<< __func__<< __FILE__

#define METHOD_OUTPUT                                                          \
  cout << typeid(*this).name() << "::" << __func__ << "              "
#define FUNCTION_OUTPUT cout << __func__ << "              "

static const string BODY_TEXT_SUFFIX = R"(.txt)";
static const string HTML_SUFFIX = R"(.htm)";
static const string ATTACHMENT_TYPE_HTML_TARGET = R"(b0)";

// start of each paragraph and line number
static const string lineNumberIdBeginChars = R"(id="P)";
static const string imageGroupBeginChars = R"(<div)";

static const string citationChapterNo = R"(第)";
static const string numberingUnit = R"(段)";

static const string defaultUnit = R"(回)";
static const string attachmentUnit = R"(篇)";
static const string searchUnit = R"(条)";

// start and end of each LINE
static const string brTab = "<br>";
static const string displaySpace = R"( )";
static const string emptyString = "";
static const string CR{0x0D};
static const string LF{0x0A};
static const string CRLF{0x0D, 0x0A};

static const string keyNotFound = R"(KeyNotFound)";
static const string attachmentFileMiddleChar = R"(_)";

static const string bracketStartChars = R"(（)";
static const string bracketEndChars = R"(）)";

static const string unhiddenDisplayProperty = R"(unhidden)";
static const string hiddenDisplayProperty = R"(hidden)";
static const string endOfBeginTag = R"(>)";

// links
static const string linkStartChars = R"(<a)";
static const string linkEndChars = R"(</a>)";
static const string personalCommentStartChars = R"(<u unhidden)";
static const string personalCommentEndChars = R"(</u>)";
static const string titleStartChars = R"(title=")";
static const string imageTypeChars = R"(IMAGE)";
static const string titleEndChars = R"(")"; // ! contained by titleStartChars
static const string referFileMiddleChar = R"(href=")";
static const string referParaMiddleChar = R"(#)";
static const string referParaEndChar = R"(">)";
static const string keyStartChars = R"(<i hidden>)";
static const string keyEndChars = R"(</i>)";
static const string changeKey = R"(changeKey)";
static const string citationStartChars = R"(<sub hidden>)";
static const string unhiddenCitationStartChars = R"(<sub unhidden>)";
static const string citationChapterParaSeparator = R"(.)";
static const string citationPara = R"(节:)";
static const string citationEndChars = R"(</sub>)";
static const string originalLinkStartChars = R"(（)";
static const string originalLinkEndChars = R"(）)";

// comments
static const string commentBeginChars = R"(<cite)";
static const string commentEndChars = R"(</cite>)";
static const string commentStart =
    bracketStartChars + commentBeginChars + displaySpace;
static const string commentEnd = commentEndChars + bracketEndChars;
static const string endOfCommentBeginTag = R"(>)";

static const int THREE_DIGIT_FILENAME = 3;
static const int TWO_DIGIT_FILENAME = 2;

string formatIntoZeroPatchedChapterNumber(int chapterNumber, int digits);

void replacePart(string &linkString, const string &key,
                 const string &toReplace);
int utf8length(const string &originalString);
string utf8substr(const string &originalString, size_t begin, size_t &end,
                  size_t SubStrLength);
string markDifference(const string &firstString, const string &secondString,
                      size_t begin = 0);

void printCompareResult(const string &firstString, const string &secondString,
                        size_t begin = 0);

string getIncludedStringBetweenTags(const string &originalString,
                                    const string &begin, const string &end,
                                    size_t after = 0);

string getWholeStringBetweenTags(const string &originalString,
                                 const string &begin, const string &end,
                                 size_t after = 0);

using AttachmentNumber = pair<int, int>; // chapter number, attachment number

using ParaLineNumber = pair<int, int>; // para number, line number

using LinkStringSet = map<pair<AttachmentNumber, ParaLineNumber>, string>;

string getFileNameFromAttachmentNumber(AttachmentNumber num);
AttachmentNumber getAttachmentNumber(const string &filename,
                                     bool prefixIncluded = true);

// same value with those defined in fileUtil.hpp
static const string MAIN_TYPE_HTML_TARGET = R"(a0)";
static const string ORIGINAL_TYPE_HTML_TARGET = R"(c0)";
static const string JPM_TYPE_HTML_TARGET = R"(d)";

string getChapterNameByTargetKind(const string &targetKind, int chapterNumber);

// get attachment numbers for a chapter
using AttachmentNumberList = set<int>;
AttachmentNumberList getAttachmentFileListForChapter(const string &fromDir,
                                                     int chapterNumber,
                                                     int minAttachNo = 0,
                                                     int maxAttachNo = 0);
