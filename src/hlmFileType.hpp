#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <chrono>

#include "Poco/File.h"

#include "hlmLineNumber.hpp"

using namespace std;
extern int debug;
#define GetTupleElement(x, y) std::get<y>(x)

#define SEPERATE(x, y)                                                         \
  cout << "*************************line " << __LINE__ << ": " << x << y       \
       << "*************************" << endl; //<< __func__<< __FILE__

static const string HTML_SRC = "utf8HTML/src/";
static const string HTML_SRC_ATTACHMENT = "utf8HTML/src/attachment/";
static const string HTML_SRC_ORIGINAL = "utf8HTML/src/original/";

static const string HTML_OUTPUT = "utf8HTML/output/";
static const string HTML_OUTPUT_ATTACHMENT = "utf8HTML/output/attachment/";
static const string HTML_OUTPUT_ORIGINAL = "utf8HTML/output/original/";
static const string HTML_SUFFIX = R"(.htm)";

static const string BODY_TEXT_OUTPUT = "bodyTexts/output/";
static const string BODY_TEXT_FIX = "bodyTexts/afterFix/";
static const string BODY_TEXT_SUFFIX = R"(.txt)";

static const string topTab = R"(name="top")";       // of the body Text file
static const string bottomTab = R"(name="bottom")"; // of the body Text file
static const string keyNotFound = R"(KeyNotFound)";
static const string attachmentFileMiddleChar = R"(_)";

static const string defaultTitle = "XXX";
static const string defaultDisplayTitle = "YYY";
static const string HTML_CONTAINER = "container/container";
static const string BODY_TEXT_CONTAINER = "container/";

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL };

string formatIntoTwoDigitChapterNumber(int chapterNumber);

// operations to construct a group of file names
using fileSet = set<string>;
fileSet buildFileSet(int min, int max);

string getFileNamePrefix(FILE_TYPE type);
string getBodyTextFilePrefix(FILE_TYPE type);
string getSeparateLineColor(FILE_TYPE type);

// backup or load files before operations
void loadBodyTexts(const string &from, const string &to);
void backupAndOverwriteSrcForHTML();

// separate/merge body text from/back to Htm files
void assembleBackToHTM(const string &inputHtmlFile,
                       const string &inputBodyTextFile,
                       const string &outputFile, string title = "",
                       string displayTitle = "");
void dissembleOriginalHtmls(int minTarget, int maxTarget);
void assembleOriginalHtmls(int minTarget, int maxTarget);
void dissembleMainHtmls(int minTarget, int maxTarget);
void assembleMainHtmls(int minTarget, int maxTarget);
void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo);
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo);
// search in body text files
string findKeyInFile(const string &key, const string &fullPath,
                     string &lineNumber, bool &needChange);

string getAttachmentTitle(const string &filename);
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir);
