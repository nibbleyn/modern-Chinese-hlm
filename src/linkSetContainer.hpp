#pragma once

#include "container.hpp"

static const string HTML_CONTAINER = R"(container/container)";
static const string BODY_TEXT_CONTAINER = R"(container/)";

using ParaHeaderPositionSet = vector<int>;

class LinkSetContainer : public Container {
public:
  LinkSetContainer() = default;
  LinkSetContainer(const string &filename) : m_outputFilename(filename) {
    m_htmlInputFilePath = HTML_CONTAINER;
    m_htmlOutputFilePath = HTML_OUTPUT_MAIN;
    m_bodyTextInputFilePath = BODY_TEXT_CONTAINER;
    m_bodyTextOutputFilePath = BODY_TEXT_CONTAINER;
  }
  virtual ~LinkSetContainer(){};
  virtual string getInputFileName() const = 0;
  void assembleBackToHTM(const string &title = emptyString,
                         const string &displayTitle = emptyString);
  string getOutputFilePath() {
    return m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;
  }
  void clearExistingBodyText();

  string getOutputBodyTextFilePath() {
    return m_bodyTextOutputFilePath + getInputFileName() + BODY_TEXT_SUFFIX;
  }
  void createParaListFrom(int first, int incremental, int max = 0);
  void addOneParaHeaderPosition(int pos) {
    m_paraHeaderPositionSet.push_back(pos);
  }
  void hideParaHeaders() { m_hideParaHeaders = true; }
  void setMaxTarget(int num) { m_maxTarget = num; }
  void setMaxTargetAsSetSize() { m_maxTarget = m_linkStringSet.size(); }
  void assignLinkStringSet(LinkStringSet stringSet) {
    m_linkStringSet = stringSet;
  }
  void clearLinkStringSet() { m_linkStringSet.clear(); }
  void addLinkToLinkStringSet(const string &link) {
    m_linkStringSet.insert(link);
  }
  virtual void
  outputToBodyTextFromLinkList(const string &units = defaultUnit) = 0;

protected:
  string m_outputFilename{"output"};
  string getInputHtmlFilePath() {
    return m_htmlInputFilePath + getInputFileName() + HTML_SUFFIX;
  }
  string getoutputHtmlFilepath() {
    return m_htmlOutputFilePath + m_outputFilename + HTML_SUFFIX;
  }
  bool m_hideParaHeaders{false};
  ParaHeaderPositionSet m_paraHeaderPositionSet;
  LinkStringSet m_linkStringSet;
  int m_maxTarget{0};
};

static const string LIST_CONTAINER_FILENAME = R"(1)";

/**
 * used for features like reConstructStory findFirstInNoAttachmentFiles etc.
 */
class ListContainer : public LinkSetContainer {
public:
  ListContainer() = default;
  ListContainer(const string &filename) : LinkSetContainer(filename) {}
  // process bodyText change directly, instead of thru CoupledBodyText
  void appendParagraphInBodyText(const string &text);
  void appendParagrapHeader(const string &header);
  void outputToBodyTextFromLinkList(const string &units = defaultUnit);

private:
  string getInputFileName() const override { return LIST_CONTAINER_FILENAME; }
};

static const string TABLE_CONTAINER_FILENAME = R"(2)";
static const string TABLE_CONTAINER_FILENAME_SMALLER_FONT = R"(3)";

/**
 * used for features like updateIndexTable etc.
 */
class TableContainer : public LinkSetContainer {
  static const string BODY_TEXT_STARTER;
  static const string BODY_TEXT_DESSERT;

public:
  TableContainer() = default;
  TableContainer(const string &filename) : LinkSetContainer(filename) {}

  // process bodyText change directly, instead of thru CoupledBodyText
  void addExistingFrontLinks();
  void enableAddExistingFrontLinks() { m_enableAddExistingFrontLinks = true; }
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
  void outputToBodyTextFromLinkList(const string &units = defaultUnit);

private:
  string getInputFileName() const override { return m_filename; }
  string m_filename{TABLE_CONTAINER_FILENAME};
  bool m_enableAddExistingFrontLinks{false};
};
