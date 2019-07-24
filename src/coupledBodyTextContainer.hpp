#pragma once
#include "container.hpp"
#include "coupledLink.hpp"

const string strongTitleBeginChars = R"(<strong unhidden>)";
const string strongTitleEndChars = R"(</strong>)";
const string sampTitleBeginChars = R"(<samp unhidden>)";
const string sampTitleEndChars = R"(</samp>)";

class CoupledBodyTextContainer : public Container {
public:
  // imported attachment list
  static AttachmentList refAttachmentTable;

public:
  CoupledBodyTextContainer() = default;
  CoupledBodyTextContainer(const string &outputHtmlFilename) = delete;

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachmentNumber = attachNo;
    setBodyTextFileName();
    setInputHtmlFileName();
    m_outputHtmlFilename = m_inputHtmlFilename;
  }

  void setFileAndAttachmentNumber(int file, int attachNo = 0) {
    m_file = getChapterNameByTargetKind(ATTACHMENT_TYPE_HTML_TARGET, file);
    m_attachmentNumber = attachNo;
    setBodyTextFileName();
    setInputHtmlFileName();
    m_outputHtmlFilename = m_inputHtmlFilename;
  }

  void setFileType(FILE_TYPE fileType) {
    m_fileType = fileType;
    if (fileType == FILE_TYPE::ATTACHMENT) {
      m_inputHtmlDir = HTML_SRC_ATTACHMENT;
      m_outputHtmlDir = HTML_OUTPUT_ATTACHMENT;
    } else if (fileType == FILE_TYPE::ORIGINAL) {
      m_inputHtmlDir = HTML_SRC_ORIGINAL;
      m_outputHtmlDir = HTML_OUTPUT_ORIGINAL;
    } else if (fileType == FILE_TYPE::JPM) {
      m_inputHtmlDir = HTML_SRC_JPM;
      m_outputHtmlDir = HTML_OUTPUT_JPM;
    }
  }

  AttachmentNumberList getAttachmentFileList(int minAttachNo = 0,
                                             int maxAttachNo = 0);

  void setBackupFilenameList(FileSet files, bool wholeFolder = true,
                             bool attachmentRequired = false);
  void backupAndOverwriteInputHtmlFiles();

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

  string getBodyTextFilePrefix() {
    if (m_fileType == FILE_TYPE::ORIGINAL)
      return ORIGINAL_BODYTEXT_PREFIX;
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      return ATTACHMENT_BODYTEXT_PREFIX;
    if (m_fileType == FILE_TYPE::JPM)
      return JPM_BODYTEXT_PREFIX;
    return MAIN_BODYTEXT_PREFIX;
  }
  void setInputHtmlFileName();
  void setBodyTextFileName();

  using BackupFilenameList = set<string>;
  BackupFilenameList m_backupFilenameList;
};
