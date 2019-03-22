#pragma once
#include "link.hpp"

static const string REFERENCE_LINES = "container/referLines.txt";
static const string REFERENCE_PAGE = "container/referPage.txt";
static const string TO_CHECK_FILE = "container/toCheck.txt";
static const string PARA_UP = R"(向上)";
static const string PARA_DOWN = R"(向下)";

enum class DISPLY_LINE_TYPE { EMPTY, PARA, TEXT };
string getDisplayTypeString(DISPLY_LINE_TYPE type);

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}
  virtual ~CoupledBodyTextWithLink(){};

  void fixLinksFromFile(fileSet referMainFiles, fileSet referOriginalFiles,
                        fileSet referJPMFiles, bool forceUpdate = true,
                        int minPara = 0, int maxPara = 0, int minLine = 0,
                        int maxLine = 0);

  void removePersonalCommentsOverNumberedFiles();
  void removeImageForAutoNumbering() {}
  void addImageBackForManualNumbering() {}

  int sizeAfterRendering(const string &lineStr);

  string getDisplayString(const string &originalString);
  void printStringInLines();
  void render(bool hideParaHeader = false);
  void addLineNumber(const string &separatorColor, bool forceUpdate = true,
                     bool hideParaHeader = false) override;
  void scanLines();

private:
  size_t getAverageLineLengthFromReferenceFile();
  size_t getLinesofReferencePage();
  bool isParaSeparator(const string &inLine) {
    return (inLine.find(PARA_UP) != string::npos and
            inLine.find(PARA_DOWN) != string::npos);
  };
  size_t getLinesOfDisplayText(const string &dispString);
  void searchForEmbededLinks();
  void scanForTypes(const string &containedLine);
  bool isEmbeddedObject(OBJECT_TYPE type, size_t offset);

  size_t m_averageSizeOfOneLine{0};
  size_t m_SizeOfReferPage{0};
  struct LineInfo {
    size_t numberOfLines{0};
    DISPLY_LINE_TYPE type{DISPLY_LINE_TYPE::EMPTY};
    string cap{""};
  };
  // line No. -> number of display lines, line type
  using LineAttrTable = std::map<size_t, LineInfo>;
  LineAttrTable m_lineAttrTable;

  void printLineAttrTable() {
    if (not m_lineAttrTable.empty())
      METHOD_OUTPUT << "m_lineAttrTable:" << endl;
    for (const auto &element : m_lineAttrTable) {
      METHOD_OUTPUT << element.first << "  " << element.second.numberOfLines
                    << "  " << getDisplayTypeString(element.second.type) << "  "
                    << element.second.cap << endl;
    }
  }

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
};

void testMixedObjects();
