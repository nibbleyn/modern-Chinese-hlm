#pragma once
#include "hlmFileType.hpp"

enum class LINK_TYPE { MAIN, ATTACHMENT, ORIGINAL, SAMEPAGE };
enum class LINK_DISPLAY_TYPE { DIRECT, HIDDEN, UNHIDDEN };

LINK_TYPE getLinKTypeFromReferFileName(const string &refereFileName);

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

enum class ATTACHMENT_TYPE { PERSONAL, REFERENCE, NON_EXISTED };

using AttachmentNumber = pair<int, int>; // chapter number, attachment number
AttachmentNumber getAttachmentNumber(const string &filename);

static const string HTML_SRC_REF_ATTACHMENT_LIST =
    "utf8HTML/src/RefAttachments.txt";

class Link {
public:
  // statistics about links
  // chapter number (added with attachment number if over fromAttachmentLinks),
  // referPara, key -> vector<fromFile, fromLine, Link string>
  using LinksTable = map<std::tuple<string, string, string>,
                         vector<std::tuple<string, string, string>>>;
  static string outPutFilePath;
  static LinksTable linksTable;
  static void displayFixedLinks();

  // statistics about links to attachments
  using AttachmentFileNameTitleAndType =
      std::tuple<string, string, ATTACHMENT_TYPE>; // filename, title, type
  using AttachmentSet =
      map<AttachmentNumber,
          pair<string, AttachmentFileNameTitleAndType>>; // attachment number
                                                         // -> fromLine,
                                                         // <filename, title,
                                                         // type>
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
        annotation != returnLink)
      readKey(linkString); // key would be searched here and replaced
  }
  virtual ~Link(){};
  Link(const Link &) = delete;
  Link &operator=(const Link &) = delete;

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
  string asString() {
    string part0 = R"(<a )" + displayPropertyAsString() + R"( href=")";
    string part1{""}, part2{""}, part3{""};
    if (type != LINK_TYPE::SAMEPAGE) {
      part1 =
          getPathOfReferenceFile() + getFileNamePrefix(type) + getChapterName();
      if (attachmentNumber != 0)
        part2 = R"(_)" + TurnToString(attachmentNumber);
      part3 = R"(.htm)";
    }
    string part4{""};
    if (type != LINK_TYPE::ATTACHMENT or referPara != R"(P0L0)") {
      part4 = R"(#)" + referPara;
    }
    string part5 = R"(">)", part6{""}, part7{""};
    if (type != LINK_TYPE::ATTACHMENT and not usedKey.empty()) {
      part6 = R"(<i hidden>)" + getKey() + R"(</i>)";
      // easier to replace to <b unhidden> if want to display this
      if (type == LINK_TYPE::MAIN)
        part7 = R"(<b hidden>)" + getReferSection() + R"(</b>)";
    }
    string part8 = getAnnotation() + R"(</a>)";
    return (part0 + part1 + part2 + part3 + part4 + part5 + part6 + part7 +
            part8 + getStringOfLinkToOrigin());
  }
  LINK_TYPE getType() { return type; }
  bool isTargetToOriginalHtm() { return (type == LINK_TYPE::ORIGINAL); };
  bool isTargetToOtherMainHtm() {
    return (type == LINK_TYPE::MAIN and getChapterName() != fromFile);
  };
  bool isTargetToOtherAttachmentHtm() {
    return (type == LINK_TYPE::ATTACHMENT and
            (getChapterName() + R"(_)" + TurnToString(attachmentNumber)) !=
                fromFile);
  };
  bool isTargetToSelfHtm() {
    return ((type == LINK_TYPE::SAMEPAGE) or
            (type == LINK_TYPE::MAIN and getChapterName() == fromFile) or
            (type == LINK_TYPE::ATTACHMENT and
             (getChapterName() + R"(_)" + TurnToString(attachmentNumber)) ==
                 fromFile));
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
    if (usedKey.find("KeyNotFound") != string::npos) {
      recordMissingKeyLink();
    } else
      logLink();
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
      return R"(（)" + linkToOrigin->asString() + R"(）)";
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
  string fromFile{"01"};
  LineNumber fromLine;
  int chapterNumber{1};
  int attachmentNumber{0};
  string referPara{"P0L0"}; // might be top bottom
  string referSection{"0.0"};
  string usedKey{""};
  string annotation{""};
  bool needChange{false};
  std::unique_ptr<Link> linkToOrigin{nullptr};
};

static const string HTML_OUTPUT_LINKS_FROM_MAIN_LIST =
    "utf8HTML/output/LinksFromMain.txt";
static const string HTML_OUTPUT_ATTACHMENT_FROM_MAIN_LIST =
    "utf8HTML/output/AttachmentsFromMain.txt";

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
      result = R"(attachment\)";
    if (type == LINK_TYPE::ORIGINAL)
      result = R"(original\)";
    return result;
  }
  void logLink();

  // utility to convert link type with filename
  string getFileNamePrefix(LINK_TYPE type);
  string getBodyTextFilePrefix(LINK_TYPE type);
};

static const string HTML_OUTPUT_LINKS_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/LinksFromAttachment.txt";
static const string HTML_OUTPUT_ATTACHMENT_FROM_ATTACHMENT_LIST =
    "utf8HTML/output/AttachmentsFromAttachment.txt";

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
    if (type == LINK_TYPE::MAIN)
      result = R"(..\)";
    if (type == LINK_TYPE::ORIGINAL)
      result = R"(..\original\)";
    return result;
  }
  void logLink();

  // utility to convert link type with filename
  string getFileNamePrefix(LINK_TYPE type);
  string getBodyTextFilePrefix(LINK_TYPE type);
};
