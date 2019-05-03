#pragma once
#include "lineNumber.hpp"

// same value with those defined in fileUtil.hpp
static const std::string MAIN_TYPE_HTML_TARGET = R"(a0)";
static const std::string ORIGINAL_TYPE_HTML_TARGET = R"(c0)";
static const std::string ATTACHMENT_TYPE_HTML_TARGET = R"(b0)";
static const std::string JPM_TYPE_HTML_TARGET = R"(d)";

static const std::string attachmentDirForLinkFromMain = R"(attachment\)";
static const std::string originalDirForLinkFromMain = R"(original\)";
static const std::string jpmDirForLinkFromMain = R"(JPM\)";
static const std::string pictureDirForLinkFromMain = R"(pictures\)";

static const std::string mainDirForLinkFromAttachment = R"(..\)";
static const std::string originalDirForLinkFromAttachment = R"(..\original\)";
static const std::string jpmDirForLinkFromAttachment = R"(..\JPM\)";
static const std::string pictureDirForLinkFromAttachment = R"(..\pictures\)";

enum class LINK_TYPE { MAIN, ATTACHMENT, ORIGINAL, SAMEPAGE, JPM, IMAGE };
enum class LINK_DISPLAY_TYPE { DIRECT, HIDDEN, UNHIDDEN };

static const string returnLinkFromAttachmentHeader = R"(返回本章原文)";
static const string annotationToOriginal = R"(原文)";
static const string contentTableFilename = R"(aindex)";

static const std::string TARGET_FILE_EXT = R"(.htm)";

// operations over link string template initialization
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara);
string fixLinkFromMainTemplate(const string &path, const string &filename,
                               LINK_DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara);
string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &citation,
                                      const string &annotation);
string
fixLinkFromOriginalTemplate(const string &path, const string &filename,
                            const string &key, const string &citation,
                            const string &referPara,
                            const string &annotation = annotationToOriginal);
string fixLinkFromJPMTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &referPara,
                              const string &annotation = annotationToOriginal);
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation);
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
  int getchapterNumer() { return m_chapterNumber; }
  int getattachmentNumber() { return m_attachmentNumber; }
  string getAnnotation() { return m_annotation; }
  bool needUpdate() { return m_needChange; }
  string getSourceChapterName() { return m_fromFile; }
  void setSourcePara(LineNumber fp) { m_fromLine = fp; }
  string getChapterName() {
    if ((m_type == LINK_TYPE::JPM))
      return formatIntoZeroPatchedChapterNumber(m_chapterNumber,
                                                THREE_DIGIT_FILENAME);
    return formatIntoZeroPatchedChapterNumber(m_chapterNumber,
                                              TWO_DIGIT_FILENAME);
  }

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

  string getWholeString();
  string getDisplayString();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

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
