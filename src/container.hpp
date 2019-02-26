#pragma once
#include "attachmentFiles.hpp"

static const string HTML_SRC_MAIN = "utf8HTML/src/";
static const string HTML_SRC_ORIGINAL = "utf8HTML/src/original/";
static const string HTML_SRC_JPM = "utf8HTML/src/JPM/";

static const string HTML_OUTPUT_MAIN = "utf8HTML/output/";
static const string HTML_OUTPUT_ATTACHMENT = "utf8HTML/output/attachment/";
static const string HTML_OUTPUT_ORIGINAL = "utf8HTML/output/original/";
static const string HTML_OUTPUT_JPM = "utf8HTML/output/JPM/";

static const string LIST_CONTAINER_FILENAME = "1";
static const string TABLE_CONTAINER_FILENAME = "2";

class Container {
public:
  Container() = default;
  virtual ~Container(){};

protected:
  string m_htmlInputFilePath{HTML_SRC_MAIN};
  string m_htmlOutputFilePath{HTML_OUTPUT_MAIN};
  string m_bodyTextInputFilePath{BODY_TEXT_OUTPUT};
  string m_bodyTextOutputFilePath{BODY_TEXT_FIX};
  virtual string getInputHtmlFile() = 0;
};

/**
 * used for features like removePersonalViewpoints, numbering and linkfixing
 * etc.
 */
class CoupledContainer : public Container {

public:
  CoupledContainer() = default;
  CoupledContainer(FILE_TYPE fileType) : m_fileType(fileType) {
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

  static void backupAndOverwriteAllInputHtmlFiles();

  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachmentNumber = attachNo;
  };
  void dissembleFromHTM();
  void assembleBackToHTM(const string &title = "",
                         const string &displayTitle = "");

private:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  string m_file{"01"};
  int m_attachmentNumber{0};

  string getBodyTextFilePrefix();
  string getInputHtmlFile() {
    string attachmentPart{""};
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      attachmentPart =
          attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    return m_htmlInputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
           attachmentPart + HTML_SUFFIX;
  }
  string getoutputHtmlFile() {
    string attachmentPart{""};
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      attachmentPart =
          attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    return m_htmlOutputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
           attachmentPart + HTML_SUFFIX;
  }
  string getBodyTextFile() {
    string attachmentPart{""};
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      attachmentPart =
          attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    return m_bodyTextInputFilePath + getBodyTextFilePrefix() + m_file +
           attachmentPart + BODY_TEXT_SUFFIX;
  }
};

static const string defaultTitle = "XXX";
static const string defaultDisplayTitle = "YYY";
static const string HTML_CONTAINER = "container/container";
static const string BODY_TEXT_CONTAINER = "container/";

class GenericContainer : public Container {
public:
  GenericContainer() = default;
  GenericContainer(const string &filename) : m_outputFilename(filename) {
    m_htmlInputFilePath = HTML_CONTAINER;
    m_htmlOutputFilePath = HTML_OUTPUT_MAIN;
    m_bodyTextInputFilePath = BODY_TEXT_CONTAINER;
    m_bodyTextOutputFilePath = BODY_TEXT_CONTAINER;
  }
  virtual ~GenericContainer(){};
  virtual string getInputFileName() const = 0;
  void assembleBackToHTM(const string &title = "",
                         const string &displayTitle = "");
  string getOutputFilePath() {
    return m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;
  }

protected:
  string m_outputFilename{"output"};
  string getOutputBodyTextFile() {
    return m_bodyTextOutputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  }
  string getInputHtmlFile() {
    return m_htmlInputFilePath + getInputFileName() + HTML_SUFFIX;
  }
  string getoutputHtmlFile() {
    return m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;
  }
};

/**
 * used for features like reConstructStory findFirstInNoAttachmentFiles etc.
 */
class ListContainer : public GenericContainer {
public:
  ListContainer() = default;
  ListContainer(const string &filename) : GenericContainer(filename) {}
  // process bodyText change directly, instead of thru CoupledBodyText
  void initBodyTextFile();
  void appendParagraphInBodyText(const string &text);

private:
  string getInputFileName() const override { return LIST_CONTAINER_FILENAME; }
};

/**
 * used for features like updateIndexTable etc.
 */
class TableContainer : public GenericContainer {
  static const string BODY_TEXT_STARTER;
  static const string BODY_TEXT_DESSERT;

public:
  TableContainer() = default;
  TableContainer(const string &filename) : GenericContainer(filename) {}

  // process bodyText change directly, instead of thru CoupledBodyText
  void initBodyTextFile();
  void appendLeftParagraphInBodyText(const string &text);
  void appendRightParagraphInBodyText(
      const string &text); // text could be null for last right column
  void finishBodyTextFile();

private:
  string getInputFileName() const override { return TABLE_CONTAINER_FILENAME; }
};

void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo);
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo);
