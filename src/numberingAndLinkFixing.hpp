#pragma once

#include "libraryInterface.hpp"

void refreshBodyTexts(const string &kind,
                      int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                      int maxTarget = MAIN_MAX_CHAPTER_NUMBER);
void refreshAttachmentBodyTexts(int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                                int maxTarget = MAIN_MAX_CHAPTER_NUMBER,
                                int minAttachNo = MIN_ATTACHMENT_NUMBER,
                                int maxAttachNo = MAX_ATTACHMENT_NUMBER);

void numberJPMHtmls(int num, bool forceUpdateLineNumber = true,
                    bool hideParaHeader = false);
void numberOriginalHtmls(bool forceUpdateLineNumber = true,
                         bool hideParaHeader = false);
void numberMainHtmls(bool forceUpdateLineNumber = true,
                     bool hideParaHeader = false);
void numberAttachmentHtmls(bool forceUpdateLineNumber = true,
                           bool hideParaHeader = false);

// link fixing for main files
void fixLinksFromMain(bool forceUpdateLink = true);

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdateLink = true);
