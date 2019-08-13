#pragma once
#include "coupledLinkEmbeddedObject.hpp"

static const string REFERENCE_LINES = R"(container/referLines.txt)";
static const string REFERENCE_PAGE = R"(container/referPage.txt)";
static const string TO_CHECK_FILE = R"(container/toCheck.txt)";

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  struct LineDetails {
    int numberOfDisplayedLines{0};
    bool isImgGroup{false};
    set<string> objectContains;
  };
  // statistics about paras
  // chapter number (added with attachment number if over fromAttachmentLinks),
  // Para, line -> LineDetails
  using LinesTable = map<pair<AttachmentNumber, ParaLineNumber>, LineDetails>;
  static string referFilePrefix;
  static string lineDetailFilePath;
  static LinesTable linesTable;
  using RangeTable =
      map<pair<AttachmentNumber, ParaLineNumber>, lineNumberSetByRange>;
  static RangeTable rangeTable;
  static void setReferFilePrefix(const string &prefix);
  static void setStatisticsOutputFilePath(const string &path);
  static void clearExistingNumberingStatistics();
  static void appendNumberingStatistics();
  static void loadNumberingStatistics();
  static lineNumberSet getLineNumberMissingObjectType(AttachmentNumber num,
                                                      TypeSet typeSet);
  static void loadRangeTableFromFile(const string &indexFilePath);

public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}
  virtual ~CoupledBodyTextWithLink(){};

  // used for Auto-numbering
  void validateParaSize();
  void disableAutoNumbering() { m_autoNumbering = false; }
  void disableNumberingStatistics() { m_numberingStatistics = false; }
  void disableNumberingStatisticsCalculateLines() {
    m_disableNumberingStatisticsCalculateLines = true;
  }
  void hideParaHeader() { m_hideParaHeader = true; }
  void forceUpdateLineNumber() { m_forceUpdateLineNumber = true; }
  void forceUpdateLink() { m_forceUpdateLink = true; }
  void forceRemoveDuplicatedOriginalLinks() {
    m_forceRemoveDuplicatedOriginalLinks = true;
  }
  void addLineNumber();
  void appendReverseLinks();
  void doStatisticsByScanningLines(bool overFixedBodyText = false);

  // used for link-fixing
  void fixLinksFromFile(FileSet referMainFiles, FileSet referOriginalFiles,
                        FileSet referJPMFiles, int minPara = 0, int maxPara = 0,
                        int minLine = 0, int maxLine = 0);
  void setLineToFix(const string &originalString) { m_inLine = originalString; }
  void fixLinksWithinOneLine(FileSet referMainFiles, FileSet referOriginalFiles,
                             FileSet referJPMFiles);
  string getFixedLine() const { return m_inLine; }

  // used for rendering
  void getDisplayString(const string &originalString, bool onlyTypes = false);
  string getResultDisplayString() { return m_resultDisplayString; }
  TypeSet getResultSet() { return m_resultSet; }

  // used by tools
  void printStringInLines();
  void render(bool removeLinkToOriginalAndAttachment = false);
  void removePersonalCommentsOverNumberedFiles();

private:
  TypeSet m_resultSet;
  string m_resultDisplayString{emptyString};

  static void addEntriesInRangeTable(AttachmentNumber startNum,
                                     AttachmentNumber endNum,
                                     ParaLineNumber startPara,
                                     ParaLineNumber endPara);
  // used for Auto-numbering
  size_t m_averageSizeOfOneLine{0};
  size_t m_SizeOfReferPage{0};
  size_t getAverageLineLengthFromReferenceFile();
  void getLinesofReferencePage();
  size_t getLinesOfDisplayText(const string &dispString);

  struct ParaHeaderInfo {
    size_t seqOfParaHeader{0};
    size_t totalLinesAbove{0};
  };
  // line No. -> para No. and above info
  using ParaHeaderTable = map<size_t, ParaHeaderInfo>;
  ParaHeaderTable m_paraHeaderTable;

  bool isInParaHeaderTable(size_t seqOfLines) {
    try {
      m_paraHeaderTable.at(seqOfLines);
      return true;
    } catch (exception &) {
      return false;
    }
  }

  void printOversizedLines();
  void printParaHeaderTable();

  void scanByRenderingLines();
  void adjustParaHeaderStartNumber();
  void calculateParaHeaderPositions();
  void paraGeneratedNumbering();

  // used for rendering
  using OffsetToObjectType = map<size_t, TypeName>;
  using ObjectTypeToOffset = map<TypeName, size_t>;
  OffsetToObjectType m_offsetOfTypes;
  ObjectTypeToOffset m_foundTypes;

  struct LinkStringInfo {
    size_t startOffset{0};
    size_t endOffset{0};
    bool embedded{false};
  };
  using LinkStringTable = map<size_t, LinkStringInfo>;

  struct CommentStringInfo {
    size_t startOffset{0};
    size_t endOffset{0};
    bool embedded{false};
  };
  using CommentStringTable = map<size_t, CommentStringInfo>;

  using PersonalCommentStringTable = map<size_t, size_t>;
  using PoemTranslationStringTable = map<size_t, size_t>;

  LinkStringTable m_linkStringTable;
  CommentStringTable m_commentStringTable;
  PersonalCommentStringTable m_personalCommentStringTable;
  PoemTranslationStringTable m_poemTranslationStringTable;

  void printOffsetToObjectType();
  void printObjectTypeToOffset();
  void printLinkStringTable();
  void printCommentStringTable();
  void printPersonalCommentStringTable();
  void printPoemTranslationStringTable();

  void searchForEmbededLinks();
  void scanForTypes(const string &containedLine);
  bool isEmbeddedObject(string type, size_t offset);

  // used for link-fixing
  using LinkPtr = unique_ptr<CoupledLink>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};
  bool m_forceUpdateLink{false};
  bool m_forceRemoveDuplicatedOriginalLinks{false};
  void removeDuplicatedOriginalLinks(size_t processBegin);

  static void printLinesTable();
  static void printRangeTable();
};
