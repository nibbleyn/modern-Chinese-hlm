#pragma once
#include "container.hpp"
#include "link.hpp"

using KeyList = vector<string>;
void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true);
void searchKeywordInNoAttachmentFiles(KeyList &keyList, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true);
void testFindFirstInFiles();
