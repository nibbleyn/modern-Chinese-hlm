#pragma once
#include "hlmContainer.hpp"
#include "hlmFileType.hpp"

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);

using ParaStruct = std::tuple<int, int, int>;
ParaStruct getNumberOfPara(const string &referFile);

void numberOriginalHtmls(bool hidden = false);
void numberMainHtmls(bool hidden = false);
void numberAttachmentHtmls(bool hidden = false);

void reformatTxtFilesForReader();
