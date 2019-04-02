#pragma once

//#include "container.hpp"
//#include "coupledBodyTextWithLink.hpp"

#include "linkFix.hpp"

void numberJPMHtmls(int num, bool forceUpdate = true,
                    bool hideParaHeader = false);
void numberOriginalHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberMainHtmls(bool forceUpdate = true, bool hideParaHeader = false);
void numberAttachmentHtmls(bool forceUpdate = true,
                           bool hideParaHeader = false);
