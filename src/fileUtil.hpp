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

#include "lineNumber.hpp"

using namespace std;
extern int debug;
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

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL };

string formatIntoTwoDigitChapterNumber(int chapterNumber);

// operations to construct a group of file names
using fileSet = set<string>;
fileSet buildFileSet(int min, int max);

string getFileNamePrefix(FILE_TYPE type);
string getBodyTextFilePrefix(FILE_TYPE type);
string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const string &fileType);

string getAttachmentTitle(const string &filename);
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir);
