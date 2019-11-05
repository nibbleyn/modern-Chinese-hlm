#pragma once

#include "lineNumber.hpp"
#include "paraHeader.hpp"

enum class DISPLY_LINE_TYPE { EMPTY, PARA, TEXT, IMAGE };
static const string DISPLY_LINE_EMPTY = R"(empty)";
static const string DISPLY_LINE_PARA = R"(para)";
static const string DISPLY_LINE_TEXT = R"(text)";
static const string DISPLY_LINE_IMAGE = R"(image)";
static const string DISPLY_LINE_BAD = R"(bad)";

static const string imageGroupBeginChars = R"(<div)";

static const LineNumber BEGIN_OF_WHOLE_BODYTEXT = LineNumber(1, 1);
static const LineNumber END_OF_WHOLE_BODYTEXT = LineNumber();

class CoupledBodyText {
public:
  static void loadBodyTextsFromFixBackToOutput();

public:
  CoupledBodyText() = default;
  CoupledBodyText(const string &filePrefix) : m_filePrefix(filePrefix) {}
  virtual ~CoupledBodyText(){};

  void setFilePrefixFromFileType(FILE_TYPE type) {
    if (type == FILE_TYPE::MAIN)
      m_filePrefix = MAIN_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::ATTACHMENT)
      m_filePrefix = ATTACHMENT_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::ORIGINAL)
      m_filePrefix = ORIGINAL_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::JPM)
      m_filePrefix = JPM_BODYTEXT_PREFIX;
    if (type == FILE_TYPE::MDT)
      m_filePrefix = MDT_BODYTEXT_PREFIX;
  }

  bool isMainBodyText() { return m_filePrefix == MAIN_BODYTEXT_PREFIX; }
  bool isOrginialBodyText() { return m_filePrefix == ORIGINAL_BODYTEXT_PREFIX; }

  void setInputBodyTextFilePath(const string &absolutePath) {
    m_inputFile = absolutePath;
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "input file is: " << m_inputFile << endl;
    }
  }
  void setOutputBodyTextFilePath(const string &absolutePath) {
    m_outputFile = absolutePath;
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "output file is: " << m_outputFile << endl;
    }
  }
  string getOutputBodyTextFilePath() { return m_outputFile; }

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachNumber = attachNo;
    setInputOutputFiles();
  }

  void setFileAndAttachmentNumber(int chapterNumber, int attachNo = 0) {

    if (m_filePrefix == JPM_BODYTEXT_PREFIX)
      m_file = getChapterNameByTargetKind(JPM_TYPE_HTML_TARGET, chapterNumber);
    else if (m_filePrefix == MAIN_BODYTEXT_PREFIX or
             m_filePrefix == ATTACHMENT_BODYTEXT_PREFIX)
      m_file = getChapterNameByTargetKind(MAIN_TYPE_HTML_TARGET, chapterNumber);
    else if (m_filePrefix == ORIGINAL_BODYTEXT_PREFIX)
      m_file =
          getChapterNameByTargetKind(ORIGINAL_TYPE_HTML_TARGET, chapterNumber);
    m_attachNumber = attachNo;
    setInputOutputFiles();
  }

  AttachmentNumber getFileAndAttachmentNumber() {
    return AttachmentNumber(TurnToInt(m_file), m_attachNumber);
  }

  // used for numbering
  void validateFormatForNumbering();

  // used for searching
  using lineNumberSet = set<string>;
  // set options before search
  void resetBeforeSearch() {
    m_ignoreSet.clear();
    m_result.clear();
    m_searchError = emptyString;
  }
  void addIgnoreLines(const string &line) { m_ignoreSet.insert(line); }
  void searchForAll() { m_onlyFirst = false; }

  void ignorePersonalComments() { m_ignorePersonalComments = true; }

  bool findKey(const string &key);

  // get search results
  string getFirstResultLine() {
    if (not m_result.empty())
      return *(m_result.begin());
    return emptyString;
  }
  lineNumberSet getResultLineSet() { return m_result; };

  // fetch lines by range
  using lineNumberSetByRange = pair<ParaLineNumber, ParaLineNumber>;

  void setStartOfRange(const ParaLineNumber &ln) { m_range.first = ln; }
  void setEndOfRange(const ParaLineNumber &ln) { m_range.second = ln; }

  // lineNumber -> text of that line
  using lineSet = map<ParaLineNumber, string>;
  void fetchLineTexts();
  void appendLinesIntoBodyTextFile();

  // used by tools
  // fix wrong html pair
  void fixTagPairBegin(const string &signOfTagAfterReplaceTag,
                       const string &from, const string &to);
  void fixTagPairEnd(const string &signOfTagBeforeReplaceTag,
                     const string &from, const string &to,
                     const string &skipTagPairBegin = emptyString);
  void fixPersonalView();
  // reformat to smaller paragraphs
  void reformatParagraphToSmallerSize(const string &sampleBlock);

  void enablePostProcessLine() { m_postProcessLine = true; }

