#pragma once

#include "coupledLinkFix.hpp"

void refreshBodyTexts(const string &kind,
                      int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                      int maxTarget = MAIN_MAX_CHAPTER_NUMBER);
void refreshAttachmentBodyTexts(int minTarget = MAIN_MIN_CHAPTER_NUMBER,
                                int maxTarget = MAIN_MAX_CHAPTER_NUMBER,
                                int minAttachNo = MIN_ATTACHMENT_NUMBER,
                                int maxAttachNo = MAX_ATTACHMENT_NUMBER);

void numberJPMHtmls(int num, bool forceUpdate = true,
                    bool hideParaHeader = false);
void numberOriginalHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberMainHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberAttachmentHtmls(bool forceUpdate = true,
                           bool hideParaHeader = false);
