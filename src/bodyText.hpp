#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

class BodyText {

public:
  BodyText() = default;
  BodyText(const string &filePrefix) : filePrefix(filePrefix) {}
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

  void setFilePrefixFromFileType(FILE_TYPE type);
  using lineNumberSet = set<string>;
  // set options before search
  void resetBeforeSearch() {
    ignoreSet.clear();
    result.clear();
    searchError = "";
  }
  void addIgnoreLines(const string &line) { ignoreSet.insert(line); }
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

  // reformat to smaller paragraphs
  void reformatParagraphToSmallerSize(const string &sampleBlock,
                                      const string &file);

  // regrouping to make total size smaller
  void regroupingParagraphs(const string &sampleBlock,
                            const string &sampleFirstLine,
                            const string &sampleWholeLine, const string &file);

  using ParaStruct = std::tuple<int, int, int>;
  ParaStruct getNumberOfPara(const string &file, int attachNo = 0);

  // add line number before each paragraph
  void addLineNumber(const string &separatorColor, const string &file,
                     int attachNo = 0, bool hidden = false);

protected:
  string filePrefix{"Main"};
  lineNumberSet ignoreSet;
  lineNumberSet result;
  string searchError{""};
  bool onlyFirst{true};
  bool autoNumbering{false};
};

bool isOnlyPartOfOtherKeys(const string &orgLine, const string &key);
