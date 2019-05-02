#pragma once
#include "container.hpp"
#include "coupledLink.hpp"

const string strongTitleBeginChars = R"(<strong unhidden>)";
const string strongTitleEndChars = R"(</strong>)";
const string sampTitleBeginChars = R"(<samp unhidden>)";
const string sampTitleEndChars = R"(</samp>)";

/**
 * used for features like removePersonalViewpoints, numbering and linkfixing
 * etc.
 */
class CoupledContainer : public Container {
public:
  static void backupAndOverwriteAllInputHtmlFiles();

public:
  CoupledContainer() = default;

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachmentNumber = attachNo;
  };

  void setFileType(FILE_TYPE fileType) {
    m_fileType = fileType;
    if (fileType == FILE_TYPE::ATTACHMENT) {
      m_htmlInputFilePath = HTML_SRC_ATTACHMENT;
      m_htmlOutputFilePath = HTML_OUTPUT_ATTACHMENT;
    } else if (fileType == FILE_TYPE::ORIGINAL) {
      m_htmlInputFilePath = HTML_SRC_ORIGINAL;
      m_htmlOutputFilePath = HTML_OUTPUT_ORIGINAL;
    } else if (fileType == FILE_TYPE::JPM) {
      m_htmlInputFilePath = HTML_SRC_JPM;
      m_htmlOutputFilePath = HTML_OUTPUT_JPM;
    }
  }

  void makeSingleLineHeaderAndFooter();
  void fixHeaderAndFooter();
  void fetchOriginalAndTranslatedTitles();
  string getOriginalTitle() { return m_originalTitle; };
  string getTranslatedTitle() { return m_translatedTitle; };
  void fixReturnLinkForAttachmentFile();
  using AttachmentNumberList = set<int>;
  AttachmentNumberList getAttachmentFileList(int minAttachNo = 0,
                                             int maxAttachNo = 0);
  AttachmentNumberList getAttachmentFileListForChapter(const string &fromDir);
  void dissembleFromHTM();
  void assembleBackToHTM(const string &title = emptyString,
                         const string &displayTitle = emptyString);

private:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  string m_file{emptyString};
  int m_attachmentNumber{0};
  string m_originalTitle{emptyString};
  string m_translatedTitle{emptyString};

  string getBodyTextFilePrefix();
  string getInputHtmlFilePath();
  string getoutputHtmlFilepath();
  string getBodyTextFilePath();
};
