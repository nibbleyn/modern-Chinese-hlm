#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

enum class ATTACHMENT_TYPE { PERSONAL, REFERENCE, NON_EXISTED };

using AttachmentNumber = pair<int, int>; // chapter number, attachment number
AttachmentNumber getAttachmentNumber(const string &filename);

string getAttachmentTitle(const string &filename);
vector<int> getAttachmentFileListForChapter(const string &referFile,
                                            const string &fromDir);
void testAttachmentOperations();
