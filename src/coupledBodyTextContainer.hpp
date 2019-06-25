#pragma once
#include "attachmentTable.hpp"
#include "container.hpp"

const string strongTitleBeginChars = R"(<strong unhidden>)";
const string strongTitleEndChars = R"(</strong>)";
const string sampTitleBeginChars = R"(<samp unhidden>)";
const string sampTitleEndChars = R"(</samp>)";

/**
 * used for features like removePersonalViewpoints, numbering and linkfixing
 * etc.
 */
class CoupledBodyTextContainer : public Container {
public:
  // imported attachment list
  static AttachmentList refAttachmentTable;

  static void backupAndOverwriteAllInputHtmlFiles();

public:
  CoupledBodyTextContainer() = default;

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachmentNumber = attachNo;
  }

  void setFileAndAttachmentNumber(int file, int attachNo = 0) {
    m_file = getChapterNameByTargetKind(ATTACHMENT_TYPE_HTML_TARGET, file);
    m_attachmentNumber = attachNo;
  }

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

  // dissemble/assemble before/after working on bodyText
  void dissembleFromHTM();
  void assembleBackToHTM(const string &title = emptyString,
                         const string &displayTitle = emptyString);
  AttachmentNumberList getAttachmentFileList(int minAttachNo = 0,
                                             int maxAttachNo = 0);

  // fix return link after fix links in main files
  void fixReturnLinkForAttachmentFile();

  // to create content table
  void fetchOriginalAndTranslatedTitles();
  string getOriginalTitle() { return m_originalTitle; };
  string getTranslatedTitle() { return m_translatedTitle; };

  // tools to fix headers
  void makeSingleLineHeaderAndFooter();
  void fixHeaderAndFooter();

private:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};

  string m_file{emptyString};
  int m_attachmentNumber{0};

  // to create content table
  string m_originalTitle{emptyString};
  string m_translatedTitle{emptyString};

  string getBodyTextFilePrefix();
  string getInputHtmlFilePath() override;
  string getoutputHtmlFilepath();
  string getBodyTextFilePath();
};
