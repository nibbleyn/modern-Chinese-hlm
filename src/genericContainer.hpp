#pragma once

#include "container.hpp"
#include "paraHeader.hpp"

static const string HTML_CONTAINER = R"(container/container)";
static const string BODY_TEXT_CONTAINER = R"(container/)";

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
  void assembleBackToHTM(const string &title = emptyString,
                         const string &displayTitle = emptyString);
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

static const string LIST_CONTAINER_FILENAME = R"(1)";

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

static const string TABLE_CONTAINER_FILENAME = R"(2)";
static const string TABLE_CONTAINER_FILENAME_SMALLER_FONT = R"(3)";

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
  // text could be null for last right column
  void appendRightParagraphInBodyText(const string &text);
  void finishBodyTextFile();
  void setInputFileName(const string &name) { m_filename = name; }

private:
  string getInputFileName() const override { return m_filename; }
  string m_filename{TABLE_CONTAINER_FILENAME};
};
