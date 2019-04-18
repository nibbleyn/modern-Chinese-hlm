#pragma once

//#include "lineNumber.hpp"
//#include "paraHeader.hpp"

#include "lineNumber.hpp"

enum class DISPLY_LINE_TYPE { EMPTY, PARA, TEXT, IMAGE };
string getDisplayTypeString(DISPLY_LINE_TYPE type);
static const LineNumber BEGIN_OF_WHOLE_BODYTEXT = LineNumber(1,1);
static const LineNumber END_OF_WHOLE_BODYTEXT = LineNumber();

class CoupledBodyText {

public:
  CoupledBodyText() = default;
  CoupledBodyText(const string &filePrefix) : m_filePrefix(filePrefix) {}
  virtual ~CoupledBodyText(){};

  // used for configuring
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

  bool isMainBodyText() { return m_filePrefix == MAIN_BODYTEXT_PREFIX; }

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachNumber = attachNo;
  }

  // used for numbering
  void validateFormatForNumbering();

  // used for searching
  using lineNumberSet = set<string>;
  // set options before search
  void resetBeforeSearch() {
    m_ignoreSet.clear();
    m_result.clear();
    m_searchError = "";
  }
  void addIgnoreLines(const string &line) { m_ignoreSet.insert(line); }
  void searchForAll() { m_onlyFirst = false; }

  void ignorePersonalComments() { m_ignorePersonalComments = true; }

  bool findKey(const string &key);

  // get search results
  string getFirstResultLine() {
    if (not m_result.empty())
      return *(m_result.begin());
    return "";
  }
  lineNumberSet getResultLineSet() { return m_result; };

  // fetch lines by range
  using lineNumberSetByRange = pair<LineNumber, LineNumber>;

  void setStartOfRange(const LineNumber &ln) { m_range.first = ln; }
  void setEndOfRange(const LineNumber &ln) { m_range.second = ln; }

  // lineNumber -> text of that line
  using lineSet = map<string, string>;
  void fetchLineTexts();

  // used by tools
  // fix wrong html pair
  void fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                       const string &from, const string &to);
  void fixTagPairEnd(const string &signOfTagBeforeReplaceTag,
                     const string &from, const string &to,
                     const string &skipTagPairBegin = "");
  void fixPersonalView();
  // reformat to smaller paragraphs
  void reformatParagraphToSmallerSize(const string &sampleBlock);

protected:
  // used for configuring
  string m_filePrefix{"Main"};
  string m_file{"01"};
  int m_attachNumber{0};
  string m_inputFile{""};
  string m_outputFile{""};
  void setInputOutputFiles();

  bool isAutoNumbering() { return m_autoNumbering; }
  bool m_autoNumbering{true};

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

  // used for numbering
  string m_inLine{};
  size_t m_para{0};
  size_t m_lineNo{1}; // LINE index within each group
  ParaHeader m_paraHeader;

  struct LineInfo {
    size_t numberOfLines{0};
    DISPLY_LINE_TYPE type{DISPLY_LINE_TYPE::EMPTY};
    string cap{""};
  };
  // line No. -> number of display lines, line type
  using LineAttrTable = std::map<size_t, LineInfo>;
  LineAttrTable m_lineAttrTable;
  size_t m_lastSeqNumberOfLine{0};

  bool isInLineAttrTable(size_t seqOfLines) {
    try {
      m_lineAttrTable.at(seqOfLines);
      return true;
    } catch (exception &) {
      // std::out_of_range if not existed
      return false;
    }
  }

  size_t findEarlierLineInLineAttrTable(size_t seqOfLines) {
    size_t result = seqOfLines - 1;
    while (true) {
      if (result == 0 or isInLineAttrTable(result))
        break;
      result--;
    }
    return result;
  }

  void printLineAttrTable() {
    if (not m_lineAttrTable.empty()) {
      METHOD_OUTPUT << "m_lineAttrTable:" << endl;
      METHOD_OUTPUT << "line No/numberOfLines/type/summary" << endl;
    } else
      METHOD_OUTPUT << "no entry in m_lineAttrTable." << endl;

    for (const auto &element : m_lineAttrTable) {
      METHOD_OUTPUT << element.first << "        "
                    << element.second.numberOfLines << "          "
                    << getDisplayTypeString(element.second.type) << "  "
                    << element.second.cap << endl;
    }
  }

  // line No.of image group -> line No. before following para header to add
  using ImgGroupFollowingParaTable = std::map<size_t, size_t>;
  ImgGroupFollowingParaTable m_imgGroupFollowingParaTable;

  bool isInImgGroupFollowingParaTable(size_t seqOfLines) {
    try {
      m_imgGroupFollowingParaTable.at(seqOfLines);
      return true;
    } catch (exception &) {
      // std::out_of_range if not existed
      return false;
    }
  }

  void printImgGroupFollowingParaTable() {
    if (not m_imgGroupFollowingParaTable.empty()) {
      METHOD_OUTPUT << "m_imgGroupFollowingParaTable:" << endl;
      METHOD_OUTPUT << "img Group line No/line No to add para below" << endl;
    } else
      METHOD_OUTPUT << "no entry in m_imgGroupFollowingParaTable." << endl;
    for (const auto &element : m_imgGroupFollowingParaTable) {
      METHOD_OUTPUT << element.first << "        " << element.second << endl;
    }
  }

  void removeNbspsAndSpaces();
  void removeOldLineNumber();

  void numberingLine(ofstream &outfile, bool forceUpdate = true,
                     bool hideParaHeader = false);

  void addFirstParaHeader(ofstream &outfile);
  void addlastParaHeader(ofstream &outfile);
  void addMiddleParaHeader(ofstream &outfile, bool enterLastPara);
  void addParaHeader(ofstream &outfile);

  size_t m_numberOfFirstParaHeader{0};
  size_t m_numberOfMiddleParaHeader{0};
  size_t m_numberOfLastParaHeader{0};

  void scanByLines();
  void paraGuidedNumbering(bool forceUpdate, bool hideParaHeader);

  // used for searching
  bool m_ignorePersonalComments{false};
  lineNumberSet m_ignoreSet;
  lineNumberSet m_result;
  string m_searchError{""};
  bool m_onlyFirst{true};

  lineNumberSetByRange m_range;
  lineSet m_resultLines;
};

bool isFoundAsNonKeys(const string &line, const string &key);
bool isFoundOutsidePersonalComments(const string &line, const string &key);
