#pragma once
#include "container.hpp"
#include "coupledBodyTextWithLink.hpp"

void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo);
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo);

// link fixing for main files
void fixLinksFromMain(bool forceUpdate = true);

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdate = true);
