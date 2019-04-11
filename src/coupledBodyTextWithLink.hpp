#pragma once
#include "linkEmbeddedObject.hpp"

static const string REFERENCE_LINES = "container/referLines.txt";
static const string REFERENCE_PAGE = "container/referPage.txt";
static const string TO_CHECK_FILE = "container/toCheck.txt";

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}
  virtual ~CoupledBodyTextWithLink(){};

  void validateParaSize();

  void disableAutoNumbering() { m_autoNumbering = false; }
  void addLineNumber(bool forceUpdate = true, bool hideParaHeader = false);

  void fixLinksFromFile(fileSet referMainFiles, fileSet referOriginalFiles,
                        fileSet referJPMFiles, bool forceUpdate = true,
                        int minPara = 0, int maxPara = 0, int minLine = 0,
                        int maxLine = 0);

  string getDisplayString(const string &originalString);

  void printStringInLines();
  void render(bool hideParaHeader = false);

  void removePersonalCommentsOverNumberedFiles();

private:
  bool isAutoNumbering() { return m_autoNumbering; }

  size_t m_averageSizeOfOneLine{0};
  size_t m_SizeOfReferPage{0};
  size_t getAverageLineLengthFromReferenceFile();
  size_t getLinesofReferencePage();
  size_t getLinesOfDisplayText(const string &dispString);

  struct ParaHeaderInfo {
    size_t seqOfParaHeader{0};
    size_t totalLinesAbove{0};
  };
  // line No. -> para No. and above info
  using ParaHeaderTable = std::map<size_t, ParaHeaderInfo>;
  ParaHeaderTable m_paraHeaderTable;

  bool isInParaHeaderTable(size_t seqOfLines) {
    try {
      m_paraHeaderTable.at(seqOfLines);
      return true;
    } catch (exception &) {
      // std::out_of_range if not existed
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
    }
    for (const auto &element : m_paraHeaderTable) {
      METHOD_OUTPUT << element.first << "        "
                    << element.second.seqOfParaHeader << "                  "
                    << element.second.totalLinesAbove << endl;
    }
  }

  void scanByRenderingLines();
  void calculateParaHeaderPositions();
  void paraGeneratedNumbering(bool forceUpdate, bool hideParaHeader);

  using LinkPtr = std::unique_ptr<Link>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};

  using OffsetToObjectType = std::map<size_t, OBJECT_TYPE>;
  using ObjectTypeToOffset = std::map<OBJECT_TYPE, size_t>;
  OffsetToObjectType m_offsetOfTypes;
  ObjectTypeToOffset m_foundTypes;

  struct LinkStringInfo {
    size_t startOffset{0};
    size_t endOffset{0};
    bool embedded{false};
  };
  using LinkStringTable = std::map<size_t, LinkStringInfo>;

  struct CommentStringInfo {
    size_t startOffset{0};
    size_t endOffset{0};
    bool embedded{false};
  };
  using CommentStringTable = std::map<size_t, CommentStringInfo>;

  using PersonalCommentStringTable = std::map<size_t, size_t>;
  using PoemTranslationStringTable = std::map<size_t, size_t>;

  LinkStringTable m_linkStringTable;
  CommentStringTable m_commentStringTable;
  PersonalCommentStringTable m_personalCommentStringTable;
  PoemTranslationStringTable m_poemTranslationStringTable;

  void printOffsetToObjectType() {
    for (const auto &element : m_offsetOfTypes) {
      METHOD_OUTPUT << element.first << "  "
                    << getNameOfObjectType(element.second) << endl;
    }
  }

  void printObjectTypeToOffset() {
    for (const auto &element : m_foundTypes) {
      METHOD_OUTPUT << getNameOfObjectType(element.first) << "  "
                    << element.second << endl;
    }
  }
  void printLinkStringTable() {
    if (not m_linkStringTable.empty())
      METHOD_OUTPUT << "m_linkStringTable:" << endl;
    for (const auto &element : m_linkStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.endOffset << "  "
                    << element.second.embedded << endl;
    }
  }

  void printCommentStringTable() {
    if (not m_commentStringTable.empty())
      METHOD_OUTPUT << "m_commentStringTable:" << endl;
    for (const auto &element : m_commentStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.endOffset << "  "
                    << element.second.embedded << endl;
    }
  }

  void printPersonalCommentStringTable() {
    if (not m_personalCommentStringTable.empty())
      METHOD_OUTPUT << "m_personalCommentStringTable:" << endl;
    for (const auto &element : m_personalCommentStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second << endl;
    }
  }

  void printPoemTranslationStringTable() {
    if (not m_poemTranslationStringTable.empty())
      METHOD_OUTPUT << "m_poemTranslationStringTable:" << endl;
    for (const auto &element : m_poemTranslationStringTable) {
      METHOD_OUTPUT << element.first << "  " << element.second << endl;
    }
  }

  void searchForEmbededLinks();
  void scanForTypes(const string &containedLine);
  bool isEmbeddedObject(OBJECT_TYPE type, size_t offset);

};
