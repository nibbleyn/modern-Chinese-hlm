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
    m_inputBodyTextDir = HTML_NON_COUPLED_CONTAINER_PATH;
  }
  virtual ~LinkSetContainer(){};

  virtual void
  outputToBodyTextFromLinkList(const string &units = defaultUnit) = 0;
  virtual string getInputBodyTextFileName() const = 0;
  string getInputHtmlFilePath() override {
    m_inputHtmlFilename = HTML_PREFIX + getInputBodyTextFileName();
    return Container::getInputHtmlFilePath();
  }
  string getBodyTextFilePath() override {
    return m_inputBodyTextDir + getInputBodyTextFileName() +
           BODY_TEXT_SUFFIX;
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

  void printParaHeaderTable() {
    if (not m_linkStringSet.empty()) {
      METHOD_OUTPUT << "m_linkStringSet:" << endl;
    } else
      METHOD_OUTPUT << "no entry in m_linkStringSet." << endl;
    for (const auto &element : m_linkStringSet) {
      METHOD_OUTPUT << element.first.first.first << "        "
                    << element.first.first.second << "        "
                    << element.first.second.first << "        "
                    << element.first.second.second << "        "
                    << element.second << endl;
    }
  }

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
  ListContainer(const string &filename) : LinkSetContainer(filename) {}

  void appendParagraphInBodyText(const string &text);
  void appendParagrapHeader(const string &header);

  void outputToBodyTextFromLinkList(const string &units = defaultUnit) override;

private:
  string getInputBodyTextFileName() const override {
    return LIST_CONTAINER_FILENAME;
  }
};

static constexpr const char *TABLE_CONTAINER_FILENAME = R"(2)";
static constexpr const char *TABLE_CONTAINER_FILENAME_SMALLER_FONT = R"(3)";

class TableContainer : public LinkSetContainer {
  static const string BODY_TEXT_STARTER;
  static const string BODY_TEXT_DESSERT;

public:
  TableContainer() = default;
  TableContainer(const string &filename) : LinkSetContainer(filename) {
    m_inputBodyTextFilename = TABLE_CONTAINER_FILENAME;
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
  void setInputBodyTextFilename(const string &filename) {
    m_inputBodyTextFilename = filename;
  }
  void outputToBodyTextFromLinkList(const string &units = defaultUnit) override;

private:
  string getInputBodyTextFileName() const override {
    return m_inputBodyTextFilename;
  }
  bool m_enableAddExistingFrontLinks{false};
};
