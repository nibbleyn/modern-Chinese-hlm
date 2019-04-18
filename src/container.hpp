#pragma once

//#include "attachmentFiles.hpp"
//#include "lineNumber.hpp"
//#include "paraHeader.hpp"

#include "lineNumber.hpp"

static const string LIST_CONTAINER_FILENAME = "1";
static const string TABLE_CONTAINER_FILENAME = "2";
static const string TABLE_CONTAINER_FILENAME_SMALLER_FONT = "3";

class Container {
public:
  Container() = default;
  virtual ~Container(){};

protected:
  string m_htmlInputFilePath{HTML_SRC_MAIN};
  string m_htmlOutputFilePath{HTML_OUTPUT_MAIN};
  string m_bodyTextInputFilePath{BODY_TEXT_OUTPUT};
  string m_bodyTextOutputFilePath{BODY_TEXT_FIX};
  virtual string getInputHtmlFilePath() = 0;
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
  void makeSingleLineHeaderAndFooter();
  void fixHeaderAndFooter();
  void fetchOriginalAndTranslatedTitles();
  string getOriginalTitle() { return m_originalTitle; };
  string getTranslatedTitle() { return m_translatedTitle; };
  void dissembleFromHTM();
  void assembleBackToHTM(const string &title = "",
                         const string &displayTitle = "");

private:
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  string m_file{"01"};
  int m_attachmentNumber{0};
  string m_originalTitle{""};
  string m_translatedTitle{""};

  string getBodyTextFilePrefix();
  string getInputHtmlFilePath() {
    string attachmentPart{""};
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      attachmentPart =
          attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    return m_htmlInputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
           attachmentPart + HTML_SUFFIX;
  }
  string getoutputHtmlFilepath() {
    string attachmentPart{""};
    if (m_fileType == FILE_TYPE::ATTACHMENT)
      attachmentPart =
          attachmentFileMiddleChar + TurnToString(m_attachmentNumber);
    return m_htmlOutputFilePath + getHtmlFileNamePrefix(m_fileType) + m_file +
           attachmentPart + HTML_SUFFIX;
  }
  string getBodyTextFilePath() {
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
  string getOutputBodyTextFilePath() {
    return m_bodyTextOutputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  }

protected:
  string m_outputFilename{"output"};
  string getInputHtmlFilePath() {
    return m_htmlInputFilePath + getInputFileName() + HTML_SUFFIX;
  }
  string getoutputHtmlFilepath() {
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
  void clearExistingBodyText();
  void appendParagraphInBodyText(const string &text);
  void appendParagrapHeader(const string &header);

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
  void addExistingFrontParagraphs();
  void insertFrontParagrapHeader(int totalPara,
                                 const string &units = defaultUnit);
  void insertMiddleParagrapHeader(bool enterLastPara, int seqOfPara,
                                  int startParaNo, int endParaNo, int totalPara,
                                  int preTotalPara,
                                  const string &units = defaultUnit);
  void insertBackParagrapHeader(int seqOfPara, int totalPara,
                                const string &units = defaultUnit);
  void appendLeftParagraphInBodyText(const string &text);
  void appendRightParagraphInBodyText(
      const string &text); // text could be null for last right column
  void finishBodyTextFile();
  void setInputFileName(const string &name) { m_filename = name; }

private:
  string getInputFileName() const override { return m_filename; }
  string m_filename{TABLE_CONTAINER_FILENAME};
};
