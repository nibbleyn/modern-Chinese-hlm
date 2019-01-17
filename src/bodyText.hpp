#pragma once
#include "fileUtil.hpp"
//#include "utf8StringUtil.hpp"
#include "lineNumber.hpp"
#include "paraHeader.hpp"

class BodyText {

public:
  BodyText() = default;
  BodyText(const string &filePrefix) : m_filePrefix(filePrefix) {}
  /**
   * load files under BODY_TEXT_OUTPUT directory with files under BODY_TEXT_FIX
   * i.e. from afterFix to output
   * for continue link fixing after numbering..
   * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for
   * it
   */
  static void loadBodyTextsFromFixBackToOutput();

  void setFilePrefixFromFileType(FILE_TYPE type);
  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachNumber = attachNo;
  };
  using lineNumberSet = set<string>;
  // set options before search
  void resetBeforeSearch() {
    m_ignoreSet.clear();
    m_result.clear();
    m_searchError = "";
  }
  void addIgnoreLines(const string &line) { m_ignoreSet.insert(line); }
  void searchForAll() { m_onlyFirst = false; }

  // search
  bool findKey(const string &key);

  // get search results
  string getFirstResultLine() {
    if (not m_result.empty())
      return *(m_result.begin());
    return "";
  }
  lineNumberSet getResultLineSet() { return m_result; };

  // reformat to smaller paragraphs
  void reformatParagraphToSmallerSize(const string &sampleBlock);

  // regrouping to make total size smaller
  void regroupingParagraphs(const string &sampleBlock,
                            const string &sampleFirstLine,
                            const string &sampleWholeLine);

  using ParaStruct = std::tuple<int, int, int>;
  ParaStruct getNumberOfPara();

  // add line number before each paragraph
  void addLineNumber(const string &separatorColor, bool hidden = false);

  // fix wrong html pair
  void fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                       const string &fromTagBegin, const string &fromTagEnd,
                       const string &to);
  void fixTagPairEnd(const string &signOfTagBeforeReplaceTag,
                     const string &from, const string &to,
                     const string &skipTagPairBegin = "",
                     const string &skipTagPairEnd = "");

protected:
  string m_filePrefix{"Main"};
  string m_file{"01"};
  int m_attachNumber{0};
  lineNumberSet m_ignoreSet;
  lineNumberSet m_result;
  string m_searchError{""};
  bool m_onlyFirst{true};
  bool m_autoNumbering{false};
};

void testSearchTextIsOnlyPartOfOtherKeys();
void testLineNumber();
void testConstructSubStory();
