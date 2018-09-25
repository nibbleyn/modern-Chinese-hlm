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

void addLineNumber(const string &inputFile, const string &outputFile,
                   const string &separatorColor);

void addLineNumbersForOriginalHtml(int minTarget, int maxTarget);
void addLineNumbersForMainHtml(int minTarget, int maxTarget);
void addLineNumbersForAttachmentHtml(int minTarget, int maxTarget,
                                     int minAttachNo, int maxAttachNo);
void NumberingOriginalHtml(int minTarget, int maxTarget);
void NumberingMainHtml(int minTarget, int maxTarget);
void NumberingAttachmentHtml(int minTarget, int maxTarget, int minAttachNo,
                             int maxAttachNo);
