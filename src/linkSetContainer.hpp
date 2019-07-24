#pragma once
#include "container.hpp"

static const string HTML_PREFIX = R"(container)";

class LinkSetContainer : public Container {
public:
  LinkSetContainer() = default;
  LinkSetContainer(const string &outputHtmlFilename)
      : Container(outputHtmlFilename) {
    m_inputHtmlDir = HTML_NON_COUPLED_CONTAINER_PATH;
    m_outputHtmlDir = HTML_OUTPUT_MAIN;
    m_bodyTextDir = HTML_NON_COUPLED_CONTAINER_PATH;
  }
  virtual ~LinkSetContainer(){};

  virtual void
  outputToBodyTextFromLinkList(const string &units = defaultUnit) = 0;
  string getInputHtmlFilePath() override {
    m_inputHtmlFilename = HTML_PREFIX + m_bodyTextFilename;
    return Container::getInputHtmlFilePath();
  }
  void clearExistingBodyText();

  void createParaListFrom(int first, int incremental, int max = 0);

  void addOneParaHeaderPosition(int pos) {
    m_paraHeaderPositionSet.push_back(pos);
  }
  void hideParaHeaders() { m_hideParaHeaders = true; }
  void setMaxTarget(int num) { m_maxTarget = num; }
  void setMaxTargetAsSetSize() { m_maxTarget = m_linkStringSet.size(); }
  size_t getLinkStringSetSize() { return m_linkStringSet.size(); }
  void assignLinkStringSet(LinkStringSet stringSet) {
    m_linkStringSet = stringSet;
  }
  void clearLinkStringSet() { m_linkStringSet.clear(); }
  void addLinkToLinkStringSet(AttachmentNumber num, const string &link,
                              ParaLineNumber pl = make_pair(0, 0)) {
    auto fullPos = make_pair(num, pl);
    m_linkStringSet[fullPos] = link;
  }

  void printParaHeaderTable();

protected:
  bool m_hideParaHeaders{false};
  using ParaHeaderPositionSet = vector<int>;
  ParaHeaderPositionSet m_paraHeaderPositionSet;
  LinkStringSet m_linkStringSet;
  int m_maxTarget{0};
};

static const string LIST_CONTAINER_FILENAME = R"(1)";

class ListContainer : public LinkSetContainer {
public:
  ListContainer() = default;
  ListContainer(const string &filename) : LinkSetContainer(filename) {
    m_bodyTextFilename = LIST_CONTAINER_FILENAME;
  }

  void appendParagraphInBodyText(const string &text);
  void appendParagrapHeader(const string &header);

  void outputToBodyTextFromLinkList(const string &units = defaultUnit) override;
};

static constexpr const char *TABLE_CONTAINER_FILENAME = R"(2)";
static constexpr const char *TABLE_CONTAINER_FILENAME_SMALLER_FONT = R"(3)";

class TableContainer : public LinkSetContainer {
  static const string BODY_TEXT_STARTER;
  static const string BODY_TEXT_DESSERT;

public:
  TableContainer() = default;
  TableContainer(const string &filename) : LinkSetContainer(filename) {
    m_bodyTextFilename = TABLE_CONTAINER_FILENAME;
  }

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
  void setConstantBodyTextFileName(const string &filename) {
    m_bodyTextFilename = filename;
  }
  void outputToBodyTextFromLinkList(const string &units = defaultUnit) override;

private:
  bool m_enableAddExistingFrontLinks{false};
};
