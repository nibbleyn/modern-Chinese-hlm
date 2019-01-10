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

class BodyText {

public:
  BodyText(string filePrefix) : filePrefix(filePrefix) {}
  /**
   * load files under BODY_TEXT_OUTPUT directory with files under BODY_TEXT_FIX
   * i.e. from afterFix to output
   * for continue link fixing after numbering..
   * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for
   * it
   */
  static void loadBodyTextsFromFixBackToOutput() {
    vector<string> filenameList;
    Poco::File(BODY_TEXT_FIX).list(filenameList);
    sort(filenameList.begin(), filenameList.end(), less<string>());
    for (const auto &file : filenameList) {
      if (debug >= LOG_INFO)
        cout << "loading " << file << ".. " << endl;
      Poco::File fileToCopy(BODY_TEXT_FIX + file);
      fileToCopy.copyTo(BODY_TEXT_OUTPUT + file);
    }
  }
  using lineNumberSet = set<string>;
  // set options before search
  void resetBeforeSearch() {
    ignoreSet.clear();
    result.clear();
    searchError = "";
  }
  void addIgnoreLines(const string &line);
  void searchForAll() { onlyFirst = false; }

  // search
  bool findKey(const string &key, const string &file, int attachNo = 0);

  // get search results
  string getFirstResultLine() {
    if (not result.empty())
      return *(result.begin());
    return "";
  }
  lineNumberSet getResultLineSet() { return result; };

private:
  string filePrefix{"Main"};
  lineNumberSet ignoreSet;
  lineNumberSet result;
  string searchError{""};
  bool onlyFirst{true};
};

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

bool isOnlyPartOfOtherKeys(const string &orgLine, const string &key);

int utf8length(std::string originalString);
std::string utf8substr(std::string originalString, size_t begin, size_t &end,
                       size_t SubStrLength);
