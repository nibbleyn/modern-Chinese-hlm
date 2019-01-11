#pragma once
//#include "fileUtil.hpp"
//#include "utf8StringUtil.hpp"
#include "attachmentFiles.hpp"

static const string HTML_SRC_MAIN = "utf8HTML/src/";
static const string HTML_SRC_ORIGINAL = "utf8HTML/src/original/";

static const string HTML_OUTPUT_MAIN = "utf8HTML/output/";
static const string HTML_OUTPUT_ATTACHMENT = "utf8HTML/output/attachment/";
static const string HTML_OUTPUT_ORIGINAL = "utf8HTML/output/original/";

static const string LIST_CONTAINER_FILENAME = "1";
static const string TABLE_CONTAINER_FILENAME = "2";

class Container {
public:
  Container() = default;

protected:
  string m_htmlInputFilePath{""};
  string m_htmlOutputFilePath{""};
  string m_bodyTextInputFilePath{""};
  string m_bodyTextOutputFilePath{""};
};

static const string final = R"(</html>)"; // last line of the html file

/**
 * used for features like removePersonalViewpoints, numbering and linkfixing
 * etc.
 */
class CoupledContainer : public Container {

public:
  CoupledContainer() {
    m_htmlInputFilePath = HTML_SRC_MAIN;
    m_htmlOutputFilePath = HTML_OUTPUT_MAIN;
    m_bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    m_bodyTextOutputFilePath = BODY_TEXT_FIX;
  }
  CoupledContainer(FILE_TYPE fileType) : m_fileType(fileType) {
    if (fileType == FILE_TYPE::ATTACHMENT) {
      m_htmlInputFilePath = HTML_SRC_ATTACHMENT;
      m_htmlOutputFilePath = HTML_OUTPUT_ATTACHMENT;
    } else if (fileType == FILE_TYPE::ORIGINAL) {
      m_htmlInputFilePath = HTML_SRC_ORIGINAL;
      m_htmlOutputFilePath = HTML_OUTPUT_ORIGINAL;
    } else {
      m_htmlInputFilePath = HTML_SRC_MAIN;
      m_htmlOutputFilePath = HTML_OUTPUT_MAIN;
    }
    m_bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    m_bodyTextOutputFilePath = BODY_TEXT_FIX;
  }
  void setFileAndAttachmentNumber(const string &file, int attachNo = 0) {
    m_file = file;
    m_attachmentNumber = attachNo;
  };
  void backupAndOverwriteAllInputHtmlFiles();
  void dissembleFromHTM();
  void assembleBackToHTM(const string &title = "",
                         const string &displayTitle = "");

private:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  string m_file{"01"};
  int m_attachmentNumber{0};

  string getBodyTextFilePrefix();
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
  void clearBodyTextFile();
  void appendParagraphInBodyText(const string &text);
  string getOutputFilePath() {
    return m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;
  }

protected:
  string m_outputFilename{"output"};
};

/**
 * used for features like reConstructStory findFirstInNoAttachmentFiles etc.
 */
class ListContainer : public GenericContainer {
public:
  ListContainer() = default;
  ListContainer(const string &filename) : GenericContainer(filename) {}

private:
  string getInputFileName() const override { return LIST_CONTAINER_FILENAME; }
};

/**
 * used for features like updateIndexTable etc.
 */
class TableContainer : public GenericContainer {
public:
  TableContainer() = default;

private:
  string getInputFileName() const override { return TABLE_CONTAINER_FILENAME; }
};

void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo);
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo);
