#pragma once
#include "lineNumber.hpp"

static const string attachmentDirForLinkFromMain = R"(attachment\)";
static const string originalDirForLinkFromMain = R"(original\)";
static const string jpmDirForLinkFromMain = R"(JPM\)";
static const string pictureDirForLinkFromMain = R"(pictures\)";

static const string mainDirForLinkFromAttachment = R"(..\)";
static const string originalDirForLinkFromAttachment = R"(..\original\)";
static const string jpmDirForLinkFromAttachment = R"(..\JPM\)";
static const string pictureDirForLinkFromAttachment = R"(..\pictures\)";

static const string mainDirForLinkFromJPM = R"(..\)";
static const string originalDirForLinkFromJPM = R"(..\original\)";
static const string jpmDirForLinkFromJPM = R"(..\JPM\)";
static const string pictureDirForLinkFromJPM = R"(..\pictures\)";

enum class LINK_TYPE { MAIN, ATTACHMENT, ORIGINAL, SAMEPAGE, JPM, IMAGE };
enum class LINK_DISPLAY_TYPE { DIRECT, HIDDEN, UNHIDDEN };

static const string returnLinkFromAttachmentHeader = R"(返回本章原文)";
static const string annotationToOriginal = R"(原文)";
static const string contentTableFilename = R"(aindex)";

static const string TARGET_FILE_EXT = R"(.htm)";

// operations over link string template initialization
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara);

string fixLinkFromMainTemplate(const string &path, const string &filename,
                               LINK_DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara = emptyString);
string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &citation,
                                      const string &annotation,
                                      const string &referPara);
string
fixLinkFromOriginalTemplate(const string &path, const string &filename,
                            const string &key, const string &citation,
                            const string &annotation = annotationToOriginal,
                            const string &referPara = emptyString);
string fixLinkFromJPMTemplate(const string &path, const string &filename,
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
  LINK_TYPE getType() { return m_type; }
  bool isTargetToImage() { return (m_type == LINK_TYPE::IMAGE); };
  bool isTargetToJPMHtm() { return (m_type == LINK_TYPE::JPM); };
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
      m_needChange = true;
    }
    if (m_attachmentNumber != 0 and m_attachmentNumber != attachNo) {
      m_attachmentNumber = attachNo;
      m_needChange = true;
    }
  }
  void fixReferPara(const string &lineNumber) {
    if (m_referPara != lineNumber) {
      m_referPara = lineNumber;
      m_needChange = true;
    }
  }
  void setTypeThruFileNamePrefix(const string &link);

  string getWholeString() override;
  string getDisplayString() override;
  size_t displaySize() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

protected:
  LINK_DISPLAY_TYPE getDisplayType() { return m_displayType; }

  void readDisplayType(const string &linkString);
  void readType(const string &linkString);
  void readReferPara(const string &linkString);
  bool readAnnotation(const string &linkString);
  void readTypeAndAnnotation(const string &linkString) {
    readType(linkString);
    readAnnotation(linkString);
  }

  string displayPropertyAsString() {
    string result{emptyString};
    if (m_displayType == LINK_DISPLAY_TYPE::UNHIDDEN)
      result = unhiddenDisplayProperty;
    if (m_displayType == LINK_DISPLAY_TYPE::HIDDEN)
      result = hiddenDisplayProperty;
    return result;
  }

protected:
  LINK_TYPE m_type{LINK_TYPE::MAIN};
  LINK_DISPLAY_TYPE m_displayType{LINK_DISPLAY_TYPE::UNHIDDEN};
  string m_fromFile{emptyString};
  LineNumber m_fromLine;
  int m_chapterNumber{0};
  int m_attachmentNumber{0};
  string m_referPara{invalidLineNumber}; // might be top bottom
  string m_annotation{emptyString};
  bool m_needChange{false};
  string m_displayText{emptyString};
};
