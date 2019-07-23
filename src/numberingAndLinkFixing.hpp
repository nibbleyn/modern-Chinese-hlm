#pragma once

#include "libraryInterface.hpp"

class Commander {
public:
  enum class COMMAND {
    validateFormatForNumbering,
    validateParaSizeForAutoNumbering,
    addLineNumber,
    appendReverseLinks,
    fixLinksFromMainFile,
    fixLinksFromJPMFile,
    fixLinksFromAttachmentFile
  };

public:
  Commander() = default;
  virtual ~Commander(){};

  COMMAND m_command{COMMAND::addLineNumber};

  string m_kind{MAIN};
  int m_minTarget{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxTarget{MAIN_MAX_CHAPTER_NUMBER};
  int m_minAttachNo{0};
  int m_maxAttachNo{0};

  // for numbering
  bool m_forceUpdateLineNumber{true};
  bool m_hideParaHeader{false};
  bool m_disableAutoNumbering{true};

  // for link fixing
  int m_minReferenceToMain{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToMain{MAIN_MAX_CHAPTER_NUMBER};
  int m_minReferenceToOriginal{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToOriginal{MAIN_MAX_CHAPTER_NUMBER};
  int m_minReferenceToJPM{JPM_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToJPM{JPM_MAX_CHAPTER_NUMBER};
  bool m_fixReturnLink{true};
  bool m_forceUpdateLink{true};

  void runCommandOverFiles();

protected:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  FileSet m_fileSet;
  FileSet m_referenceToMainfileSet;
  FileSet m_referenceToOriginalfileSet;
  FileSet m_referenceToJPMfileSet;
  CoupledBodyTextContainer m_container;
  CoupledBodyTextWithLink m_bodyText;
  virtual void dissembleHtmls() = 0;
  virtual void assembleHtmls() = 0;
  virtual void runCommandOverEachFile() = 0;
  void execute();
  void updateAttachmentContentTableAndfixReturnLink();
  void backupAndOverwriteSrcFiles();
  void setupReferenceFileSet();
  void setupNumberingStatistics();
  void setupLinkFixingStatistics();
  void setupBodyTextFixDir();
  void LoadBodyTextFromFixDir();
  void updateAttachmentListIntoFile();
  void outputLinkFixingStatistics();

  int m_storedDebugLevel{0};
  void increaseDebugLevel();
  void restoreDebugLevel();
};

class NonAttachmentCommander : public Commander {
  void dissembleHtmls() override;
  void assembleHtmls() override;
  void runCommandOverEachFile() override;
};

class AttachmentCommander : public Commander {
  void dissembleHtmls() override;
  void assembleHtmls() override;
  void runCommandOverEachFile() override;
};

void numberJPMHtmls(bool forceUpdateLineNumber = true,
                    bool hideParaHeader = false,
                    bool disableAutoNumbering = false);
void numberOriginalHtmls(bool forceUpdateLineNumber = true,
                         bool hideParaHeader = false,
                         bool disableAutoNumbering = true);
void numberMainHtmls(bool forceUpdateLineNumber = true,
                     bool hideParaHeader = false,
                     bool disableAutoNumbering = true);
void numberAttachmentHtmls(bool forceUpdateLineNumber = true,
                           bool hideParaHeader = false,
                           bool disableAutoNumbering = true);

void refreshBodyTexts(const string &kind,
                      int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                      int maxTarget = MAIN_MAX_CHAPTER_NUMBER);
void refreshAttachmentBodyTexts(int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                                int maxTarget = MAIN_MAX_CHAPTER_NUMBER,
                                int minAttachNo = 0, int maxAttachNo = 0);
void validateMainHtmls();
void validateAttachmentHtmls();
void validateParaSizeForAutoNumberingJPMHtmls();

// link fixing for main files
void fixLinksFromMain(bool forceUpdateLink = true);
void appendReverseLinksForMain();

// link fixing for JPM files
void fixLinksFromJPM(bool forceUpdateLink = true);

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdateLink = true);
