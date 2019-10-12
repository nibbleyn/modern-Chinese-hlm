#pragma once
#include "libraryInterface.hpp"

// main entry of calling above functions
void generateContentTable(int num = 0);

void generateContentTableForAttachments(ATTACHMENT_TYPE type,
                                        bool needToReloadAttachmentList = true);
