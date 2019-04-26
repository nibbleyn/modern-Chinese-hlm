#pragma once

#include "linkFix.hpp"

void refreshBodyTexts(const string &kind, int minTarget = 1,
                      int maxTarget = 80);
void refreshAttachmentBodyTexts(int minTarget = 1, int maxTarget = 80,
                                int minAttachNo = 1, int maxAttachNo = 50);

void numberJPMHtmls(int num, bool forceUpdate = true,
                    bool hideParaHeader = false);
void numberOriginalHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberMainHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberAttachmentHtmls(bool forceUpdate = true,
                           bool hideParaHeader = false);
