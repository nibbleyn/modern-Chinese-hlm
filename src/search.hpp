#pragma once
#include "libraryInterface.hpp"

static constexpr int SEARCH_IN_MAIN = 1;
static constexpr int SEARCH_IN_ORIGINAL = 2;
static constexpr int SEARCH_IN_JPM = 3;

void search(const string &key, int num = SEARCH_IN_MAIN,
            const string &outputFilename = "");

class Searcher {
public:
  using KeyList = vector<string>;

public:
  Searcher() = default;
  virtual ~Searcher(){};

  string m_kind{MAIN};
  string m_key;
  KeyList m_keyList;
  string m_outputFilename;
  void searchOnlyForFirst() { m_searchForAll = false; }
  int m_minTarget{MAIN_MIN_CHAPTER_NUMBER};
  int m_maxTarget{MAIN_MAX_CHAPTER_NUMBER};
  int m_minAttachNo{0};
  int m_maxAttachNo{0};
  void runSearchingOverFiles();

protected:
  void execute();
  void outputSearchResult();
  FILE_TYPE m_fileType{FILE_TYPE::MAIN};
  FileSet m_fileSet;
  bool m_asList{false};
  bool m_searchForAll{true};
  unique_ptr<LinkSetContainer> m_containerPtr{nullptr};
  CoupledBodyTextWithLink m_bodyText;
  virtual void runSearchingOverEachFile() = 0;
};

class NonAttachmentSearcher : public Searcher {
  void runSearchingOverEachFile();
};

class AttachmentSearcher : public Searcher {
  void runSearchingOverEachFile();
};
