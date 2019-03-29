#pragma once

//#include "lineNumber.hpp"
//#include "paraHeader.hpp"

#include "lineNumber.hpp"

class CoupledBodyText {

public:
  CoupledBodyText() = default;
  CoupledBodyText(const string &filePrefix) : m_filePrefix(filePrefix) {}
  virtual ~CoupledBodyText(){};
  /**
   * load files under BODY_TEXT_OUTPUT directory with files under BODY_TEXT_FIX
   * i.e. from afterFix to output
   * for continue link fixing after numbering..
   * BODY_TEXT_OUTPUT currently is only to output, no backup would be done for
   * it
   */
  static void loadBodyTextsFromFixBackToOutput();

  void setFilePrefixFromFileType(FILE_TYPE type) {
    if (type == FILE_TYPE::MAIN)
      m_filePrefix = MAIN_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::ATTACHMENT)
      m_filePrefix = ATTACHMENT_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::ORIGINAL)
      m_filePrefix = ORIGINAL_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::JPM)
      m_filePrefix = JPM_BODYTEXT_PREFIX;
  }

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachNumber = attachNo;
  }

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

  void getNumberOfPara();

  // add line number before each paragraph
  virtual void addLineNumber(const string &separatorColor,
                             bool forceUpdate = true,
                             bool hideParaHeader = false);

  // fix wrong html pair
  void fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                       const string &from, const string &to);
  void fixTagPairEnd(const string &signOfTagBeforeReplaceTag,
                     const string &from, const string &to,
                     const string &skipTagPairBegin = "");
  void fixPersonalView();

protected:
  string m_filePrefix{"Main"};
  string m_file{"01"};
  string m_inputFile{""};
  string m_outputFile{""};
  int m_attachNumber{0};
  lineNumberSet m_ignoreSet;
  lineNumberSet m_result;
  string m_searchError{""};
  bool m_onlyFirst{true};
  bool m_autoNumbering{false};

  // apperance of imageGroup -> counts of BR afterwards, missing a paraheader
  // afterwards
  using BrAfterImageGroupTable = std::map<size_t, pair<size_t, bool>>;
  int m_numberOfFirstParaHeader{0};
  int m_numberOfMiddleParaHeader{0};
  int m_numberOfLastParaHeader{0};
  int m_numberOfImageGroupNotIncludedInPara{0};
  BrAfterImageGroupTable m_brTable;

  void printBrAfterImageGroupTable() {
    if (not m_brTable.empty())
      METHOD_OUTPUT << "m_brTable:" << endl;
    for (const auto &element : m_brTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.first << "  "
                    << element.second.second << endl;
    }
  }

  void setInputOutputFiles();
  void removeNbspsAndSpaces(string &inLine);
  void removeOldLineNumber(string &inLine);
  bool isImageGroupLine(const string &inLine) {
    return (inLine.find(imageGroupBeginChars) != string::npos);
  }
  bool isEmptyLine(const string &inLine) {
    return (inLine == "") or (inLine == "\r") or (inLine == "\n") or
           (inLine == "\r\n");
  }
  bool isLeadingBr(const string &inLine) {
    return (inLine == brTab) or (inLine == brTab + "\r") or
           (inLine == brTab + "\n") or (inLine == brTab + "\r\n");
  };
  bool hasEndingBr(const string &inLine) {
    return (inLine.find(brTab) != string::npos and not isLeadingBr(inLine));
  };
};

bool isFoundAsNonKeys(const string &line, const string &key);
bool isFoundOutsidePersonalComments(const string &line, const string &key);
