#pragma once

#include "libraryInterface.hpp"

class Numbering {
public:
  enum class COMMAND {
    validateFormatForNumbering,
    validateParaSizeForAutoNumbering,
    addLineNumber,
    fixLinksFromMainFile,
    fixLinksFromAttachmentFile
  };

public:
  Numbering() = default;
  virtual ~Numbering(){};
  COMMAND m_command{COMMAND::addLineNumber};
  string m_kind{MAIN};
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  int m_filenameDigit{TWO_DIGIT_FILENAME};
  FileSet m_fileSet;
  int m_minTarget{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxTarget{MAIN_MAX_CHAPTER_NUMBER};
  int m_minAttachNo{0};
  int m_maxAttachNo{0};
  bool m_forceUpdateLineNumber{true};
  bool m_hideParaHeader{false};
  bool m_disableAutoNumbering{true};
  bool m_fixReturnLink{true};
  CoupledBodyTextWithLink m_bodyText;
  void numberHtmls();

  int m_minReferenceToMain{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToMain{MAIN_MAX_CHAPTER_NUMBER};
  int m_minReferenceToOriginal{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToOriginal{MAIN_MAX_CHAPTER_NUMBER};
  int m_minReferenceToJPM{JPM_MIN_CHAPTER_NUMBER};
  int m_maxReferenceToJPM{JPM_MAX_CHAPTER_NUMBER};
  bool m_forceUpdateLink{true};
  FileSet m_referenceToMainfileSet;
  FileSet m_referenceToOriginalfileSet;
  FileSet m_referenceToJPMfileSet;

protected:
  virtual void dissembleHtmls(CoupledContainer &container) = 0;
  virtual void assembleHtmls(CoupledContainer &container) = 0;
  virtual void runCommandOverEachFile(CoupledContainer &container) = 0;
  void execute();
  void fixReturnLink();
  void setupNumberingStatistics();
  void setupLinkFixingStatistics();
  void outputLinkFixingStatistics();
  int m_storedDebugLevel{0};
  void increaseDebugLevel();
  void restoreDebugLevel();
};

class NumberingNonAttachment : public Numbering {
  void dissembleHtmls(CoupledContainer &container);
  void assembleHtmls(CoupledContainer &container);
  void runCommandOverEachFile(CoupledContainer &container);
};

class NumberingAttachment : public Numbering {
  void dissembleHtmls(CoupledContainer &container);
  void assembleHtmls(CoupledContainer &container);
  void runCommandOverEachFile(CoupledContainer &container);
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

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdateLink = true);
