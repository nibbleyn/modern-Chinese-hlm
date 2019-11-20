#pragma once
#include "lineNumber.hpp"

static const string attachmentDirForLinkFromMain = R"(attachment\)";
static const string originalDirForLinkFromMain = R"(original\)";
static const string jpmDirForLinkFromMain = R"(JPM\)";
static const string mdtDirForLinkFromMain = R"(MDT\)";
static const string pictureDirForLinkFromMain = R"(pictures\)";

enum class LINK_TYPE {
  MAIN,
  ATTACHMENT,
  ORIGINAL,
  SAMEPAGE,
  JPM,
  MDT,
  IMAGE,
  REVERSE,
  SUBPARA
};

// links
static const string imageTypeChars = R"(IMAGE)";
static const string referFileMiddleChar = R"(href=")";
static const string referParaMiddleChar = R"(#)";
static const string referParaEndChar = R"(">)";
static const string changeKey = R"(changeKey)";
static const string originalLinkStartChars = R"(（)";
static const string originalLinkEndChars = R"(）)";

static const string returnLinkFromAttachmentHeader = R"(返回本章原文)";
static const string annotationToOriginal = R"(原文)";
static const string contentTableFilename = R"(aindex)";

static const string upArrow = R"(↑)";
static const string downArrow = R"(↓)";
static const string subParaTitle = R"(子段)";

static constexpr const char *ImageAnnotationStartChars = R"(↑（图示：)";
static const string TARGET_FILE_EXT = R"(.htm)";

// operations over link string template initialization
string fixLinkFromSameFileTemplate(DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara);

string fixLinkFromMainTemplate(const string &path, const string &filename,
                               DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara = emptyString);
string fixLinkFromReverseLinkTemplate(const string &filename, DISPLAY_TYPE type,
                                      const string &citation,
                                      const string &referPara,
                                      const string &annotation = emptyString);
string fixLinkFromSubParaLinkTemplate(const string &seqenceNumber,
                                      DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &id = emptyString,
                                      const string &subPara = emptyString);
string
fixLinkFromOriginalTemplate(const string &path, const string &filename,
                            const string &key, const string &citation,
                            const string &annotation = annotationToOriginal,
                            const string &referPara = emptyString);
string fixLinkFromJPMTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &annotation = annotationToOriginal,
                              const string &referPara = emptyString);
string fixLinkFromMDTTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &annotation = annotationToOriginal,
                              const string &referPara = emptyString);
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation,
                                     const string &referPara = emptyString);

string fixLinkFromImageTemplate(
    const string &fullReferFilenameWithPathExt, const string &picFilename,
    const string &annotation,
    const string &displayProperty = unhiddenDisplayProperty);

class Link : public Object {
public:
  Link() = default;
  Link(const string &fromFile) : m_fromFile(fromFile) {}
  // the first shallow step of construction
  Link(const string &fromFile, const string &linkString)
      : m_fromFile(fromFile) {
    readTypeAndAnnotation(linkString);
  }

  virtual ~Link(){};
  Link(const Link &) = delete;
  Link &operator=(const Link &) = delete;
  void readTypeAndAnnotation(const string &linkString) {
    readType(linkString);
    readAnnotation(linkString);
  }
  LINK_TYPE getType() { return m_type; }
  bool isSubParaLink() { return m_type == LINK_TYPE::SUBPARA; };
  bool isReverseLink() { return m_type == LINK_TYPE::REVERSE; };
  bool isTargetToImage() { return (m_type == LINK_TYPE::IMAGE); };
  bool isTargetToJPMHtm() { return (m_type == LINK_TYPE::JPM); };
  bool isTargetToMDTHtm() { return (m_type == LINK_TYPE::MDT); };
  bool isTargetToOriginalHtm() { return (m_type == LINK_TYPE::ORIGINAL); };
  bool isTargetToOtherMainHtm() {
    return (m_type == LINK_TYPE::MAIN and getChapterName() != m_fromFile);
  };
  bool isTargetToOtherAttachmentHtm() {
    return (m_type == LINK_TYPE::ATTACHMENT and
            (getChapterName() + attachmentFileMiddleChar +
             TurnToString(m_attachmentNumber)) != m_fromFile);
  };
  bool isTargetToSelfHtm() {
    return ((m_type == LINK_TYPE::SAMEPAGE) or
            (m_type == LINK_TYPE::MAIN and getChapterName() == m_fromFile) or
            (m_type == LINK_TYPE::ATTACHMENT and
             (getChapterName() + attachmentFileMiddleChar +
              TurnToString(m_attachmentNumber)) == m_fromFile));
  };
  string getChapterName() {
    if (m_type == LINK_TYPE::JPM)
      return getChapterNameByTargetKind(JPM_TYPE_HTML_TARGET, m_chapterNumber);
    return getChapterNameByTargetKind(MAIN_TYPE_HTML_TARGET, m_chapterNumber);
  }

  int getchapterNumer() { return m_chapterNumber; }
  int getattachmentNumber() { return m_attachmentNumber; }
  string getAnnotation() { return m_annotation; }
  bool needUpdate() { return m_needChange; }
  string getSourceChapterName() { return m_fromFile; }
  void setSourcePara(LineNumber fp) { m_fromLine = fp; }

  void fixReferFile(int chapter, int attachNo = 0) {
    if (m_chapterNumber != chapter) {
      m_chapterNumber = chapter;
      needToChange();
    }
    if (m_attachmentNumber != 0 and m_attachmentNumber != attachNo) {
      m_attachmentNumber = attachNo;
      needToChange();
    }
  }
  void fixReferPara(const string &lineNumber) {
    if (m_referPara != lineNumber) {
      m_referPara = lineNumber;
      needToChange();
    }
  }
  void setTypeThruFileNamePrefix(const string &link);

  string getFormatedFullString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

protected:
  void readType(const string &linkString);
  void readReferPara(const string &linkString);
  bool readAnnotation(const string &linkString);
  void needToChange();

protected:
  LINK_TYPE m_type{LINK_TYPE::MAIN};
  string m_fromFile{emptyString};
  LineNumber m_fromLine;
  int m_chapterNumber{0};
  int m_attachmentNumber{0};
  string m_referPara{invalidLineNumber}; // might be top bottom
  string m_annotation{emptyString};
  bool m_needChange{false};
};
