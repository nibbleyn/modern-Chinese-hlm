#pragma once
#include "container.hpp"
#include "link.hpp"

void searchKeywordInNoAttachmentFiles(const string &key, const string &fileType,
                                      const string &outputFilename,
                                      bool searchForAll = true);
void testFindFirstInFiles();
