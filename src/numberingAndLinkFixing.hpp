#pragma once

#include "libraryInterface.hpp"

class Numbering {
public:
  Numbering() = default;
  virtual ~Numbering(){};
  int m_command{3};
  string m_kind{MAIN};
  int m_filenameDigit{TWO_DIGIT_FILENAME};
  FileSet m_fileSet;
  int m_minTarget{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxTarget{MAIN_MAX_CHAPTER_NUMBER};
  int m_minAttachNo{0};
  int m_maxAttachNo{0};
  bool m_forceUpdateLineNumber{true};
  bool m_hideParaHeader{false};
  bool m_disableAutoNumbering{true};
  CoupledBodyTextWithLink m_bodyText;
  void numberHtmls();

protected:
  virtual void dissembleHtmls(CoupledContainer &container) = 0;
  virtual void assembleHtmls(CoupledContainer &container) = 0;
  virtual void runOverEachFile(CoupledContainer &container) = 0;
  void execute();
};

class NumberingNonAttachment : public Numbering {
  void dissembleHtmls(CoupledContainer &container);
  void assembleHtmls(CoupledContainer &container);
  void runOverEachFile(CoupledContainer &container);
};

class NumberingAttachment : public Numbering {
  void dissembleHtmls(CoupledContainer &container);
  void assembleHtmls(CoupledContainer &container);
  void runOverEachFile(CoupledContainer &container);
};

void numberJPMHtmls(int num, bool forceUpdateLineNumber = true,
                    bool hideParaHeader = false,
                    bool disableAutoNumbering = false);
void numberOriginalHtmls(int num, bool forceUpdateLineNumber = true,
                         bool hideParaHeader = false,
                         bool disableAutoNumbering = true);
void numberMainHtmls(int num, bool forceUpdateLineNumber = true,
                     bool hideParaHeader = false,
                     bool disableAutoNumbering = true);
void numberAttachmentHtmls(int num, bool forceUpdateLineNumber = true,
                           bool hideParaHeader = false,
                           bool disableAutoNumbering = true);

void refreshBodyTexts(const string &kind,
                      int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                      int maxTarget = MAIN_MAX_CHAPTER_NUMBER);
void refreshAttachmentBodyTexts(int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                                int maxTarget = MAIN_MAX_CHAPTER_NUMBER,
                                int minAttachNo = 0, int maxAttachNo = 0);

// link fixing for main files
void fixLinksFromMain(bool forceUpdateLink = true);

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdateLink = true);
