#pragma once
#include "Object.hpp"
#include "attachmentFiles.hpp"
#include "coupledBodyText.hpp"

enum class LINK_TYPE { MAIN, ATTACHMENT, ORIGINAL, SAMEPAGE };
enum class LINK_DISPLAY_TYPE { DIRECT, HIDDEN, UNHIDDEN };

// operations over link string template initialization
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &annotation,
                                   const string &referPara);
string fixLinkFromMainTemplate(const string &path, const string &filename,
                               LINK_DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara);
string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &annotation);
string fixLinkFromOriginalTemplate(const string &path, const string &filename,
                                   const string &key, const string &citation,
                                   const string &referPara);
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation);

static const string unhiddenDisplayProperty = R"(unhidden)";
static const string hiddenDisplayProperty = R"(hidden)";
static const string returnLinkFromAttachmentHeader = R"(返回本章原文)";
static const string returnLink = R"(被引用)";
static const string returnToContentTable = R"(回目录)";
static const string contentTableFilename = R"(aindex.htm)";
static const string citationChapterNo = R"(第)";
static const string citationChapter = R"(章)";

class Comment;
class Link : public Object {
public:
  struct LinkDetails {
    string key{""};
    string fromFile{""};
    string fromLine{""};
    string link{""};
  };
  // statistics about links
  // chapter number (added with attachment number if over fromAttachmentLinks),
  // referPara -> vector<key, fromFile, fromLine, Link string>
  using LinksTable = map<std::pair<string, string>, vector<LinkDetails>>;
  static string outPutFilePath;
  static LinksTable linksTable;
  static void displayFixedLinks();

  // statistics about links to attachments
  using AttachmentFileNameTitleAndType =
      std::tuple<string, string, ATTACHMENT_TYPE>; // filename, title, type
  // attachment number -> fromLine, <filename, title, type>
  using AttachmentSet =
      map<AttachmentNumber, pair<string, AttachmentFileNameTitleAndType>>;
  // imported attachment list
  static AttachmentSet refAttachmentTable;
  static ATTACHMENT_TYPE getAttachmentType(AttachmentNumber num);
  static void loadReferenceAttachmentList();
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  Link() = default;
  // the first shallow step of construction
  Link(const string &fromFile, const string &linkString)
      : m_fromFile(fromFile) {
    readTypeAndAnnotation(linkString);
  }
  // the second step to read target file name
  virtual bool readReferFileName(const string &link) = 0;
  // the last step also including fixing
  void fixFromString(const string &linkString) {
    readDisplayType(linkString);
    readReferPara(linkString);
    // no need for key for these links
    if (m_annotation != returnLinkFromAttachmentHeader and
        m_annotation != returnLink and m_annotation != returnToContentTable)
      readKey(linkString); // key would be searched here and replaced
  }
  virtual ~Link(){};
  Link(const Link &) = delete;
  Link &operator=(const Link &) = delete;
  string getWholeString();
  string getDisplayString();
  size_t length();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  string asString();
  LINK_TYPE getType() { return m_type; }
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
    return formatIntoTwoDigitChapterNumber(m_chapterNumber);
  }
  void doStatistics() {
    if (m_usedKey.find(keyNotFound) != string::npos) {
      recordMissingKeyLink();
    } else
      logLink();
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
  void fixReferSection(const string &expectedSection) {
    if (m_referSection != expectedSection) {
      m_referSection = expectedSection;
      m_needChange = true;
    }
  }
  virtual void generateLinkToOrigin() = 0;

protected:
  LINK_DISPLAY_TYPE getDisplayType() { return m_displayType; }
  string getKey() { return m_usedKey; }
  string getReferSection() { return m_referSection; }

  void readDisplayType(const string &linkString);
  void readType(const string &linkString);
  void readReferPara(const string &linkString);
  bool readAnnotation(const string &linkString);
  void readKey(const string &linkString);
  void readTypeAndAnnotation(const string &linkString) {
    readType(linkString);
    readAnnotation(linkString);
  }

  string displayPropertyAsString() {
    string result{""};
    if (m_displayType == LINK_DISPLAY_TYPE::UNHIDDEN)
      result = unhiddenDisplayProperty;
    if (m_displayType == LINK_DISPLAY_TYPE::HIDDEN)
      result = hiddenDisplayProperty;
    return result;
  }
  string getStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return originalLinkStartChars + m_linkPtrToOrigin->asString() +
             originalLinkEndChars;
    return "";
  }
  void recordMissingKeyLink();

  // utility to convert link type with filename
  // samepage link would refer to different file prefix
  virtual string getHtmlFileNamePrefix() = 0;
  virtual string getBodyTextFilePrefix() = 0;

  // main and attachment html files are in different directories
  virtual string getPathOfReferenceFile() const = 0;
  virtual void logLink() = 0;

