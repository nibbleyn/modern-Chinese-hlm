#pragma once
#include "attachmentTable.hpp"
#include "coupledBodyText.hpp"

class Comment : public Object {
public:
  Comment(const string &fromFile) : m_fromFile(fromFile) {}
  string getStartTag() override { return commentBeginChars; }
  string getEndTag() override { return commentEndChars; }
  string getName() override { return nameOfCommentType; }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  string m_fromFile{emptyString};
};

class Citation : public Object {
public:
  static string
  getExpectedSection(AttachmentNumber num, ParaLineNumber paraLine,
                     const string &chapterString = defaultUnit,
                     const string &attachmentString = attachmentUnit,
                     const string &sectionString = citationPara);

  Citation() = default;
  void setReferSection(AttachmentNumber num, ParaLineNumber paraLine) {
    updateWithAttachmentNumberAndParaLineNumber(num, paraLine);
  }
  Citation(AttachmentNumber num, ParaLineNumber paraLine) {
    updateWithAttachmentNumberAndParaLineNumber(num, paraLine);
  }
  AttachmentNumber getAttachmentNumber() { return m_num; }
  ParaLineNumber getParaLineNumber() { return m_paraLine; }
  bool isValid() { return m_paraLine != make_pair(0, 0); }
  bool equal(AttachmentNumber num, ParaLineNumber paraLine) {
    return (m_num == num and m_paraLine == paraLine);
  }
  string getStartTag() override { return citationStartChars; }
  string getEndTag() override { return citationEndChars; }
  string getName() override { return nameOfCitationType; }
  string getWholeString() override { return getStringWithTags(); }
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;

private:
  void fromSectionString(const string &citationString,
                         const string &chapterString = defaultUnit,
                         const string &attachmentString = attachmentUnit,
                         const string &sectionString = citationPara);
  void updateWithAttachmentNumberAndParaLineNumber(AttachmentNumber num,
                                                   ParaLineNumber paraLine);
  AttachmentNumber m_num{0, 0};
  ParaLineNumber m_paraLine{0, 0};
  size_t m_page{0};
};

static const string returnLinkSetIndicator = R"(被引用：)";
static const string returnToContentTable = R"(回目录)";

static const string upArrow = R"(↑)";
static const string downArrow = R"(↓)";

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

  string getStartTag() override { return linkStartChars; }
  string getEndTag() override { return linkEndChars; }
  string getWholeString() override;
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0) override;
  string asString(bool ignoreOriginalPart = false);
  void doFixStatistics() {
    if (m_usedKey.find(keyNotFound) != string::npos) {
      recordMissingKeyLink();
    } else
      logLink();
  }
  void fixReferSection(const string &linkString) {
    Citation referSection;
    referSection.loadFirstFromContainedLine(linkString);
    if (referSection.isValid())
      fixReferSection(referSection.getAttachmentNumber(),
                      referSection.getParaLineNumber());
  }
  virtual void generateLinkToOrigin() = 0;
  string getStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return m_linkPtrToOrigin->asString();
    return emptyString;
  }

protected:
  string getKey() { return m_usedKey; }
  void readKey(const string &linkString);
  string getEnclosedStringOfLinkToOrigin() {
    if (m_linkPtrToOrigin != nullptr)
      return originalLinkStartChars + getStringOfLinkToOrigin() +
             originalLinkEndChars;
    return emptyString;
  }
  void fixReferSection(AttachmentNumber num, ParaLineNumber paraLine) {
    if (not m_referSection.equal(num, paraLine)) {
      if (not isReverseLink())
        m_referSection.hide();
      m_referSection.setReferSection(num, paraLine);
      needToChange();
    }
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
  Citation m_referSection;
  string m_usedKey{emptyString};
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
  LinkFromMain() : CoupledLink() {}
  LinkFromMain(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromMain(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {
    m_fullString = linkString;
    Object::readDisplayType();
  }
  ~LinkFromMain(){};
  void generateLinkToOrigin();
  string getName() override { return nameOfLinkFromMainType; }

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
  LinkFromAttachment() : CoupledLink() {}
  LinkFromAttachment(const string &fromFile) : CoupledLink(fromFile) {}
  LinkFromAttachment(const string &fromFile, const string &linkString)
      : CoupledLink(fromFile, linkString) {
    m_fullString = linkString;
    Object::readDisplayType();
  }
  ~LinkFromAttachment(){};
  void generateLinkToOrigin();
  string getName() override { return nameOfLinkFromAttachmentType; }

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
  string getName() override { return nameOfLinkFromJPMType; }

private:
  string getPathOfReferenceFile() const override;
  void logLink() override;
  void recordMissingKeyLink() override{};
  // utility to convert link type with filename
  string getHtmlFileNamePrefix() override;
  string getBodyTextFilePrefix() override;
  bool isTargetToAttachmentFile() override;
};

// only support link and comment sub-object
string scanForSubObjects(const string &original, const string &fromFile,
                         bool forLink = true);
