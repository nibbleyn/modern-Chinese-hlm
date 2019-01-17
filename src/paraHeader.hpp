#include "utf8StringUtil.hpp"

static const string paraTab = R"(name=")"; // of each paragraph
string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);
