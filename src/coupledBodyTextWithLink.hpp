#pragma once
#include "coupledLinkEmbeddedObject.hpp"

static const string REFERENCE_LINES = R"(container/referLines.txt)";
static const string REFERENCE_PAGE = R"(container/referPage.txt)";
static const string TO_CHECK_FILE = R"(container/toCheck.txt)";

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  struct LineDetails {
    size_t numberOfDisplayedLines{0};
    bool isImgGroup{false};
    Object::SET_OF_OBJECT_TYPES objectContains;
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
  void addLineNumber();
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
  string getDisplayString(const string &originalString);
  Object::SET_OF_OBJECT_TYPES
  getContainedObjectTypes(const string &originalString);

  // used by tools
  void printStringInLines();
  void render();
  void removePersonalCommentsOverNumberedFiles();

private:
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

  void printOversizedLines() {
    for (const auto &element : m_lineAttrTable) {
      if (element.second.numberOfLines > m_SizeOfReferPage) {
        METHOD_OUTPUT << "file:      " << m_file << endl;
        METHOD_OUTPUT << element.first << "        "
                      << element.second.numberOfLines << "          "
                      << getDisplayTypeString(element.second.type) << "  "
                      << element.second.cap << endl;
      }
    }
  }

  void printParaHeaderTable() {
    if (not m_paraHeaderTable.empty()) {
      METHOD_OUTPUT << "m_paraHeaderTable:" << endl;
      METHOD_OUTPUT << "line No/seqOfParaHeader/totalLinesAbove" << endl;
    } else
      METHOD_OUTPUT << "no entry in m_paraHeaderTable." << endl;
    for (const auto &element : m_paraHeaderTable) {
      METHOD_OUTPUT << element.first << "        "
                    << element.second.seqOfParaHeader << "                  "
                    << element.second.totalLinesAbove << endl;
    }
  }

  void scanByRenderingLines();
  void calculateParaHeaderPositions();
  void paraGeneratedNumbering();

  // used for rendering
  using OffsetToObjectType = map<size_t, Object::OBJECT_TYPE>;
  using ObjectTypeToOffset = map<Object::OBJECT_TYPE, size_t>;
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

  void printOffsetToObjectType() {
    if (not m_offsetOfTypes.empty())
      METHOD_OUTPUT << "m_offsetOfTypes:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_offsetOfTypes." << endl;
    for (const auto &element : m_offsetOfTypes) {
      METHOD_OUTPUT << element.first << "  "
                    << Object::getNameOfObjectType(element.second) << endl;
    }
  }

  void printObjectTypeToOffset() {
    if (not m_foundTypes.empty())
      METHOD_OUTPUT << "m_foundTypes:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_foundTypes." << endl;
    for (const auto &element : m_foundTypes) {
      METHOD_OUTPUT << Object::getNameOfObjectType(element.first) << "  "
                    << element.second << endl;
    }
  }
  void printLinkStringTable() {
    if (not m_linkStringTable.empty())
      METHOD_OUTPUT << "m_linkStringTable:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_linkStringTable." << endl;
    for (const auto &element : m_linkStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.endOffset << "  "
                    << element.second.embedded << endl;
    }
  }

  void printCommentStringTable() {
    if (not m_commentStringTable.empty())
      METHOD_OUTPUT << "m_commentStringTable:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_commentStringTable." << endl;
    for (const auto &element : m_commentStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.endOffset << "  "
                    << element.second.embedded << endl;
    }
  }

  void printPersonalCommentStringTable() {
    if (not m_personalCommentStringTable.empty())
      METHOD_OUTPUT << "m_personalCommentStringTable:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_personalCommentStringTable." << endl;
    for (const auto &element : m_personalCommentStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second << endl;
    }
  }

  void printPoemTranslationStringTable() {
    if (not m_poemTranslationStringTable.empty())
      METHOD_OUTPUT << "m_poemTranslationStringTable:" << endl;
    else
      METHOD_OUTPUT << "no entry in m_poemTranslationStringTable." << endl;
    for (const auto &element : m_poemTranslationStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second << endl;
    }
  }

  void searchForEmbededLinks();
  void scanForTypes(const string &containedLine);
  bool isEmbeddedObject(Object::OBJECT_TYPE type, size_t offset);

  // used for link-fixing
  using LinkPtr = unique_ptr<CoupledLink>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};
  bool m_forceUpdateLink{false};

  void printLinesTable() {
    if (not linesTable.empty()) {
      METHOD_OUTPUT << "linesTable:" << endl;
      METHOD_OUTPUT << "chapter/attachment/ParaNumber/LineNumber" << endl;
    } else
      METHOD_OUTPUT << "no entry in linesTable." << endl;
    for (const auto &element : linesTable) {
      auto num = element.first.first;
      auto paraLine = element.first.second;
      auto detail = element.second;
      METHOD_OUTPUT << num.first << "  " << num.second << "  " << paraLine.first
                    << "  " << paraLine.second << "  " << std::boolalpha
                    << detail.isImgGroup << "  "
                    << detail.numberOfDisplayedLines << "  "
                    << Object::typeSetAsString(detail.objectContains) << endl;
    }
  }

  static void printRangeTable() {
    if (not rangeTable.empty()) {
      FUNCTION_OUTPUT << "rangeTable:" << endl;
      FUNCTION_OUTPUT << "chapter/attachment/startParaNumber/startLineNumber/"
                         "endParaNumber/endLineNumber"
                      << endl;
    } else
      FUNCTION_OUTPUT << "no entry in rangeTable." << endl;
    for (const auto &element : rangeTable) {
      auto num = element.first.first;
      auto paraLine = element.first.second;
      auto startPara = element.second.first;
      auto endPara = element.second.first;
      FUNCTION_OUTPUT << num.first << "  " << num.second << "  "
                      << paraLine.first << "  " << paraLine.second << "  "
                      << startPara.first << "  " << startPara.second << "  "
                      << endPara.first << "  " << endPara.second << "  "

                      << endl;
    }
  }
};