protected:
  LINK_TYPE m_type{LINK_TYPE::MAIN};
  LINK_DISPLAY_TYPE m_displayType{LINK_DISPLAY_TYPE::UNHIDDEN};
  string m_fromFile{"81"};
  LineNumber m_fromLine;
  int m_chapterNumber{0};
  int m_attachmentNumber{0};
  string m_referPara{invalidLineNumber}; // might be top bottom
  string m_referSection{"0.0"};
  string m_usedKey{""};
  string m_annotation{""};
  bool m_needChange{false};
  using LinkPtr = std::unique_ptr<Link>;
  LinkPtr m_linkPtrToOrigin{nullptr};
  using CommentPtr = std::unique_ptr<Comment>;
  using CommentPtrCollection = std::map<size_t, CommentPtr>;
  CommentPtrCollection m_commentPtrs;
};

class LinkFromMain : public Link {
public:
  static AttachmentSet attachmentTable;
  static void displayAttachments();
  static string getFromLineOfAttachment(AttachmentNumber num);

  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  LinkFromMain() = default;
  LinkFromMain(const string &fromFile, const string &linkString)
      : Link(fromFile, linkString) {}
  ~LinkFromMain(){};
  void generateLinkToOrigin();
  bool readReferFileName(const string &link);

private:
  string getPathOfReferenceFile() const override;
  void logLink();

  // utility to convert link type with filename
  string getHtmlFileNamePrefix();
  string getBodyTextFilePrefix();
};

class LinkFromAttachment : public Link {
public:
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  LinkFromAttachment(const string &fromFile, const string &linkString)
      : Link(fromFile, linkString) {}
  ~LinkFromAttachment(){};
  void generateLinkToOrigin();
  bool readReferFileName(const string &link);
  void setTypeThruFileNamePrefix(const string &link);

private:
  string getPathOfReferenceFile() const override;
  void logLink();

  // utility to convert link type with filename
  string getHtmlFileNamePrefix();
  string getBodyTextFilePrefix();
};

class Comment : public Object {
public:
  size_t length() { return 0; }
  size_t displaySize() { return 0; }
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) {
    return 0;
  }

private:
  using LinkPtr = std::unique_ptr<Link>;
  using LinkPtrCollection = std::map<size_t, LinkPtr>;
  LinkPtrCollection m_linkPtrs;
  using TextCollection = std::map<size_t, string>;
  TextCollection m_texts;
};

static const string personalCommentStartChars = R"(（<u unhidden)";
static const string personalCommentEndChars = R"(</u>）)";
static const string endOfPersonalCommentBeginTag = R"(">)";

class PersonalComment : public Object {
public:
  string getWholeString();
  string getDisplayString();
  size_t length();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

private:
  using LinkPtr = std::unique_ptr<Link>;
  using LinkPtrCollection = std::map<size_t, LinkPtr>;
  LinkPtrCollection m_linkPtrs;
  using TextCollection = std::map<size_t, string>;
  TextCollection m_texts;
};

// poemTranslation
static const string poemTranslationBeginChars = R"(<samp)";
static const string poemTranslationEndChars = R"(</samp>)";
static const string endOfPoemTranslationBeginTag = R"(">)";

class PoemTranslation : public Object {
public:
  string getWholeString();
  string getDisplayString();
  size_t length();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

private:
  using LinkPtr = std::unique_ptr<Link>;
  using LinkPtrCollection = std::map<size_t, LinkPtr>;
  LinkPtrCollection m_linkPtrs;
  using TextCollection = std::map<size_t, string>;
  TextCollection m_texts;
};

void testLinkOperation();
