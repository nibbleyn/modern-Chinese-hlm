#pragma once
#include "hlmFileType.hpp"

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
                                   const string &key, const string &referPara);
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation);

static const string returnLinkFromAttachmentHeader = R"(返回本章原文)";
static const string returnLink = R"(被引用)";
static const string returnToContentTable = R"(回目录)";
static const string contentTableFilename = R"(aindex.htm)";

enum class ATTACHMENT_TYPE { PERSONAL, REFERENCE, NON_EXISTED };

using AttachmentNumber = pair<int, int>; // chapter number, attachment number
AttachmentNumber getAttachmentNumber(const string &filename);

static const string linkStartChars = R"(<a)";
static const string linkEndChars = R"(</a>)";
static const string originalLinkStartChars = R"(（)";
static const string originalLinkEndChars = R"(）)";

class Link {
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
  Link(const string &fromFile, const string &linkString) : fromFile(fromFile) {
    readTypeAndAnnotation(linkString);
  }
  // the second step to read target file name
  virtual bool readReferFileName(const string &link) = 0;
  // the last step also including fixing
  void fixFromString(const string &linkString) {
    readDisplayType(linkString);
    readReferPara(linkString);
    // no need for key for these links
    if (annotation != returnLinkFromAttachmentHeader and
        annotation != returnLink and annotation != returnToContentTable)
      readKey(linkString); // key would be searched here and replaced
  }
  virtual ~Link(){};
  Link(const Link &) = delete;
  Link &operator=(const Link &) = delete;
  string asString();
  LINK_TYPE getType() { return type; }
  bool isTargetToOriginalHtm() { return (type == LINK_TYPE::ORIGINAL); };
  bool isTargetToOtherMainHtm() {
    return (type == LINK_TYPE::MAIN and getChapterName() != fromFile);
  };
  bool isTargetToOtherAttachmentHtm() {
    return (type == LINK_TYPE::ATTACHMENT and
            (getChapterName() + attachmentFileMiddleChar +
             TurnToString(attachmentNumber)) != fromFile);
  };
  bool isTargetToSelfHtm() {
    return ((type == LINK_TYPE::SAMEPAGE) or
            (type == LINK_TYPE::MAIN and getChapterName() == fromFile) or
            (type == LINK_TYPE::ATTACHMENT and
             (getChapterName() + attachmentFileMiddleChar +
              TurnToString(attachmentNumber)) == fromFile));
  };
  int getchapterNumer() { return chapterNumber; }
  int getattachmentNumber() { return attachmentNumber; }
  string getAnnotation() { return annotation; }
  bool needUpdate() { return needChange; }
  string getSourceChapterName() { return fromFile; }
  void setSourcePara(LineNumber fp) { fromLine = fp; }
  string getChapterName() {
    return formatIntoTwoDigitChapterNumber(chapterNumber);
  }
  void doStatistics() {
    if (usedKey.find(keyNotFound) != string::npos) {
      recordMissingKeyLink();
    } else
      logLink();
  }

  void fixReferFile(int chapter, int attachNo = 0) {
    if (chapterNumber != chapter) {
      chapterNumber = chapter;
      needChange = true;
    }
    if (attachmentNumber != 0 and attachmentNumber != attachNo) {
      attachmentNumber = attachNo;
      needChange = true;
    }
  }
  void fixReferPara(const string &lineNumber) {
    if (referPara != lineNumber) {
      referPara = lineNumber;
      needChange = true;
    }
  }
  void fixReferSection(const string &expectedSection) {
    if (referSection != expectedSection) {
      referSection = expectedSection;
      needChange = true;
    }
  }

protected:
  LINK_DISPLAY_TYPE getDisplayType() { return displayType; }
  string getKey() { return usedKey; }
  string getReferSection() { return referSection; }

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
    if (displayType == LINK_DISPLAY_TYPE::UNHIDDEN)
      result = R"(unhidden)";
    if (displayType == LINK_DISPLAY_TYPE::HIDDEN)
      result = R"(hidden)";
    return result;
  }
  string getStringOfLinkToOrigin() {
    if (linkToOrigin != nullptr)
      return originalLinkStartChars + linkToOrigin->asString() +
             originalLinkEndChars;
    return "";
  }
  void recordMissingKeyLink();

  // utility to convert link type with filename
  virtual string getFileNamePrefix(LINK_TYPE type) = 0;
  virtual string getBodyTextFilePrefix(LINK_TYPE type) = 0;
  virtual string getPathOfReferenceFile() const = 0;
  virtual void generateLinkToOrigin() = 0;
  virtual void logLink() = 0;

protected:
  LINK_TYPE type{LINK_TYPE::MAIN};
  LINK_DISPLAY_TYPE displayType{LINK_DISPLAY_TYPE::UNHIDDEN};
  string fromFile{"81"};
  LineNumber fromLine;
  int chapterNumber{0};
  int attachmentNumber{0};
  string referPara{invalidLineNumber}; // might be top bottom
  string referSection{"0.0"};
  string usedKey{""};
  string annotation{""};
  bool needChange{false};
  std::unique_ptr<Link> linkToOrigin{nullptr};
};

static const string attachmentDirForLinkFromMain = R"(attachment\)";
static const string originalDirForLinkFromMain = R"(original\)";

class LinkFromMain : public Link {
public:
  static AttachmentSet attachmentTable;
  static void displayAttachments();
  static string getFromLineOfAttachment(AttachmentNumber num);

  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  LinkFromMain(const string &fromFile, const string &linkString)
      : Link(fromFile, linkString) {}
  ~LinkFromMain(){};
  void generateLinkToOrigin();
  bool readReferFileName(const string &link);

private:
  /**
   *  the directory structure is like below
   *  refer to utf8HTML/src
   *  \              <-link from main to access main
   *  \attachment\   <-link from attachment to access main to access other
   *  \original\     <- no link in text here
   *  \a*.htm        <- main texts
   * @param linkString the link to check
   * @return the level difference between a link and its target
   */
  string getPathOfReferenceFile() const override {
    string result{""};
    if (type == LINK_TYPE::ATTACHMENT)
      result = attachmentDirForLinkFromMain;
    if (type == LINK_TYPE::ORIGINAL)
      result = originalDirForLinkFromMain;
    return result;
  }
  void logLink();

  // utility to convert link type with filename
  string getFileNamePrefix(LINK_TYPE type);
  string getBodyTextFilePrefix(LINK_TYPE type);
};

static const string mainDirForLinkFromAttachment = R"(..\)";
static const string originalDirForLinkFromAttachment = R"(..\original\)";

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
  /**
   *  the directory structure is like below
   *  refer to utf8HTML/src
   *  \              <-link from main to access main
   *  \attachment\   <-link from attachment to access main to access other
   *  \original\     <- no link in text here
   *  \a*.htm        <- main texts
   * @param linkString the link to check
   * @return the level difference between a link and its target
   */
  string getPathOfReferenceFile() const override {
    string result{""};
    if (type == LINK_TYPE::MAIN or annotation == returnToContentTable)
      result = mainDirForLinkFromAttachment;
    if (type == LINK_TYPE::ORIGINAL)
      result = originalDirForLinkFromAttachment;
    return result;
  }
  void logLink();

  // utility to convert link type with filename
  string getFileNamePrefix(LINK_TYPE type);
  string getBodyTextFilePrefix(LINK_TYPE type);
};
