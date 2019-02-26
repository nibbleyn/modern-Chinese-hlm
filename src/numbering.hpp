#pragma once
#include "container.hpp"
#include "coupledBodyText.hpp"

void numberJPMHtmls(bool forceUpdate = true, bool hidden = false);
void numberOriginalHtmls(bool forceUpdate = true, bool hidden = false);
void numberMainHtmls(bool forceUpdate = true, bool hidden = false);
void numberAttachmentHtmls(bool forceUpdate = true, bool hidden = false);

void reformatTxtFilesForReader();
void autoSplitBodyText(const string &fileType);
