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

static const string leadingChar = R"(P)";
static const string middleChar = R"(L)";
static constexpr const char *invalidLineNumber = R"(P0L0)";
// top of the body Text file
static const string topParagraphIndicator = R"(top)";
// bottom of the body Text file
static const string bottomParagraphIndicator = R"(bottom)";

#define METHOD_OUTPUT                                                          \
  cout << typeid(*this).name() << "::" << __func__ << "              "
#define FUNCTION_OUTPUT cout << __func__ << "              "

static const string BODY_TEXT_SUFFIX = R"(.txt)";
static const string HTML_SUFFIX = R"(.htm)";

static const string citationChapterNo = R"(第)";
static const string citationPara = R"(节:)";
static const string citationChapterParaSeparator = R"(.)";

static constexpr const char *defaultUnit = R"(回)";
static constexpr const char *attachmentUnit = R"(篇)";
static constexpr const char *searchUnit = R"(条)";

// start and end of each LINE
static const string brTab = "<br>";
static const string displaySpace = R"( )";
static constexpr const char *emptyString = "";
static const string endOfBeginTag = R"(>)";
static const string CR{0x0D};
static const string LF{0x0A};
static const string CRLF{0x0D, 0x0A};
static const string FieldSeparator = R"($$)";

static const string keyNotFound = R"(KeyNotFound)";
static const string attachmentFileMiddleChar = R"(_)";

static const string bracketStartChars = R"(（)";
static const string bracketEndChars = R"(）)";
static const string textSeparator = R"(，)";

static const string unhiddenDisplayProperty = R"(unhidden)";
static const string hiddenDisplayProperty = R"(hidden)";

static const int THREE_DIGIT_FILENAME = 3;
static const int TWO_DIGIT_FILENAME = 2;
using FileSet = set<string>;
string currentDateTime();
string formatIntoZeroPatchedChapterNumber(int chapterNumber, int digits);

void replacePart(string &targetString, const string &subStrToReplace,
                 const string &withStr);
int utf8length(const string &originalString);
string utf8substr(const string &originalString, size_t begin, size_t &end,
                  size_t SubStrLength);

string getIncludedStringBetweenTags(const string &originalString,
                                    const string &begin, const string &end,
                                    size_t after = 0);

string getWholeStringBetweenTags(const string &originalString,
                                 const string &begin, const string &end,
                                 size_t after = 0);

// same value with those defined in fileUtil.hpp
static const string MAIN_TYPE_HTML_TARGET = R"(a0)";
static const string ATTACHMENT_TYPE_HTML_TARGET = R"(b0)";
static const string ORIGINAL_TYPE_HTML_TARGET = R"(c0)";
static const string JPM_TYPE_HTML_TARGET = R"(d)";

string getChapterNameByTargetKind(const string &targetKind, int chapterNumber);

using AttachmentNumber = pair<int, int>; // chapter number, attachment number

using ParaLineNumber = pair<int, int>; // para number, line number

using LinkStringSet = map<pair<AttachmentNumber, ParaLineNumber>, string>;

string getFileNameFromAttachmentNumber(const string &targetKind,
                                       AttachmentNumber num);
AttachmentNumber getAttachmentNumber(const string &filename,
                                     const string &prefix = emptyString);
// get attachment numbers for a chapter
using AttachmentNumberList = set<int>;
AttachmentNumberList getAttachmentFileListForChapter(const string &fromDir,
                                                     int chapterNumber,
                                                     int minAttachNo = 0,
                                                     int maxAttachNo = 0);