protected:
  // used for configuring
  string m_filePrefix{MAIN_BODYTEXT_PREFIX};
  string m_inputFile{emptyString};
  string m_outputFile{emptyString};

  string m_file{emptyString};
  int m_attachNumber{0};
  void setInputOutputFiles();

  bool isAutoNumbering() { return m_autoNumbering; }
  bool isNumberingStatistics() { return m_numberingStatistics; }
  bool m_autoNumbering{true};
  bool m_numberingStatistics{true};
  bool m_disableNumberingStatisticsCalculateLines{false};

  bool isImageGroupLine(const string &inLine) {
    return (inLine.find(imageGroupBeginChars) != string::npos);
  }
  bool isEmptyLine(const string &inLine) {
    return (inLine == emptyString) or (inLine == CR) or (inLine == LF) or
           (inLine == CRLF);
  }
  bool isLeadingBr(const string &inLine) {
    return (inLine == brTab) or (inLine == brTab + CR) or
           (inLine == brTab + LF) or (inLine == brTab + CRLF);
  };
  bool hasEndingBr(const string &inLine) {
    return (inLine.find(brTab) != string::npos and not isLeadingBr(inLine));
  };
  bool isMixedOfSpaceBrackets(const string &inLine) {
    string toChange = inLine;
    while (true) {
      if (toChange.find(bracketStartChars) == string::npos)
        break;
      toChange.replace(toChange.find(bracketStartChars),
                       bracketStartChars.length(), emptyString);
    }
    while (true) {
      if (toChange.find(bracketEndChars) == string::npos)
        break;
      toChange.replace(toChange.find(bracketEndChars), bracketEndChars.length(),
                       emptyString);
    }
    toChange.erase(remove(toChange.begin(), toChange.end(), ' '),
                   toChange.end());
    return (toChange == emptyString);
  };

  // used for numbering
  string m_inLine{};
  size_t m_para{0};
  // LINE index within each group
  size_t m_lineNo{1};
  CoupledParaHeader m_paraHeader;

  struct LineInfo {
    size_t numberOfLines{0};
    DISPLY_LINE_TYPE type{DISPLY_LINE_TYPE::EMPTY};
    string cap{emptyString};
  };
  // line No. -> number of display lines, line type
  using LineAttrTable = map<size_t, LineInfo>;
  LineAttrTable m_lineAttrTable;
  size_t m_lastSeqNumberOfLine{0};

  bool isInLineAttrTable(size_t seqOfLines) {
    try {
      m_lineAttrTable.at(seqOfLines);
      return true;
    } catch (exception &) {
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

  string getDisplayTypeString(DISPLY_LINE_TYPE type) {
    if (type == DISPLY_LINE_TYPE::EMPTY)
      return DISPLY_LINE_EMPTY;
    if (type == DISPLY_LINE_TYPE::PARA)
      return DISPLY_LINE_PARA;
    if (type == DISPLY_LINE_TYPE::TEXT)
      return DISPLY_LINE_TEXT;
    if (type == DISPLY_LINE_TYPE::IMAGE)
      return DISPLY_LINE_IMAGE;
    return DISPLY_LINE_BAD;
  }

  // line No.of image group -> line No. before following para header to add
  using ImgGroupFollowingParaTable = map<size_t, size_t>;
  ImgGroupFollowingParaTable m_imgGroupFollowingParaTable;

  bool isInImgGroupFollowingParaTable(size_t seqOfLines) {
    try {
      m_imgGroupFollowingParaTable.at(seqOfLines);
      return true;
    } catch (exception &) {
      return false;
    }
  }

  void removeNbspsAndSpaces();
  void removeOldLineNumber();

  void numberingLine(ofstream &outfile);

  void addFirstParaHeader(ofstream &outfile);
  void addlastParaHeader(ofstream &outfile);
  void addMiddleParaHeader(ofstream &outfile, bool enterLastPara);
  void addParaHeader(ofstream &outfile);

  size_t m_numberOfFirstParaHeader{0};
  size_t m_numberOfMiddleParaHeader{0};
  size_t m_numberOfLastParaHeader{0};
  bool m_hideParaHeader{false};
  bool m_postProcessLine{false};
  bool m_forceUpdateLineNumber{false};

  string postProcessLine(const string &originalString);

  void scanByLines();
  void paraGuidedNumbering();

  // used for searching
  bool m_ignorePersonalComments{false};
  lineNumberSet m_ignoreSet;
  lineNumberSet m_result;
  string m_searchError{emptyString};
  bool m_onlyFirst{true};

  lineNumberSetByRange m_range;
  lineSet m_resultLines;

  void printLineAttrTable();
  void printImgGroupFollowingParaTable();
  void printResultLines();
};

bool isFoundAsNonKeys(const string &line, const string &key);
bool isFoundOutsidePersonalComments(const string &line, const string &key);
