#pragma once
#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);

static const string defaultUnit = R"(回)";
static const string attachmentUnit = R"(篇)";
static const string searchUnit = R"(条)";
static const string numberingUnit = R"(段)";

string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara,
                                      const string &units = defaultUnit,
                                      bool hidden = false);

string insertParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                    int startParaNo, int endParaNo,
                                    int totalPara, int preTotalPara,
                                    const string &color,
                                    const string &units = defaultUnit,
                                    bool hidden = false, bool lastPara = false);

string fixBackParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                     int totalPara, const string &color,
                                     const string &units = defaultUnit,
                                     bool hidden = false);
