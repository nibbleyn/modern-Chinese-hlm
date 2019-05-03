#pragma once
#include "libraryInterface.hpp"

void generateContentTableForMainHtmls();
void generateContentTableForOriginalHtmls();
void generateContentTableForJPMHtmls();
void generateContentTableForReferenceAttachments(
    bool needToReloadAttachmentList = true);
void generateContentTableForPersonalAttachments(
    bool needToReloadAttachmentList = true);
