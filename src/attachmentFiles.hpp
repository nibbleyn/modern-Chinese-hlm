#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

string getAttachmentTitle(const string &filename);
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir);
void testAttachmentOperations();
