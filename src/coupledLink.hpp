#pragma once
#include "attachmentTable.hpp"
#include "coupledBodyText.hpp"

using FileSet = set<string>;

static const string returnLink = R"(被引用)";
static const string returnToContentTable = R"(回目录)";
static const string citationChapter = R"(章)";

class CoupledLink : public Link {
public:
  struct LinkDetails {
    string key{emptyString};
    string fromFile{emptyString};
    string fromLine{emptyString};
    string link{emptyString};
  };
  // statistics about links
  // chapter number (added with attachment number if over fromAttachmentLinks),
  // referPara -> vector<key, fromFile, fromLine, Link string>
  using LinksTable = map<pair<string, string>, vector<LinkDetails>>;
  static string referFilePrefix;
  static string linkDetailFilePath;
  static string keyDetailFilePath;
  static LinksTable linksTable;
  static void displayFixedLinks();

  static void clearLinkTable();
  static void outPutStatisticsToFiles();

public:
  CoupledLink() = default;
  CoupledLink(const string &fromFile) : Link(fromFile) {}
  // the first shallow step of construction
  CoupledLink(const string &fromFile, const string &linkString)
      : Link(fromFile, linkString) {}
  // the second step to read target file name
  bool readReferFileName(const string &link);
  // the last step also including fixing
  void fixFromString(const string &linkString);

  virtual ~CoupledLink(){};

  string getWholeString() override;
  string getDisplayString() override;
  size_t displaySize() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  string asString();
  void doStatistics() {
    if (m_usedKey.find(keyNotFound) != string::npos) {
      recordMissingKeyLink();
    } else
      logLink();
  }
  void fixReferSection(const string &expectedSection) {
    if (m_referSection != expectedSection) {
      m_referSection = expectedSection;
      m_needChange = true;
    }
  }
  virtual void generateLinkToOrigin() = 0;

protected:
  string getKey() { return m_usedKey; }
  string getReferSection() { return m_referSection; }
  void readKey(const string &linkString);
  string getStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return originalLinkStartChars + m_linkPtrToOrigin->asString() +
             originalLinkEndChars;
    return emptyString;
  }
  virtual void recordMissingKeyLink() = 0;

  // utility to convert link type with filename
  // samepage link would refer to different file prefix
  virtual string getHtmlFileNamePrefix() = 0;
  virtual string getBodyTextFilePrefix() = 0;
  virtual bool isTargetToAttachmentFile() = 0;

  // main and attachment html files are in different directories
  virtual string getPathOfReferenceFile() const = 0;
  virtual void logLink() = 0;

protected:
  string m_referSection{"0.0"};
  string m_usedKey{emptyString};
  bool m_needChange{false};
  using LinkPtr = unique_ptr<CoupledLink>;
  LinkPtr m_linkPtrToOrigin{nullptr};
  string m_imageReferFilename{emptyString};
};

class LinkFromMain : public CoupledLink {
public:
  static AttachmentList attachmentTable;

  static FileSet keyMissingChapters;
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void updateReferenceAttachmentListIntoFile();
  static void outPutStatisticsToFiles();
  static void displayMainFilesOfMissingKey();

public:
  LinkFromMain() = default;
  LinkFromMain(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromMain(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {}
  ~LinkFromMain(){};
  void generateLinkToOrigin();

private:
  string getPathOfReferenceFile() const override;
  void logLink();
  void recordMissingKeyLink();

  // utility to convert link type with filename
  string getHtmlFileNamePrefix();
  string getBodyTextFilePrefix();
  bool isTargetToAttachmentFile();
};

class LinkFromAttachment : public CoupledLink {
public:
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  LinkFromAttachment(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromAttachment(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {}
  ~LinkFromAttachment(){};
  void generateLinkToOrigin();

private:
  string getPathOfReferenceFile() const override;
  void logLink();
  void recordMissingKeyLink(){};
  // utility to convert link type with filename
  string getHtmlFileNamePrefix();
  string getBodyTextFilePrefix();
  bool isTargetToAttachmentFile();
};

class LinkFromJPM : public CoupledLink {
public:
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void outPutStatisticsToFiles();

public:
  LinkFromJPM(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromJPM(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {}
  ~LinkFromJPM(){};
  void generateLinkToOrigin();

private:
  string getPathOfReferenceFile() const override;
  void logLink();
  void recordMissingKeyLink(){};
  // utility to convert link type with filename
  string getHtmlFileNamePrefix();
  string getBodyTextFilePrefix();
  bool isTargetToAttachmentFile();
};

class Comment : public Object {
public:
  Comment(const string &fromFile) : m_fromFile(fromFile) {}
  string getWholeString();
  string getDisplayString();
  size_t displaySize();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);

private:
  string m_displayText{emptyString};
  string m_fromFile{emptyString};
};
