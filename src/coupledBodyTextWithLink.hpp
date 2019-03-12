#pragma once
#include "link.hpp"

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}

  void fixLinksFromFile(fileSet referMainFiles, fileSet referOriginalFiles,
                        fileSet referJPMFiles, bool forceUpdate = true,
                        int minPara = 0, int maxPara = 0, int minLine = 0,
                        int maxLine = 0);

  void removePersonalCommentsOverNumberedFiles();
  void removeImageForAutoNumbering() {}
  void addImageBackForManualNumbering() {}

  int sizeAfterRendering(const string &lineStr);

  string getDisplayString(const string &originalString);
  void render();

private:
  void printOffsetToObjectType();
  void printObjectTypeToOffset();
  void printLinkStringTable();
  void printCommentStringTable();
  void printPersonalCommentStringTable();
  void printPoemTranslationStringTable();

  void searchForEmbededLinks();
  void scanForTypes(const string &containedLine);
  bool isEmbeddedObject(OBJECT_TYPE type, size_t offset);

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
};

void testMixedObjects();
