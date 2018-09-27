#pragma once
#include "hlmFileType.hpp"

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color);

using ParaStruct = std::tuple<int, int, int>;
ParaStruct getNumberOfPara(const string &referFile);

void numberOriginalHtmls();
void numberMainHtmls();
void numberAttachmentHtmls();
