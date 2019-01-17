#pragma once
#include "container.hpp"
#include "coupledBodyText.hpp"

void numberOriginalHtmls(bool hidden = false);
void numberMainHtmls(bool hidden = false);
void numberAttachmentHtmls(bool hidden = false);

void reformatTxtFilesForReader();
void autoSplitBodyText(const string &fileType);
