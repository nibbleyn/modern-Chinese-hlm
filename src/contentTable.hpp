#pragma once
#include "libraryInterface.hpp"

enum class TABLE_TYPE { MAIN, ORIGINAL, JPM, IMAGE, POEMS, ATTACHMENT };
// main entry of calling above functions
void generateContentTable(TABLE_TYPE type,
                          ATTACHMENT_TYPE attType = ATTACHMENT_TYPE::PERSONAL,
                          bool needToReloadAttachmentList = true);
