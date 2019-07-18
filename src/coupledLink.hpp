#pragma once
#include "attachmentTable.hpp"
#include "coupledBodyText.hpp"

static const string returnLinkSetIndicator = R"(被引用：)";
static const string returnToContentTable = R"(回目录)";
static const string citationChapter = R"(章)";

class CoupledLink : public Link {
public:
  struct LinkDetails {
    string key{emptyString};
    string link{emptyString};
  };
  // (fromFile, fromLine) -> LinkDetails
  using LinkDetailSet =
      map<pair<AttachmentNumber, ParaLineNumber>, LinkDetails>;
  // statistics about links
  // (chapter number, referPara) -> LinkDetailSet
  using LinksTable = map<pair<AttachmentNumber, ParaLineNumber>, LinkDetailSet>;
  static string referFilePrefix;
  static string linkDetailFilePath;
  static string keyDetailFilePath;
  static LinksTable linksTable;

  static void clearLinkTable();
  static void loadLinkTableFromStatisticsFile();
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
  string asString(bool ignoreOriginalPart = false);
  void doFixStatistics() {
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
  string getStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return m_linkPtrToOrigin->asString();
    return emptyString;
  }

protected:
  string getKey() { return m_usedKey; }
  string getReferSection() { return m_referSection; }
  void readKey(const string &linkString);
  string getEnclosedStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return originalLinkStartChars + getStringOfLinkToOrigin() +
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
  static void setupStatisticsParameters();
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
  void logLink() override;
  void recordMissingKeyLink() override;

  // utility to convert link type with filename
  string getHtmlFileNamePrefix() override;
  string getBodyTextFilePrefix() override;
  bool isTargetToAttachmentFile() override;
};

class LinkFromAttachment : public CoupledLink {
public:
  static void resetStatisticsAndLoadReferenceAttachmentList();
  static void setupStatisticsParameters();
  static void outPutStatisticsToFiles();

public:
  LinkFromAttachment(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromAttachment(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {}
  ~LinkFromAttachment(){};
  void generateLinkToOrigin();

private:
  string getPathOfReferenceFile() const override;
  void logLink() override;
  void recordMissingKeyLink() override{};
  // utility to convert link type with filename
  string getHtmlFileNamePrefix() override;
  string getBodyTextFilePrefix() override;
  bool isTargetToAttachmentFile() override;
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
  void logLink() override;
  void recordMissingKeyLink() override{};
  // utility to convert link type with filename
  string getHtmlFileNamePrefix() override;
  string getBodyTextFilePrefix() override;
  bool isTargetToAttachmentFile() override;
};

class Comment : public Object {
public:
  Comment(const string &fromFile) : m_fromFile(fromFile) {}
  string getWholeString() override;
  string getDisplayString() override;
  size_t displaySize() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_displayText{emptyString};
  string m_fromFile{emptyString};
};
