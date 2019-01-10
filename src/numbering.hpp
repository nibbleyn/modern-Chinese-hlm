#pragma once
#include "bodyText.hpp"
#include "container.hpp"
#include "fileUtil.hpp"

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);

void numberOriginalHtmls(bool hidden = false);
void numberMainHtmls(bool hidden = false);
void numberAttachmentHtmls(bool hidden = false);

void reformatTxtFilesForReader();
void autoSplitBodyText(const string &fileType);
