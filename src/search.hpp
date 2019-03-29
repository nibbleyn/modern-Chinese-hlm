#pragma once

//#include "container.hpp"
//#include "link.hpp"

#include "numbering.hpp"

using KeyList = vector<string>;
void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true);
void searchKeywordInNoAttachmentFiles(KeyList &keyList, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true);

void searchByTagInNoAttachmentFiles(const string &tag, const string &fileType,
                                    const string &outputFilename,
                                    bool showLineNumberOnly = true);

void searchForImages();
void findFirstInFiles();
